#pragma once

#include "common.hpp"

enum Op : u8 {
  Add, Sub, Mul, Div, Ld, Jump
};

inline const char *op_name(Op op) {
  constexpr const char *NAMES[] = {"ADD", "SUB", "MUL", "DIV", "LD", "JUMP"};
  return NAMES[op];
}

// return true if (i32) l / (i32) r is undefined behavior in C/C++
// in this case, the simulator returns l for l / r, in one time cycle
inline bool div_special(u32 l, u32 r) {
  // actually it is nothing special, INT_MIN / -1 will cause signed integer overflow, which is UB
  // in practical cases, it causes floating point exception on x86_64 linux
  return r == 0 || (l == 0x80000000u && r == -1u);
}

// it depends on `l` and `r`, so can't determine it when issuing
inline u8 op_time(Op op, u32 l, u32 r) {
  constexpr u8 TIME[] = {3, 3, 4, 4, 3, 1};
  return op == Div && div_special(l, r) ? 1 : TIME[op];
}

// I know all registers are signed integers, but I am using u32 in all other places, just keep consistent
// caller should guarantee `op` is within {+, -, *, /}
inline u32 op_eval(Op op, u32 l, u32 r) {
  // the result of +-* on u32 is the same as on i32
  switch (op) {
    case Add:
      return l + r;
    case Sub:
      return l - r;
    case Mul:
      return l * r;
    case Div:
      return div_special(l, r) ? l : (i32) l / (i32) r;
    DEFAULT
  }
}