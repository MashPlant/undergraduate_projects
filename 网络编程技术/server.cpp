#include "defer.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <linux/tcp.h>
#include <memory>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using u16 = uint16_t;
using u32 = uint32_t;
using iptr = intptr_t;

#define TRY(op, require) \
  ({ auto __x = op; if (!(__x require)) return perror(#op), false; __x; })

// ty = 0 for normal, else for error
#define MSG(x, ty, op)                                           \
  do {                                                           \
    char __buf[8];                                               \
    (u32 &)__buf[0] = ty; /* no need for `htonl` for zeroness*/  \
    (u32 &)__buf[4] = htonl(sizeof(x));                          \
    if (write(connfd, __buf, 8) != 8) perror("MSG");             \
    if (write(connfd, x, sizeof(x)) != sizeof(x)) perror("MSG"); \
    op;                                                          \
  } while (false)
#define ERRMSG(x) MSG(x, 1, return )

#define PASSWD_SUFFIX ".passwd"
#define FILE_SUFFIX ".file"

// not using `new` to avoid exceptions; a nullptr result will be handled properly in subsequent operations
struct FreeDeleter {
  void operator()(void *p) const { free(p); }
};
using Buffer = std::unique_ptr<char[], FreeDeleter>;
Buffer alloc(u32 n) { return Buffer((char *)malloc(n)); }

enum Op {
  Get = 0,
  Put,
  List,
  Quit,
};

const static int ON = 1;

// assume success
bool verify_passwd(int fd, char *passwd, u32 len) {
  struct stat st;
  fstat(fd, &st);
  if (st.st_size != len) { return false; }
  void *data = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  bool result = memcmp(passwd, data, len) == 0;
  munmap(data, st.st_size);
  return result;
}

Buffer get_path(int connfd, u32 len) {
  Buffer buf = alloc(len + 1);
  if (recv(connfd, buf.get(), len, MSG_WAITALL) != len) {
    return nullptr;
  }
  buf[len] = 0;                // it is safe to write, `buf` cannot be nullptr here since `recv` successfully returned
  if (strstr(&buf[0], "..")) { // disallow access to parent dir
    buf.reset(nullptr);
  }
  return buf;
}

void serve(int connfd, int dirfd) {
  // must use the same `dir`, instead of calling `fdopendir` every time
  // because the file offset in `dirfd` is not reset in subsequent `fdopendir` calls
  DIR *dir = fdopendir(dirfd);
  if (!dir) { ERRMSG("failed to open user dir"); }
  DEFER(closedir(dir));
  char buf[sizeof(u32) * 2];
  while (true) {
    if (recv(connfd, buf, sizeof(buf), MSG_WAITALL) != sizeof(buf)) {
      ERRMSG("expect <arg1(u29), op code(u2), arg2(u32)>");
    }
    u32 arg1 = ntohl((u32 &)buf[0]), arg2 = ntohl((u32 &)buf[4]);
    switch (arg1 & 3) {
    case Get: {
      Buffer p = get_path(connfd, arg1 >> 2 & 0xFFFF);
      int fd = openat(dirfd, p.get(), O_RDONLY);
      // file-not-found can be forgiven, so no need to close the connection
      if (fd > 0) {
        DEFER(close(fd));
        if (![=] {
              struct stat st;
              TRY(fstat(fd, &st), == 0);
              (u32 &)buf[0] = 0;
              (u32 &)buf[4] = htonl(st.st_size);
              TRY(write(connfd, buf, sizeof(buf)), == sizeof(buf));
              TRY(sendfile(connfd, fd, nullptr, st.st_size), == st.st_size);
              return true;
            }()) { ERRMSG("failed to get"); }
      } else {
        MSG("no such file", 1, /* nop */);
      }
    } break;
    case Put:
      if (![=] {
            Buffer p = get_path(connfd, arg1 >> 2 & 0xFFFF);
            int fd = TRY(openat(dirfd, p.get(), O_RDWR | O_CREAT | O_TRUNC, 0666), > 0);
            DEFER(close(fd));
            TRY(ftruncate(fd, arg2), == 0);
            char *dst = (char *)TRY(mmap(nullptr, arg2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0), != MAP_FAILED);
            DEFER(munmap(dst, arg2));
            TRY(recv(connfd, dst, arg2, MSG_WAITALL), == arg2);
            return true;
          }()) {
        ERRMSG("failed to put");
      } else {
        MSG("put success", 0, /* nop */);
      }
      break;
    case List:
      if (![=] {
            rewinddir(dir);
            std::string ls;
            while (dirent *ent = readdir(dir)) {
              if (ent->d_type == DT_REG) {
                ls += ent->d_name, ls += '\n';
              }
            }
            (u32 &)buf[0] = 0;
            (u32 &)buf[4] = htonl(ls.size());
            TRY(write(connfd, buf, sizeof(buf)), == sizeof(buf));
            TRY((size_t)write(connfd, ls.c_str(), ls.size()), == ls.size());
            return true;
          }()) { ERRMSG("failed to list"); }
      break;
    case Quit:
      return;
    default:
      __builtin_unreachable();
    }
  }
}

void work(void *arg) {
  int connfd = (int)(iptr)arg;
  DEFER(close(connfd));
  char buf[sizeof(u32)];
  if (recv(connfd, buf, sizeof(u32), MSG_WAITALL) != sizeof(u32)) {
    perror("recv");
    ERRMSG("expect <name len(u16), passwd len(u16)>");
  }
  u32 name_len = ntohs((u16 &)buf[0]), passwd_len = ntohs((u16 &)buf[2]);
  Buffer name_passwd = alloc(name_len + sizeof(PASSWD_SUFFIX) + passwd_len);
  char *name = name_passwd.get(), *passwd = name_passwd.get() + name_len + sizeof(PASSWD_SUFFIX);
  if (recv(connfd, name, name_len, MSG_WAITALL) != name_len ||
      recv(connfd, passwd, passwd_len, MSG_WAITALL) != passwd_len) {
    ERRMSG("inconsistent name len/passwd len");
  }
  if (!std::all_of(name, name + name_len, isalnum)) {
    ERRMSG("invalid name");
  }
  static pthread_mutex_t FILE_LOCK = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&FILE_LOCK);
  DEFER(pthread_mutex_unlock(&FILE_LOCK));
  memcpy(name + name_len, PASSWD_SUFFIX, sizeof(PASSWD_SUFFIX));
  int fd = open(name, O_RDONLY, 0666);
  if (fd > 0) { // login
    if (verify_passwd(fd, passwd, passwd_len)) {
      memcpy(name + name_len, FILE_SUFFIX, sizeof(FILE_SUFFIX));
      serve(connfd, open(name, O_EXCL));
    } else {
      ERRMSG("incorrect passwd");
    }
  } else { // register
    if ([=] {
          int fd = TRY(open(name, O_WRONLY | O_CREAT, 0666), > 0);
          TRY(write(fd, passwd, passwd_len), == passwd_len);
          TRY(close(fd), == 0);
          memcpy(name + name_len, FILE_SUFFIX, sizeof(FILE_SUFFIX));
          TRY(mkdir(name, 0777), == 0);
          return true;
        }()) {
      serve(connfd, open(name, O_EXCL));
    } else {
      ERRMSG("failed to create user");
    }
  }
}

bool configure_listen(int listenfd) {
  TRY(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &ON, sizeof(ON)), == 0);
  struct sigaction act {};
  act.sa_handler = SIG_IGN; // other fields are set default to 0
  TRY(sigaction(SIGPIPE, &act, nullptr), == 0);
  signal(SIGPIPE, [](int) {});
  return true;
}

bool configure_connect(int connfd) {
  setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, &ON, sizeof(ON));
  linger lg{.l_onoff = 1, .l_linger = 0};
  TRY(setsockopt(connfd, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg)), == 0);
  return true;
}

int main(int argc, const char **argv) {
  return ![=] {
    int listenfd = TRY(socket(AF_INET, SOCK_STREAM, 0), > 0); // it doesn't need to be closed
    sockaddr_in addr{AF_INET, htons(8000), {}, {}};           // by default, sin_addr = {} => any, sin_zero = {}
    if (argc == 2) {
      addr.sin_addr.s_addr = TRY(inet_addr(argv[1]), != INADDR_NONE);
    }
    if (!configure_listen(listenfd)) { perror("configure_listen"); }
    TRY(bind(listenfd, (sockaddr *)&addr, sizeof(addr)), == 0);
    TRY(listen(listenfd, 10), == 0);
    while (true) {
      int connfd = accept(listenfd, nullptr, nullptr);
      if (connfd < 0) {
        perror("accept");
      } else {
        pthread_t pid;
        if (!configure_connect(connfd)) { perror("configure_connect"); }
        if (pthread_create(&pid, nullptr, (void *(*)(void *))work, (void *)(iptr)connfd) != 0 ||
            pthread_detach(pid) != 0) { perror("pthread"); }
      }
    }
    // unreachable
  }();
}