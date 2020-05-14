#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "temperature.h"

using u16 = unsigned short;
int n, m;
int bit[1201][1201], t[1201][1201];

void update(int x, int y, int temp) {
  u16 hi[11]{}, hi_cnt = 0;
  for (int i = y; i <= m; i += i & -i, ++hi_cnt)
    hi[hi_cnt] = i;
  for (int i = x; i <= n; i += i & -i) {
    int *p = &bit[i][0];
    switch (hi_cnt) {
      case 11:
        p[hi[10]] += temp;
      case 10:
        p[hi[9]] += temp;
      case 9:
        p[hi[8]] += temp;
      case 8:
        p[hi[7]] += temp;
      case 7:
        p[hi[6]] += temp;
      case 6:
        p[hi[5]] += temp;
      case 5:
        p[hi[4]] += temp;
      case 4:
        p[hi[3]] += temp;
      case 3:
        p[hi[2]] += temp;
      case 2:
        p[hi[1]] += temp;
      case 1:
        p[hi[0]] += temp;
    }
  }
}

void init(int n, int m, int **temp) {
  ::n = n, ::m = m;
  for (int i = 1; i <= n; ++i) {
    memcpy(t[i] + 1, temp[i] + 1, m * sizeof(int));
    memcpy(bit[i] + 1, temp[i] + 1, m * sizeof(int));
  }
  for (int i = 1; i <= n; ++i)
    for (int j = 1; j <= m; ++j)
      if (j + (j & -j) <= m)
        bit[i][j + (j & -j)] += bit[i][j];
  for (int i = 1; i <= n; ++i)
    if (i + (i & -i) <= n)
      for (int j = 1; j <= m; ++j)
        bit[i + (i & -i)][j] += bit[i][j];
}

int query(int x1, int y1, int x2, int y2) {
  int ret = 0;
  u16 lo[11]{}, lo_cnt = 0, hi[11]{}, hi_cnt = 0;
  for (int i = y2; i; i &= (i - 1), ++hi_cnt)
    hi[hi_cnt] = i;
  for (int i = y1 - 1; i; i &= (i - 1), ++lo_cnt)
    lo[lo_cnt] = i;
  for (int i = x2; i; i &= (i - 1)) {
    int *p = &bit[i][0];
    switch (hi_cnt) {
      case 11:
        ret += p[hi[10]];
      case 10:
        ret += p[hi[9]];
      case 9:
        ret += p[hi[8]];
      case 8:
        ret += p[hi[7]];
      case 7:
        ret += p[hi[6]];
      case 6:
        ret += p[hi[5]];
      case 5:
        ret += p[hi[4]];
      case 4:
        ret += p[hi[3]];
      case 3:
        ret += p[hi[2]];
      case 2:
        ret += p[hi[1]];
      case 1:
        ret += p[hi[0]];
    }
    switch (lo_cnt) {
      case 11:
        ret -= p[lo[10]];
      case 10:
        ret -= p[lo[9]];
      case 9:
        ret -= p[lo[8]];
      case 8:
        ret -= p[lo[7]];
      case 7:
        ret -= p[lo[6]];
      case 6:
        ret -= p[lo[5]];
      case 5:
        ret -= p[lo[4]];
      case 4:
        ret -= p[lo[3]];
      case 3:
        ret -= p[lo[2]];
      case 2:
        ret -= p[lo[1]];
      case 1:
        ret -= p[lo[0]];
    }
  }
  for (int i = x1 - 1; i; i &= (i - 1)) {
    int *p = &bit[i][0];
    switch (hi_cnt) {
      case 11:
        ret -= p[hi[10]];
      case 10:
        ret -= p[hi[9]];
      case 9:
        ret -= p[hi[8]];
      case 8:
        ret -= p[hi[7]];
      case 7:
        ret -= p[hi[6]];
      case 6:
        ret -= p[hi[5]];
      case 5:
        ret -= p[hi[4]];
      case 4:
        ret -= p[hi[3]];
      case 3:
        ret -= p[hi[2]];
      case 2:
        ret -= p[hi[1]];
      case 1:
        ret -= p[hi[0]];
    }
    switch (lo_cnt) {
      case 11:
        ret += p[lo[10]];
      case 10:
        ret += p[lo[9]];
      case 9:
        ret += p[lo[8]];
      case 8:
        ret += p[lo[7]];
      case 7:
        ret += p[lo[6]];
      case 6:
        ret += p[lo[5]];
      case 5:
        ret += p[lo[4]];
      case 4:
        ret += p[lo[3]];
      case 3:
        ret += p[lo[2]];
      case 2:
        ret += p[lo[1]];
      case 1:
        ret += p[lo[0]];
    }
  }
  return ret / ((y2 - y1 + 1) * (x2 - x1 + 1));
}

void change(int x, int y, int temp) {
  int tmp = temp;
  temp -= t[x][y];
  t[x][y] = tmp;
  update(x, y, temp);
}