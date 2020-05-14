#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#pragma GCC optimize("O3")

struct IO
{
    char *p_in, *p_out = out;
    char out[1 << 23];
    unsigned table[10000];

    IO()
    {
#ifndef _OJ_
        freopen("in.txt", "r", stdin);
#endif
        struct stat st;
        int fn = fileno(stdin);
        fstat(fn, &st);
        p_in = (char *)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fn, 0);
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

    ~IO()
    {
        fwrite(out, 1, p_out - out, stdout);
    }

    operator int()
    {
        unsigned v = 0, s = 0;
        while (*p_in < '0')
            s = *p_in++ == '-';
        do
            v = v * 10 + *p_in++ - '0';
        while (*p_in > ' ');
        return s ? -v : v;
    }

    void print(int o)
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
        if (o < 0)
            *--f = '-';
        memcpy(p_out, f, ((char *)blocks + 12) - f);
        p_out += ((char *)blocks + 12) - f;
        *p_out++ = '\n';
    }
} io;
const int N = 500000 + 5;
int bit[N * 2], val[N], ans[N];

int main()
{
    int n = io, m = io;
    for (int i = 1; i <= n; ++i)
        bit[i] = val[i] = io;
    for (int i = 1; i <= n; ++i)
        bit[i + (i & -i)] += bit[i];
    int *ans_ptr = ans;
    ++io.p_in;
    while (m--)
    {
        ++io.p_in;
        int op = *io.p_in++, a = io, b = io;
        if (op == '0')
        {
            int diff = b - val[a];
            val[a] = b;
            for (; a <= n; a += a & -a)
                bit[a] += diff;
        }
        else
        {
            int ans = 0;
            for (; b; b -= b & -b)
                ans += bit[b];
            for (--a; a; a -= a & -a)
                ans -= bit[a];
            *ans_ptr++ = ans;
        }
    }
    for (int *it = ans; it < ans_ptr; ++it)
        io.print(*it);
}