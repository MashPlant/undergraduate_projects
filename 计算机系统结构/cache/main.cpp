#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <immintrin.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

enum RepAlgo {
  Lru, Tree, Rand, Lfu
};

bool read_bit(const u8 *p, u32 idx) {
  return p[idx >> 3] >> (idx & 7) & 1;
}

void write_bit(u8 *p, u32 idx, bool val) {
  if (val) {
    p[idx >> 3] |= 1 << (idx & 7);
  } else {
    p[idx >> 3] &= ~(1 << (idx & 7));
  }
}

// both read & write assume len < 16
u32 read_bits(const u8 *p, u32 start, u32 len) {
  p += (start >> 4) << 1, start &= 15;
  return _bextr_u32(*(u32 *) p, start, len);
}

void write_bits(u8 *p, u32 start, u32 len, u32 val) {
  p += (start >> 4) << 1, start &= 15;
  u32 *p1 = (u32 *) p;
  *p1 = *p1 ^ _bextr_u32(*p1, start, len) << start | val << start;
}

struct FreeDeleter {
  void operator()(void *p) { free(p); }
};

struct Cache {
  const static u32 ADDR_LEN = 64;
  const static u32 SIZE = 128 * 1024, LG_SIZE = __builtin_ctz(SIZE);

  RepAlgo algo;
  bool write_alloc;
  u8 lg_nway;
  u8 idx_shift, tag_shift;
  u8 linesz;
  u32 repsz;
  u32 nway;
  u16 idx_mask;
  u64 tag_mask;
  std::unique_ptr<u8[], FreeDeleter> lines;
  std::unique_ptr<u8[], FreeDeleter> rep;
  u32 nmiss = 0, nhit = 0;

  Cache(u32 nway, u8 blocksz, RepAlgo algo, bool write_back, bool write_alloc)
      : algo(algo), write_alloc(write_alloc), nway(nway) {
    lg_nway = __builtin_ctz(nway);
    u8 lg_blocksz = __builtin_ctz(blocksz);
    assert(nway == 1 << lg_nway && blocksz == 1 << lg_blocksz && lg_nway + lg_blocksz <= LG_SIZE);
    idx_shift = lg_blocksz;
    idx_mask = (1 << (LG_SIZE - lg_nway - lg_blocksz)) - 1;
    tag_shift = LG_SIZE - lg_nway;
    tag_mask = (1ULL << (ADDR_LEN - tag_shift)) - 1;
    linesz = ADDR_LEN - tag_shift /* tag */ + 1 /* valid */ + write_back /* dirty */;
    linesz = (linesz + 7) / 8;
    assert(linesz <= 8);
    lines.reset((u8 *) calloc((SIZE >> lg_blocksz) * linesz + 8, 1));
    repsz = 0; // default 0 for Rand
    if (algo == Lru || algo == Lfu) {
      repsz = nway * lg_nway;
    } else if (algo == Tree) {
      repsz = nway - 1;
    }
    repsz = (repsz + 7) / 8;
    rep.reset((u8 *) calloc((SIZE >> lg_blocksz >> lg_nway) * repsz + 8, 1));
  }

  bool access(u64 addr, bool write) {
    u16 idx = addr >> idx_shift & idx_mask;
    u64 tag = addr >> tag_shift & tag_mask;
    u32 line_base = idx * nway * linesz;
    u32 free_way = -1u;
    for (u32 i = 0; i < nway; ++i) {
      u64 line = *(u64 *) &lines[line_base + i * linesz];
      if (line & 1) {
        if ((line >> 1 & tag_mask) == tag) {
          u8 *p = &rep[idx * repsz];
          if (algo == Lru) {
            u32 cur = read_bits(p, i * lg_nway, lg_nway);
            for (u32 j = 0; j < nway; ++j) {
              u32 val = read_bits(p, j * lg_nway, lg_nway);
              u32 new_ = val < cur ? val + 1 : val == cur ? 0 : val;
              write_bits(p, j * lg_nway, lg_nway, new_);
            }
          } else if (algo == Tree) {
            access_tree(p, i);
          } else if (algo == Lfu) {
            u32 new_ = read_bits(p, i * lg_nway, lg_nway) + 1;
            if (new_ == nway) {
              for (u32 j = 0; j < nway; ++j) {
                u32 val = read_bits(p, j * lg_nway, lg_nway);
                write_bits(p, j * lg_nway, lg_nway, val + (i == j) >> 1);
              }
            } else {
              write_bits(p, i * lg_nway, lg_nway, new_);
            }
          }
          return ++nhit, true;
        }
      } else if (free_way == -1u) {
        free_way = i;
      }
    }
    if (!write || write_alloc) {
      u8 *p = &rep[idx * repsz];
      if (algo == Lru) {
        for (u32 i = 0; i < nway; ++i) {
          u32 val = read_bits(p, i * lg_nway, lg_nway);
          u32 new_ = val + 1;
          if (val == nway - 1 || i == free_way) {
            new_ = 0, fill(line_base, i, tag);
          }
          write_bits(p, i * lg_nway, lg_nway, new_);
        }
      } else if (algo == Tree) {
        if (free_way == -1u) {
          free_way = 0;
          for (u32 i = lg_nway - 1; ~i >> 31; --i) {
            free_way = (free_way << 1) + 1 + read_bit(p, free_way);
          }
          free_way -= nway - 1;
        }
        access_tree(p, free_way);
        fill(line_base, free_way, tag);
      } else if (algo == Rand) {
        fill(line_base, free_way == -1u ? rand() & nway - 1 : free_way, tag);
      } else if (algo == Lfu) {
        u32 min = read_bits(p, 0, lg_nway), argmin = 0;
        for (u32 i = 1; i < nway; ++i) {
          u32 val = read_bits(p, i * lg_nway, lg_nway);
          if (val < min) {
            min = val, argmin = i;
          }
        }
        write_bits(p, argmin * lg_nway, lg_nway, 1);
        fill(line_base, argmin, tag);
      }
    }
    return ++nmiss, false;
  }

  void fill(u32 line_base, u32 way, u64 tag) {
    u64 *pline = (u64 *) &lines[line_base + way * linesz];
    *pline = 1 | tag << 1 | *pline ^ _bextr_u64(*pline, 0, linesz * 8);
  }

  void access_tree(u8 *p, u32 way) {
    u32 idx = 0;
    for (u32 j = lg_nway - 1; ~j >> 31; --j) {
      bool right = way >> j & 1;
      write_bit(p, idx, !right);
      idx = (idx << 1) + 1 + right;
    }
  }
};

struct Access {
  u64 addr;
  bool write;
};

std::vector<Access> parse(const char *f) {
  std::vector<Access> ret;
  int fd = open(f, O_RDONLY);
  struct stat st;
  fstat(fd, &st);
  char *p = (char *) mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(p != MAP_FAILED);
  char *end = p + st.st_size;
  auto read_u64 = [](char *&p) { return strtoull(p, &p, 16); };
  while (p < end) {
    switch (*p) {
      case 'r':
      case 'l':
        ret.push_back(Access{read_u64(++p), false});
        break;
      case 'w':
      case 's':
        ret.push_back(Access{read_u64(++p), true});
        break;
      case '0':
        ret.push_back(Access{read_u64(p), false});
        break;
      default:
        __builtin_unreachable();
    }
    while (*p == '\n')
      ++p;
  }
  munmap(p, st.st_size);
  close(fd);
  return ret;
}

int main() {
#define RUN()                  \
  for (auto x : f) {           \
    c.access(x.addr, x.write); \
  }                            \
  printf("%.2f ", (float)c.nmiss / (c.nmiss + c.nhit) * 100);

  const std::vector<Access> FILES[] = {
    parse("trace/astar.trace"),
    parse("trace/bzip2.trace"),
    parse("trace/gcc.trace"),
    parse("trace/perlbench.trace"),
    parse("trace/swim.trace"),
    parse("trace/bodytrack_1m.trace"),
    parse("trace/canneal.uniq.trace"),
    parse("trace/mcf.trace"),
    parse("trace/streamcluster.trace"),
    parse("trace/twolf.trace"),
  };
  srand(19260817);
  puts("statistics for different cache layouts:");
  for (auto &f : FILES) {
    for (u32 nway : {1, 4, 8, 0}) { // 0 for full-associative
      for (u32 blocksz : {8, 32, 64}) {
        Cache c(nway == 0 ? Cache::SIZE / blocksz : nway, blocksz, Lru, true, true);
        RUN();
      }
    }
    puts("");
  }
  puts("statistics for different replace algorithms:");
  for (auto &f : FILES) {
    for (auto a : {Lru, Tree, Rand, Lfu}) {
      Cache c(8, 8, a, true, true);
      RUN();
    }
    puts("");
  }
  puts("statistics for different write strategies:");
  for (auto &f : FILES) {
    for (auto write_alloc : {true, false}) {
      Cache c(8, 8, Lru, true, write_alloc);
      RUN();
    }
    puts("");
  }
  puts("write logs:");
  const char *LOGS[] = {"astar.log", "bzip2.log", "gcc.log", "perlbench.log", "swim.log", "bodytrack_1m.log", "canneal.log", "mcf.log", "streamcluster.log", "twolf.log"};
  for (u32 i = 0; i < 10; ++i) {
    Cache c(8, 8, Lru, true, true);
    FILE *f = fopen(LOGS[i], "w");
    for (auto x : FILES[i]) {
      fputs(c.access(x.addr, x.write) ? "Hit\n" : "Miss\n", f);
    }
    fclose(f);
  }
}