#include <fcntl.h>
#include <sys/stat.h>
// stdio.h defines getline, define it something else to avoid conflict
#define getline __getline_place_holder
#include <stdio.h>
#undef getline

int getline(int fd, unsigned char *buf, unsigned long size);

int main() {
  char buf[20];
  int fd, ret;

  fd = open("input1.txt", O_RDONLY), ret = getline(fd, buf, 20);
  printf("ret = %d, buf = \"%s\"\n", ret, buf);

  fd = open("input1.txt", O_RDONLY), ret = getline(fd, buf, 5);
  printf("ret = %d, buf = \"%s\"\n", ret, buf);

  fd = open("input2.txt", O_RDONLY), ret = getline(fd, buf, 20);
  printf("ret = %d, buf = \"%s\"\n", ret, buf);

  fd = open("input2.txt", O_RDONLY), ret = getline(fd, buf, 5);
  printf("ret = %d, buf = \"%s\"\n", ret, buf);

  buf[0] = '\0';
  fd = open("input3.txt", O_RDONLY), ret = getline(fd, buf, 20);
  printf("ret = %d, buf = \"%s\"\n", ret, buf);
}