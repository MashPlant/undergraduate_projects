#include <cstdio>
#include <cstring>

using u8 = unsigned char;
using u32 = unsigned;

struct DpItem {
  u8 ok : 1;
  u8 p : 1;
  u8 covered : 1;
} dp[3 * 3 * 3 * 3 * 3 * 3 * 3 * 3];

u8 dfs1(u32 idx) {
  if (dp[idx].ok) {
    return dp[idx].p;
  }
  for (u32 i = 0, fac = 1; i < 8; ++i, fac *= 3) {
    if (idx / fac % 3 == 2 && dfs1(idx - fac) && dfs1(idx - fac * 2)) {
      dp[idx].p = 1;
    }
  }
  dp[idx].ok = 1;
  return dp[idx].p;
}

void dfs2(u32 idx, u8 cover = 0) {
  if (!dp[idx].p || dp[idx].covered) {
    return;
  }
  dp[idx].covered = cover;
  for (u32 i = 0, fac = 1; i < 8; ++i, fac *= 3) {
    if (idx / fac % 3 == 2) {
      dfs2(idx - fac, 1);
      dfs2(idx - fac * 2, 1);
    }
  }
}

// 8 bit in tbl indicate is is A or \bar{A}
u32 simplify(const u8 *tbl, u32 len, u8 (*out)[8]) {
  memset(dp, 0, sizeof dp);
  for (u32 i = 0; i < len; ++i) {
    u32 idx = 0;
    for (u32 j = 0, fac = 1; j < 8; ++j, fac *= 3) {
      idx += fac * (tbl[i] >> j & 1);
    }
    dp[idx].ok = dp[idx].p = 1;
  }
  for (u32 i = 0; i < sizeof(dp); ++i) {
    dfs1(i);
  }
  for (u32 i = 0; i < sizeof(dp); ++i) {
    dfs2(i);
  }
  u32 cnt = 0;
  for (u32 i = 0; i < sizeof(dp); ++i) {
    if (dp[i].p && !dp[i].covered) {
      for (u32 j = 0, copy = i; j < 8; ++j, copy /= 3) {
        // putchar("01_"[copy % 3]);
        out[cnt][j] = (u8) (copy % 3);
      }
      // puts("");
      ++cnt;
    }
  }
  return cnt;
}