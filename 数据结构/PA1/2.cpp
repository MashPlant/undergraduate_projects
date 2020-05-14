#include <cstdio>
#include <cstdlib>
#include <cstring>
const int N = 200000 + 5;
int aux[N];
void radix_sort(int *a, int n)
{
    // wys radix sort
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
struct IO
{
    char *p_in = in, *p_out = out;
    char in[1 << 24], out[1 << 24];
    unsigned table[10000];
    IO()
    {
        fread(in, 1, sizeof in, stdin);
        for (int i = 0; i < 10000; i++)
        {
            unsigned v = i;
            char *o = (char *)(table + i);
            o[3] = v % 10 + '0';
            o[2] = (v % 100) / 10 + '0';
            o[1] = (v % 1000) / 100 + '0';
            o[0] = (v % 10000) / 1000;
            if (o[0])
                o[0] |= 0x30;
            else if (o[1] != '0')
                o[0] |= 0x20;
            else if (o[2] != '0')
                o[0] |= 0x10;
            else
                o[0] |= 0x00;
        }
    }
    ~IO() { fwrite(out, 1, p_out - out, stdout); }
    operator unsigned()
    {
        unsigned v = 0;
        while (*p_in < '0')
            p_in++;
        do
            v = v * 10 + *p_in++ - '0';
        while (*p_in > ' ');
        return v;
    }
    void print(unsigned o)
    {
        unsigned blocks[3], *b = blocks + 2;
        blocks[0] = o < 0 ? ~o + 1 : o;
        blocks[2] = blocks[0] % 10000;
        blocks[0] /= 10000;
        blocks[2] = table[blocks[2]];
        if (blocks[0])
        {
            blocks[1] = blocks[0] % 10000;
            blocks[0] /= 10000;
            blocks[1] = table[blocks[1]];
            blocks[2] |= 0x30303030;
            b--;
        }
        if (blocks[0])
        {
            blocks[0] = table[blocks[0] % 10000];
            blocks[1] |= 0x30303030;
            b--;
        }
        char *f = (char *)b;
        f += 3 - (*f >> 4);
        memcpy(p_out, f, ((char *)blocks + 12) - f);
        p_out += ((char *)blocks + 12) - f;
        *p_out++ = '\n';
    }
} io;

int x[N], y[N];
int main()
{
    int n = io;
    for (int i = 0; i < n; ++i)
        x[i] = io;
    for (int i = 0; i < n; ++i)
        y[i] = io;
    radix_sort(x, n);
    radix_sort(y, n);
    x[n] = y[n] = -1u;
    int m = io;
    while (m--)
    {
        int qx = io, qy = io;
        int l = 0, r = n;
        while (l <= r)
        {
            int mid = (l + r) >> 1;
            if ((long long)y[mid] * qx < (long long)x[mid] * (y[mid] - qy))
                r = mid - 1;
            else
                l = mid + 1;
        }
        io.print(l);
    }
}