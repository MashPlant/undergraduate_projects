#include <cstdio>
#include <cstdlib>
#include <cstring>
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
const int U = 1e7 + 5;
int cnt[U];
int main()
{
    int n = io, m = io;
    for (int i = 0; i < n; ++i)
        ++cnt[io + 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = 0, a, b; i < m; ++i)
    {
        a = io, b = io + 1;
        io.print(cnt[b] - cnt[a]);
    }
}