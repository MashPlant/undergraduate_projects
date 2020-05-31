//
// Created by mashplant on 2020/3/3.
//

#ifndef DPLL_BITSET_HPP
#define DPLL_BITSET_HPP

#include <cstring>

using u32 = unsigned;

// some helper functions manipulating a **non-owning** bitset
namespace bs {
  inline u32 word_len(u32 nbits) { return (nbits + 31) >> 5; }

  inline bool get(const u32 *p, u32 idx) { return p[idx >> 5] >> (idx & 31) & 1; }

  inline void set(u32 *p, u32 idx) { p[idx >> 5] |= 1 << (idx & 31); }

  inline void del(u32 *p, u32 idx) { p[idx >> 5] &= ~(1 << (idx & 31)); }

  inline void clear(u32 *p, u32 len) { memset(p, 0, len * sizeof(u32)); }

  inline void disjunction(u32 *l, const u32 *r, u32 len) {
    for (u32 i = 0; i < len; ++i) { l[i] |= r[i]; }
  }

  template<typename OS>
  inline void show(OS &&os, u32 *p, u32 nbits) {
    bool first = true;
    os << "bitset [";
    for (u32 i = 0; i < nbits; ++i) {
      if (get(p, i)) {
        if (!first) { os << ", "; }
        first = false;
        os << i;
      }
    }
    os << "]\n";
  }

  inline u32 count(const u32 *p, u32 len) {
    u32 sum = 0;
    for (u32 i = 0; i < len; ++i) {
      sum += __builtin_popcount(p[i]);
    }
    return sum;
  }
}

#endif //DPLL_BITSET_HPP
