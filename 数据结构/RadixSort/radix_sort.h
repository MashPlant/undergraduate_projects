#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
void radix_sort(unsigned *a, int n) {
  const int U = 256;
  int cnt[U];
  unsigned *aux = (unsigned *)malloc(sizeof(unsigned) * n);
  memset(cnt, 0, sizeof cnt);
  for (int i = 0; i < n; ++i)
    ++cnt[a[i] & U - 1];
  for (int i = 1; i < U; ++i)
    cnt[i] += cnt[i - 1];
  for (int i = n - 1; i >= 0; --i)
    aux[--cnt[a[i] & U - 1]] = a[i];
  memset(cnt, 0, sizeof cnt);
  for (int i = 0; i < n; ++i)
    ++cnt[aux[i] >> 8 & U - 1];
  for (int i = 1; i < U; ++i)
    cnt[i] += cnt[i - 1];
  for (int i = n - 1; i >= 0; --i)
    a[--cnt[aux[i] >> 8 & U - 1]] = aux[i];
  memset(cnt, 0, sizeof cnt);
  for (int i = 0; i < n; ++i)
    ++cnt[a[i] >> 16 & U - 1];
  for (int i = 1; i < U; ++i)
    cnt[i] += cnt[i - 1];
  for (int i = n - 1; i >= 0; --i)
    aux[--cnt[a[i] >> 16 & U - 1]] = a[i];
  memset(cnt, 0, sizeof cnt);
  for (int i = 0; i < n; ++i)
    ++cnt[aux[i] >> 24];
  for (int i = 1; i < U; ++i)
    cnt[i] += cnt[i - 1];
  for (int i = n - 1; i >= 0; --i)
    a[--cnt[aux[i] >> 24]] = aux[i];
  free(aux);
}

// able to handle neg ints
void radix_sort(int *a, int n) {
  const int U = 256;
  int cnt[U];
  int *aux = (int *)malloc(sizeof(int) * n);
  memset(cnt, 0, sizeof cnt);
  for (int i = 0; i < n; ++i)
    ++cnt[a[i] & U - 1];
  for (int i = 1; i < U; ++i)
    cnt[i] += cnt[i - 1];
  for (int i = n - 1; i >= 0; --i)
    aux[--cnt[a[i] & U - 1]] = a[i];
  memset(cnt, 0, sizeof cnt);
  for (int i = 0; i < n; ++i)
    ++cnt[aux[i] >> 8 & U - 1];
  for (int i = 1; i < U; ++i)
    cnt[i] += cnt[i - 1];
  for (int i = n - 1; i >= 0; --i)
    a[--cnt[aux[i] >> 8 & U - 1]] = aux[i];
  memset(cnt, 0, sizeof cnt);
  for (int i = 0; i < n; ++i)
    ++cnt[a[i] >> 16 & U - 1];
  for (int i = 1; i < U; ++i)
    cnt[i] += cnt[i - 1];
  for (int i = n - 1; i >= 0; --i)
    aux[--cnt[a[i] >> 16 & U - 1]] = a[i];
  memset(cnt, 0, sizeof cnt);
  for (int i = 0; i < n; ++i)
    ++cnt[aux[i] >> 24 & U - 1];
  for (int i = U / 2 + 1; i < U + U / 2; ++i)
    cnt[i & U - 1] += cnt[(i - 1) & U - 1];
  for (int i = n - 1; i >= 0; --i)
    a[--cnt[aux[i] >> 24 & U - 1]] = aux[i];
  free(aux);
}

// sort positive float numbers
void radix_sort(float *a, int n) {
  radix_sort(reinterpret_cast<unsigned *>(a), n);
}

void radix_sort_neg(float *b, int n) {
  unsigned *a = reinterpret_cast<unsigned *>(b);
  for (int i = 0; i < n; ++i)
    a[i] = (a[i] >> 31 & 1) ? ~a[i] : a[i] | 0x80000000;
  radix_sort(a, n);
  for (int i = 0; i < n; ++i)
    a[i] = (a[i] >> 31 & 1) ? a[i] & 0x7fffffff : ~a[i];
}