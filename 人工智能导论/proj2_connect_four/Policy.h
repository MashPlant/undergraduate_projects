#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <utility>
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using i32 = int32_t;
using uptr = uintptr_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;

struct XorShiftRNG {
  u32 seed;

  u32 gen() {
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
  }
};

const u8 Invalid = -3, Unfinished = -2, Draw = -1, WWin = 0, BWin = 1;

inline u32 ctz_wrapper(u32 x) {
#ifdef _MSC_VER
  u32 ret;
  _BitScanForward((unsigned long *)&ret, x);
  return ret;
#else
  return __builtin_ctz(x);
#endif
}

inline u32 bextr_wrapper(u32 x, u32 start, u32 len) {
#ifdef __BMI__
  return _bextr_u32(x, start, len);
#else
  return (x >> start) & ((1 << len) - 1);
#endif
}

template <u32 R, u32 C>
struct Board {
  u16 w[C], b[C];
  u8 state = Unfinished;
  u8 last_p = 0, last_row, last_col;

  Board(const i32 *board, u8 last_row, u8 last_col) : last_p(1), last_row(last_row), last_col(last_col) {
    for (u32 i = 0; i < C; ++i) {
      w[i] = b[i] = ~((1 << R) - 1);
    }
    for (u32 i = 0; i < R; ++i) {
      for (u32 j = 0; j < C; ++j) {
        if (board[i * C + j] == 1) { // counterpart
          b[j] |= 1 << i;
        } else if (board[i * C + j] == 2) { // me
          w[j] |= 1 << i;
        }
      }
    }
  }

  Board() {
    for (u32 i = 0; i < C; ++i) {
      w[i] = b[i] = ~((1 << R) - 1);
    }
  }

  void print(FILE *f, u32 dis_r, u32 dis_c) {
    for (u32 j = 0; j < R; ++j) {
      fprintf(f, "%d ", R - 1 - j);
      for (u32 i = 0; i < C; ++i) {
        if (j == dis_r && i == dis_c) {
          assert(!(w[i] & (1 << j)));
          assert(!(b[i] & (1 << j)));
          fprintf(f, "X ");
        } else if (w[i] & (1 << j)) {
          assert(!(b[i] & (1 << j)));
          fprintf(f, "W ");
        } else if (b[i] & (1 << j)) {
          fprintf(f, "B ");
        } else {
          fprintf(f, "- ");
        }
      }
      fputs("\n", f);
    }
    fprintf(f, "  ");
    for (u32 i = 0; i < C; ++i) {
      fprintf(f, "%d ", i);
    }
    fputs("\n", f);
  }

  // return = top
  // return - 1 = the next row to put chess(return 0 for invalid)
  u32 get_row(u32 i, u32 dis_r, u32 dis_c) const {
    u32 row = ctz_wrapper(b[i] | w[i]);
    if (row == 0) {
      return 0;
    }
    return row - (i == dis_c && row - 1 == dis_r);
  }

  void move(u32 i, u32 dis_r, u32 dis_c) {
    // assert(state == Unfinished);
    if (u32 row = get_row(i, dis_r, dis_c)) {
      last_p ^= 1, last_col = i, last_row = row - 1;
      (last_p ? b[i] : w[i]) |= 1 << last_row;
      if (check_win()) {
        state = last_p;
      } else {
        for (u32 i = 0; i < C; ++i) {
          if (get_row(i, dis_r, dis_c)) {
            return;
          }
        }
        state = Draw;
      }
    } else {
      state = Invalid;
    }
  }

  // return true for last_p wins
  bool check_win() const {
    u32 i = last_col, row = last_row;
    u32 sel = std::min(7u, R + 3 - row);
    const u16 *which = last_p ? b + i : w + i;
    u16 rd0 = which[-3], rd1 = which[-2], rd2 = which[-1], rd3 = which[0], rd4 = which[1], rd5 = which[2], rd6 = which[3];
    u64 block = (u64)bextr_wrapper((i - 3 < C ? rd0 : 0) << 3, row, sel) | (u64)bextr_wrapper((i - 2 < C ? rd1 : 0) << 3, row, sel) << 8 |
                (u64)bextr_wrapper((i - 1 < C ? rd2 : 0) << 3, row, sel) << 16 | (u64)bextr_wrapper(rd3 << 3, row, sel) << 24 |
                (u64)bextr_wrapper((i + 1 < C ? rd4 : 0) << 3, row, sel) << 32 | (u64)bextr_wrapper((i + 2 < C ? rd5 : 0) << 3, row, sel) << 40 |
                (u64)bextr_wrapper((i + 3 < C ? rd6 : 0) << 3, row, sel) << 48;
    return !((~block & 0x78000000ULL)                                                                                                    // vertical
             && (~block & 0x08080808ULL)                                                                                                 // horizontal
             && (~block & 0x0808080800ULL) && (~block & 0x080808080000ULL) && (~block & 0x08080808000000ULL) && (~block & 0x08040201ULL) // right slash
             && (~block & 0x1008040200ULL) && (~block & 0x201008040000ULL) && (~block & 0x40201008000000ULL) && (~block & 0x08102040ULL) // left slash
             && (~block & 0x0408102000ULL) && (~block & 0x020408100000ULL) && (~block & 0x01020408000000ULL));
  }

  u8 move_to_end(u32 seed, u32 dis_r, u32 dis_c) {
    XorShiftRNG rng{seed};
    u8 ok[C], len = 0;
    for (u32 i = 0; i < C; ++i) {
      if (u32 row = get_row(i, dis_r, dis_c)) {
        ok[len++] = (u8)(i << 4 | row);
      }
    }
    for (; len; last_p ^= 1) {
      if (check_win()) {
        return last_p;
      }
      u8 sel = rng.gen() % len;
      u8 i = ok[sel] >> 4, row = (ok[sel] & 0xF) - 1;
      (last_p ? w[i] : b[i]) |= 1 << row;
      last_col = i, last_row = row;
      if (row == 0 || (row -= (i == dis_c && row - 1 == dis_r)) == 0) {
        for (u32 i = sel; i + 1 < len; ++i) {
          ok[i] = ok[i + 1];
        }
        --len;
      } else {
        ok[sel] = (u8)(i << 4 | row);
      }
    }
    return Draw;
  }
};

template <u32 R, u32 C>
struct Node {
  Board<R, C> b;
  Node *p, *ch[C]; // nullptr -> unexplored; 0x1 -> invalid move;
  i32 vis, dirty : 1, win : 31;
  f32 uct;

  u32 uct_select() {
    u32 ret; // this->b is unfinished, so ret must be assigned in the loop(or just return i)
    f32 ans = 0.0;
    if (dirty) {
      dirty = 0;
      f32 ln_n = logf(f32(vis));
      for (u32 i = 0; i < C; ++i) {
        if (ch[i] == nullptr) {
          return i;
        } else if ((uptr)ch[i] != 1) {
          ch[i]->uct = f32(ch[i]->win) / ch[i]->vis + sqrtf(2.0f * ln_n / ch[i]->vis);
          if (ch[i]->uct > ans) {
            ans = ch[i]->uct;
            ret = i;
          }
        }
      }
    } else {
      for (u32 i = 0; i < C; ++i) {
        if (ch[i] == nullptr) {
          return i;
        } else if ((uptr)ch[i] != 1) {
          // f32 tmp = eval(*(ch[i]));
          if (ch[i]->uct > ans) {
            ans = ch[i]->uct;
            ret = i;
          }
        }
      }
    }
    return ret;
  }
};

template <u32 R, u32 C>
struct Policy {
  u32 dis_r, dis_c;
  Node<R, C> *top, pool[1];

  Node<R, C> *alloc(Node<R, C> *p, u32 i) {
    // assert(p->b.state == Unfinished)
    Board<R, C> b = p->b;
    b.move(i, dis_r, dis_c);
    if (b.state == Invalid) {
      p->ch[i] = (Node<R, C> *)1;
      return p;
    } else {
      Node<R, C> *n = p->ch[i] = top++;
      n->b = b;
      n->p = p;
      memset(n->ch, 0, sizeof(n->ch) + 12); // ch = {nullptr}
      // n->vis = 0, n->win = 0, n->uct = 0.0f;
      return n;
    }
  }

  std::pair<u32, u32> mcts(const Board<R, C> &b, u32 iter, f32 c = 1) {
    using namespace std::chrono;
    const u32 STEP = 20000;
    auto beg = high_resolution_clock::now();
    Node<R, C> root{b};
    top = pool;
    u32 j = 1;
    for (; (top - pool) + STEP < iter; j += STEP) {
      if (duration<f32>(high_resolution_clock::now() - beg).count() > 2.7) {
        break;
      }
      for (u32 i = j; i < j + STEP; ++i) {
        Node<R, C> *x = &root;
        u32 id;
        while (true) {
          id = x->uct_select();
          if (x->ch[id] == nullptr) {
            break;
          }
          x = x->ch[id];
        }
        u8 win;
        if (x->b.state == Unfinished) {
          x = alloc(x, id);
          win = Board<R, C>(x->b).move_to_end(i * 19260817, dis_r, dis_c);
        } else {
          win = x->b.state;
        }
        while (x) {
          ++x->vis, x->win += win == x->b.last_p, x->dirty = 1;
          x = x->p;
        }
      }
    }
    u32 ret;
    i32 ans = 0;
    for (u32 i = 0; i < C; ++i) {
      if ((uptr)root.ch[i] != 1) {
        if (root.ch[i]->vis > ans) {
          ans = root.ch[i]->vis;
          ret = i;
        }
      }
    }
    return {b.get_row(ret, dis_r, dis_c), ret};
  }
};