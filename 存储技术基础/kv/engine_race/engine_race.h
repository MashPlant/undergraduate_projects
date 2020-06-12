// Copyright [2018] Alibaba Cloud All rights reserved
#ifndef ENGINE_RACE_ENGINE_RACE_H_
#define ENGINE_RACE_ENGINE_RACE_H_
#include "include/engine.h"
#include <fcntl.h>
#include <map>
#include <pthread.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace polar_race {
using i8 = char;
using i32 = int;
using isize = ssize_t;
using u8 = unsigned char;
using u32 = unsigned;
using usize = size_t;

inline bool operator<(const PolarString &l, const PolarString &r) {
  return l.compare(r) < 0;
}

const u32 HASH_SIZE = 256;
// must define as usize, if use u32, (usize) ~PAGE_SIZE_M1 will be 0x0000000011111000
const usize PAGE_SIZE_M1 = 4095;

struct File {
  // by explicitly giving std::less<>, which accepts different comparison parameter type
  // plus the generic std::map::find in C++ 14
  // we can search in `map` by a PolarString, without constructing a std::string
  std::map<std::string, std::string, std::less<>> map;
  i32 fd;
  pthread_rwlock_t lock;
};

class EngineRace : public Engine {
public:
  static RetCode Open(const std::string &name, Engine **eptr) {
    std::string buf = "mkdir -p " + name;
    if (system(buf.c_str()) != 0) { return kIOError; }
    EngineRace *ret = new EngineRace;
    buf = name + "/00";
    u32 id_pos = buf.size() - 2;
    for (u32 i = 0; i < HASH_SIZE; ++i) {
      buf[id_pos] = (i >> 4) + 'a', buf[id_pos + 1] = (i & 15) + 'a';
      File &f = ret->fs[i];
      do {
        i32 fd = open(buf.c_str(), O_RDONLY, 0666);
        if (fd < 0) { break; }
        struct stat st;
        fstat(fd, &st);
        i8 *data = (i8 *)mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        for (i8 *cur = data, *end = data + st.st_size; cur < end;) {
          u32 key_sz = *(u32 *)cur;
          u32 val_sz = *(u32 *)(cur + 4 + key_sz);
          f.map.insert_or_assign(std::string(cur + 4, key_sz), std::string(cur + 8 + key_sz, val_sz));
          cur = (i8 *)((usize)(cur + 8 + key_sz + val_sz + PAGE_SIZE_M1) & ~PAGE_SIZE_M1);
        }
      } while (false);
      f.fd = open(buf.c_str(), O_RDWR | O_CREAT | O_APPEND | O_DIRECT, 0666);
      pthread_rwlock_init(&f.lock, nullptr);
    }
    *eptr = ret;
    return kSucc;
  }

  ~EngineRace() {
    for (File &f : fs) {
      close(f.fd);
      pthread_rwlock_destroy(&f.lock);
    }
  }

  RetCode Write(const PolarString &key, const PolarString &val) override {
    const u32 BUF_SZ = 1024 * 1024;
    thread_local u8 BUF[BUF_SZ + PAGE_SIZE_M1]; // I tried alignas(4096), it seems not working, so I have to manually align it
    if (key.empty()) { return kInvalidArgument; }
    File &f = fs[(u8)key[0]];
    u32 key_sz = key.size(), val_sz = val.size();
    u32 tot_sz = (8 + key_sz + val_sz + PAGE_SIZE_M1) & ~PAGE_SIZE_M1;
    if (tot_sz > BUF_SZ) { return kInvalidArgument; }
    u8 *buf = (u8 *)((usize)(BUF + PAGE_SIZE_M1) & ~PAGE_SIZE_M1);
    *(u32 *)buf = key_sz;
    memcpy(buf + 4, key.data(), key_sz);
    *(u32 *)(buf + 4 + key_sz) = val_sz;
    memcpy(buf + 8 + key_sz, val.data(), val_sz);
    if (write(f.fd, buf, tot_sz) != tot_sz) {
      // if IO cannot be performed in one `write` call, there will be synchronization problems
      return kIncomplete;
    }
    std::string owned_key(key.data(), key_sz), owned_val(val.data(), val_sz);
    pthread_rwlock_wrlock(&f.lock);
    f.map.insert_or_assign(std::move(owned_key), std::move(owned_val));
    pthread_rwlock_unlock(&f.lock);
    return kSucc;
  }

  RetCode Read(const PolarString &key, std::string *value) override {
    if (key.empty()) { return kInvalidArgument; }
    File &f = fs[(u8)key[0]];
    RetCode ret = kNotFound;
    pthread_rwlock_rdlock(&f.lock);
    auto it = f.map.find(key);
    if (it != f.map.end()) {
      ret = kSucc;
      *value = it->second;
    }
    pthread_rwlock_unlock(&f.lock);
    return ret;
  }

  RetCode Range(const PolarString &lo, const PolarString &hi, Visitor &vis) override {
    u32 lo_hash = lo.empty() ? 0 : lo[0];
    u32 hi_hash = hi.empty() ? HASH_SIZE - 1 : hi[0];
    for (u32 i = lo_hash; i <= hi_hash; ++i) {
      pthread_rwlock_rdlock(&fs[i].lock);
    }
    if (lo_hash == hi_hash) {
      auto &m = fs[lo_hash].map;
      auto it_pair = std::pair(m.lower_bound(lo), hi.empty() ? m.end() : m.lower_bound(hi));
      for (auto it = it_pair.first; it != it_pair.second; ++it) {
        vis.Visit(it->first, it->second);
      }
    } else {
      for (u32 i = lo_hash; i <= hi_hash; ++i) {
        auto &m = fs[i].map;
        auto it_pair = i == lo_hash ? std::pair(m.lower_bound(lo), m.end())
                                    : i == hi_hash ? std::pair(m.begin(), hi.empty() ? m.end() : m.lower_bound(hi))
                                                   : std::pair(m.begin(), m.end());
        for (auto it = it_pair.first; it != it_pair.second; ++it) {
          vis.Visit(it->first, it->second);
        }
      }
    }
    for (u32 i = lo_hash; i <= hi_hash; ++i) {
      pthread_rwlock_unlock(&fs[i].lock);
    }
    return kSucc;
  }

private:
  File fs[HASH_SIZE];
};
} // namespace polar_race

#endif // ENGINE_RACE_ENGINE_RACE_H_
