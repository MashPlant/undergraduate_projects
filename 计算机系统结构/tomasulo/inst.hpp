#pragma once

#include <vector>
#include <string_view>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "op.hpp"

struct Inst {
  Op op;
  union {
    struct {
      u32 dst;
      union {
        struct { u32 l, r; }; // Add, Sub, Mul, Div
        struct { u32 imm; }; // Ld
      };
    };
    struct { u32 cond, cmp, offset; }; // Jump
  };
  // 0 for not set yet; write back time always = exec comp time + 1, so it not recorded
  u32 issue_time, comp_time;

  // `end` will be set to pointing the char past the last char belonging to this inst, normally a '\n'
  Inst(char *s, char **end) : issue_time(0), comp_time(0) {
    char *p = s;
    do ++p; while (isalpha(*p));
    std::string_view o(s, p - s); // now *p == ','
    // "JUMP" is not checked, assume `o` is within the keyword's range
    op = o == "ADD" ? Add : o == "MUL" ? Mul : o == "SUB" ? Sub : o == "DIV" ? Div : o == "LD" ? Ld : Jump;
    switch (op) {
      // this is a gcc extension, "case ranges", I love it
      case Add ... Div:
        dst = strtol(p + 2, &p, 10);
        l = strtol(p + 2, &p, 10);
        r = strtol(p + 2, &p, 10);
        break;
      case Ld:
        dst = strtol(p + 2, &p, 10);
        imm = strtol(p + 3, &p, 16);
        break;
      case Jump:
        cmp = strtol(p + 3, &p, 16);
        cond = strtol(p + 2, &p, 10);
        offset = strtol(p + 3, &p, 16);
        break;
      DEFAULT
    }
    *end = p;
  }

  static std::vector<Inst> parse_file(const char *name) {
    int fd = open(name, O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    char *p = (char *) mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(p != MAP_FAILED);
    char *end = p + st.st_size;
    std::vector<Inst> ret;
    while (p < end) {
      ret.emplace_back(p, &p), ++p;
    }
    munmap(end - st.st_size, st.st_size);
    return ret;
  }

  void display(FILE *f = stdout) {
    const char *name = op_name(op);
    switch (op) {
      case Add ... Div:
        fprintf(f, "%s,R%d,R%d,R%d", name, dst, l, r);
        break;
      case Ld:
        fprintf(f, "%s,R%d,0x%X", name, dst, imm);
        break;
      case Jump:
        fprintf(f, "%s,0x%X,R%d,0x%X", name, cmp, dst, offset);
        break;
      DEFAULT
    }
  }
};