#include <cstring>
#include <cstdio>
#include <cstdlib>

const int N = 4000000 + 5;
struct Node
{
    int x, y;
} a[N], aux[N];
using It = Node *;
using i64 = long long;
i64 ans;

struct IO
{
    char *p_in = in;
    char in[N * 20 + 2333];

    IO() { fread(in, 1, sizeof in, stdin); }
    operator int()
    {
        unsigned v = 0;
        while (*p_in < '0')
            p_in++;
        do
            v = v * 10 + *p_in++ - '0';
        while (*p_in > ' ');
        return v;
    }
} io;

void radix_sort(It a, int n)
{
    const int U = 256;
    int cnt[U];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[a[i].x & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        aux[--cnt[a[i].x & U - 1]] = a[i];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[aux[i].x >> 8 & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        a[--cnt[aux[i].x >> 8 & U - 1]] = aux[i];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[a[i].x >> 16 & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        aux[--cnt[a[i].x >> 16 & U - 1]] = a[i];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[aux[i].x >> 24];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        a[--cnt[aux[i].x >> 24]] = aux[i];
}

void merge_sort(It first, It last)
{
    if (first >= last - 1)
        return;
    It mid = first + (last - first) / 2;
    merge_sort(first, mid);
    merge_sort(mid, last);
    It pos1 = first, pos2 = mid, pos = aux;
    while (pos1 != mid && pos2 != last)
    {
        if (pos2->y < pos1->y)
        {
            ans += mid - pos1;
            *pos++ = *pos2++;
        }
        else
            *pos++ = *pos1++;
    }
    memcpy(pos, pos1, sizeof(Node) * (mid - pos1)), pos += mid - pos1;
    memcpy(first, aux, sizeof(Node) * (pos - aux));
}

int main()
{
    int n = io;
    for (int i = 0; i < n; ++i)
        a[i].x = io, a[i].y = -io;
    radix_sort(a, n);
    merge_sort(a, a + n);
    printf("%lld\n", ans);
}