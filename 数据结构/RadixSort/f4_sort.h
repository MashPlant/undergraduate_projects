
#include <stdlib.h>
#include <string.h>

#define _0(v) ((v)&0x7FF)
#define _1(v) (((v) >> 11) & 0x7FF)
#define _2(v) ((v) >> 22)
#define F4_SORT_HIST_SIZE 2048

static inline unsigned f4_sort_FloatFlip(unsigned f) {
  unsigned mask = -(f >> 31) | 0x80000000;
  return (f ^ mask);
}

static inline unsigned f4_sort_IFloatFlip(unsigned f) {
  unsigned mask = ((f >> 31) - 1) | 0x80000000;
  return (f ^ mask);
}

static inline void f4_sort(float *a, const long sz) {
  long j;
  unsigned long pos;
  long n, sum0 = 0, sum1 = 0, sum2 = 0, tsum = 0;
  unsigned *reader, *writer, *buf1 = (unsigned *)a, *buf2;
  unsigned long b0[F4_SORT_HIST_SIZE * 3], *b1, *b2;
  buf2 = (unsigned *)malloc(sz * sizeof(unsigned));

  b1 = b0 + F4_SORT_HIST_SIZE;
  b2 = b1 + F4_SORT_HIST_SIZE;
  memset(b0, 0, F4_SORT_HIST_SIZE * 3 * sizeof(unsigned long));

  for (n = 0; n < sz; n++) {

    buf1[n] = f4_sort_FloatFlip(a[n]);
    b0[_0(buf1[n])]++;
    b1[_1(buf1[n])]++;
    b2[_2(buf1[n])]++;
  }

  for (j = 0; j < F4_SORT_HIST_SIZE; j++) {
    tsum = b0[j] + sum0;
    b0[j] = sum0 - 1;
    sum0 = tsum;
    tsum = b1[j] + sum1;
    b1[j] = sum1 - 1;
    sum1 = tsum;
    tsum = b2[j] + sum2;
    b2[j] = sum2 - 1;
    sum2 = tsum;
  }

  writer = buf2; /* byte 0 */
  reader = buf1;
  for (n = 0; n < sz; n++) {
    pos = _0(reader[n]);
    writer[++b0[pos]] = reader[n];
  }

  writer = buf1; /* byte 1 */
  reader = buf2;
  for (n = 0; n < sz; n++) {
    pos = _1(reader[n]);
    writer[++b1[pos]] = reader[n];
  }

  writer = buf2; /* byte 2 */
  reader = buf1;
  for (n = 0; n < sz; n++) {
    pos = _2(reader[n]);
    writer[++b2[pos]] = f4_sort_IFloatFlip(reader[n]);
  }
  memcpy(a, writer, sz * sizeof(float));
  free(buf2);
}

#undef F4_SORT_HIST_SIZE
#undef _0
#undef _1
#undef _2