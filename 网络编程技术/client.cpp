#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
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
using u64 = uint64_t;

#define ASSERT(op) \
  if (!(op)) {     \
    perror(#op);   \
    exit(1);       \
  }

int main(int argc, const char **argv) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in addr{AF_INET, htons(8000), {inet_addr("127.0.0.1")}, {}};
  ASSERT(connect(sockfd, (sockaddr *)&addr, sizeof addr) == 0);

  char name[100], passwd[100];
  scanf("%s%s", name, passwd);
  char buf[8];
  u32 namelen = strlen(name), passwdlen = strlen(passwd);
  (u16 &)buf[0] = htons(namelen);
  (u16 &)buf[2] = htons(passwdlen);
  ASSERT(write(sockfd, buf, 4) == 4);
  ASSERT(write(sockfd, name, namelen) == namelen);
  ASSERT(write(sockfd, passwd, passwdlen) == passwdlen);
  char *recvbuf = (char *)malloc(4 * 1024 * 1024);
  auto do_recv = [&] {
    ASSERT(recv(sockfd, buf, 8, MSG_WAITALL) == 8);
    u32 arg1 = ntohl((u32 &)buf[0]), arg2 = ntohl((u32 &)buf[4]);
    bool ok = arg1 & 0xFFFF;
    u64 recvlen = (u64)(arg1 >> 16) << 32 | arg2;
    printf("recvlen = %ld\n", recvlen);
    ASSERT(recv(sockfd, recvbuf, recvlen, MSG_WAITALL) == recvlen);
    recvbuf[recvlen] = '\0';
    puts(recvbuf);
  };
  while (printf(">> "), fflush(stdout), scanf(" %c", passwd) == 1) {
    switch (passwd[0]) {
    case 'g': {
      scanf("%s", name), namelen = strlen(name);
      (u32 &)buf[0] = htonl(namelen << 2 | 0);
      (u32 &)buf[4] = htonl(0);
      ASSERT(write(sockfd, buf, 8) == 8);
      ASSERT(write(sockfd, name, namelen) == namelen);
      do_recv();
    } break;
    case 'p': {
      scanf("%s", name), namelen = strlen(name);
      int fd = open(name, O_RDONLY);
      ASSERT(fd > 0);
      struct stat st;
      fstat(fd, &st);
      printf("st.st_size = %ld\n", st.st_size);
      // printf("%u %u\n", (u32)(st.st_size >> 32), (u32)(st.st_size >> 32 | namelen << 2 | 1));
      (u32 &)buf[0] = htonl((st.st_size >> 32) << 16 | namelen << 2 | 1);
      (u32 &)buf[4] = htonl(st.st_size);
      ASSERT(write(sockfd, buf, 8) == 8);
      ASSERT(write(sockfd, name, namelen) == namelen);
      ASSERT(sendfile64(sockfd, fd, nullptr, st.st_size) == st.st_size);
      do_recv();
    } break;
    case 'l': {
      (u32 &)buf[0] = htonl(0 << 2 | 2);
      (u32 &)buf[4] = htonl(0);
      ASSERT(write(sockfd, buf, 8) == 8);
      do_recv();
    } break;
    case 'q': {
      (u32 &)buf[0] = htonl(0 << 2 | 3);
      (u32 &)buf[4] = htonl(0);
      ASSERT(write(sockfd, buf, 8) == 8);
      return 0;
    }
    default:
      printf("no such command: %c\n", passwd[0]);
      break;
    }
  }
}