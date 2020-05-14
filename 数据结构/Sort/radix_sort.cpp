#include <cstring>
const int N = 1e7 + 5;
int aux[N];

void radix_sort(int *a, int n)
{
    const int U = 256;
    int cnt[U];
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
}