#include <cstdio>

using u8 = unsigned char;
using u32 = unsigned;

void print(int x, int len = 4) {
  for (int i = len - 1; i > -1; --i) {
    putchar('|');
    putchar(' '), putchar(' '), putchar(' ');
    putchar("01"[x >> i & 1]);
    putchar(' '), putchar(' '), putchar(' ');
  }
}

struct Item {
  int ab, d, m, e;
} items[16 * 16];

int main() {
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 16; ++j) {
      if (j == 0) {
        items[i * 16 + j] = {i * 16 + j, 0, 0, 1};
      } else {
        items[i * 16 + j] = {i * 16 + j, i / j, i % j, 0};
      }
    }
  }
  for (auto x : items) {
    print(x.ab % 16), print(x.ab / 16), print(x.d), print(x.m), print(x.e, 1);
    puts("|");
  }

  u32 simplify(const u8 *tbl, u32 len, u8(*out)[8]);

  for (int i = 3; i > -1; --i) {
    printf("D_%d & = & ", i);
    int newline = 1, len = 0;
    u8 tbl[256], sim[256][8];
    for (auto x : items) {
      if (x.d >> i & 1) {
        for (u32 j = 7; ~j; --j) {
          char ch = "BBBBAAAA"[j];
          if (x.ab >> j & 1) {
            printf("%c_%d", ch, j % 4);
          } else {
            printf("\\overline{%c_%d}", ch, j % 4);
          }
        }
        if (++newline == 3) {
          printf(" \\\\ && + ");
          newline = 0;
        } else {
          printf(" + ");
        }
        tbl[len++] = x.ab;
      }
    }
    len = simplify(tbl, len, sim);
    puts("\\\\ & = &");
    newline = 1;
    for (u32 i = 0; i < len; ++i) {
      for (u32 j = 7; ~j; --j) {
        char ch = "BBBBAAAA"[j];
        if (sim[i][j] == 1) {
          printf("%c_%d", ch, j % 4);
        } else if (sim[i][j] == 0) {
          printf("\\overline{%c_%d}", ch, j % 4);
        }
      }
      if (++newline == 3) {
        printf(" \\\\ && + ");
        newline = 0;
      } else {
        printf(" + ");
      }
    }
    puts("\n");
  }

  for (int i = 3; i > -1; --i) {
    printf("M_%d & = & ", i);
    int newline = 1, len = 0;
    u8 tbl[256], sim[256][8];
    for (auto x : items) {
      if (x.m >> i & 1) {
        for (u32 j = 7; ~j; --j) {
          char ch = "BBBBAAAA"[j];
          if (x.ab >> j & 1) {
            printf("%c_%d", ch, j % 4);
          } else {
            printf("\\overline{%c_%d}", ch, j % 4);
          }
        }
        if (++newline == 3) {
          printf(" \\\\ && + ");
          newline = 0;
        } else {
          printf(" + ");
        }
        tbl[len++] = x.ab;
      }
    }
    len = simplify(tbl, len, sim);
    puts("\\\\ & = &");
    newline = 1;
    for (u32 i = 0; i < len; ++i) {
      for (u32 j = 7; ~j; --j) {
        char ch = "BBBBAAAA"[j];
        if (sim[i][j] == 1) {
          printf("%c_%d", ch, j % 4);
        } else if (sim[i][j] == 0) {
          printf("\\overline{%c_%d}", ch, j % 4);
        }
      }
      if (++newline == 3) {
        printf(" \\\\ && + ");
        newline = 0;
      } else {
        printf(" + ");
      }
    }
    puts("\n");
  }

  for (int i = 0; i > -1; --i) {
    printf("Err & = & ", i);
    int newline = 1, len = 0;
    u8 tbl[256], sim[256][8];
    for (auto x : items) {
      if (x.e >> i & 1) {
        for (u32 j = 7; ~j; --j) {
          char ch = "BBBBAAAA"[j];
          if (x.ab >> j & 1) {
            printf("%c_%d", ch, j % 4);
          } else {
            printf("\\overline{%c_%d}", ch, j % 4);
          }
        }
        if (++newline == 3) {
          printf(" \\\\ && + ");
          newline = 0;
        } else {
          printf(" + ");
        }
        tbl[len++] = x.ab;
      }
    }
    len = simplify(tbl, len, sim);
    puts("\\\\ & = &");
    newline = 1;
    for (u32 i = 0; i < len; ++i) {
      for (u32 j = 7; ~j; --j) {
        char ch = "BBBBAAAA"[j];
        if (sim[i][j] == 1) {
          printf("%c_%d", ch, j % 4);
        } else if (sim[i][j] == 0) {
          printf("\\overline{%c_%d}", ch, j % 4);
        }
      }
      if (++newline == 3) {
        printf(" \\\\ && + ");
        newline = 0;
      } else {
        printf(" + ");
      }
    }
    puts("\n");
  }
}