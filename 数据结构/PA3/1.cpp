#include <cstdio>
#include <cstring>

const int N = 500000 + 5;

template<typename T>
T max(T a, T b)
{ return a < b ? b : a; }

template<typename T, typename... Args>
T max(T a, Args... bs)
{ return max(a, max(bs...)); }

struct IO
{
    char *p_in = in, *p_out = out;
    char in[1 << 24], out[1 << 24];
    unsigned table[10000];

    IO()
    {
#ifndef _OJ_
        freopen("in.txt", "r", stdin);
#endif
        fread(in, 1, sizeof in, stdin);
        for (int i = 0; i < 10000; i++)
        {
            unsigned v = i;
            char *o = (char *) (table + i);
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
    { fwrite(out, 1, p_out - out, stdout); }

    operator unsigned()
    {
        unsigned v = 0, s = 0;
        while (*p_in < '0')
            s = *p_in++ == '-';
        do
            v = v * 10 + *p_in++ - '0';
        while (*p_in > ' ');
        return s ? -v : v;
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
        char *f = (char *) b;
        f += 3 - (*f >> 4);
        memcpy(p_out, f, ((char *) blocks + 12) - f);
        p_out += ((char *) blocks + 12) - f;
        *p_out++ = '\n';
    }
} io;

struct Node
{
    int lmax, rmax, max, sum;
} seg[N * 4];
int map[N];

void update(Node &p, const Node &l, const Node &r)
{
    p.sum = l.sum + r.sum;
    p.lmax = max(l.lmax, l.sum + r.lmax);
    p.rmax = max(r.rmax, l.rmax + r.sum);
    p.max = max(l.max, r.max, l.rmax + r.lmax);
}

void update(int x)
{ update(seg[x], seg[x << 1], seg[x << 1 | 1]); }

void modify(int pos, int nval)
{
    pos = map[pos];
    seg[pos].sum = nval;
    seg[pos].lmax = seg[pos].rmax = seg[pos].max = max(0, nval);
    for (pos >>= 1; pos; pos >>= 1)
        update(pos);
}

void build(int rt, int l, int r)
{
    if (l == r)
    {
        int in = io;
        seg[rt].sum = in;
        seg[rt].lmax = seg[rt].rmax = seg[rt].max = max(0, in);
        map[l] = rt;
        return;
    }
    int mid = (l + r) >> 1;
    build(rt << 1, l, mid);
    build(rt << 1 | 1, mid + 1, r);
    update(rt);
}

int ql, qr;

Node query(int rt, int l, int r)
{
    if (ql <= l && r <= qr)
        return seg[rt];
    int mid = (l + r) >> 1;
    Node lres{}, rres{}, res{};
    if (ql <= mid)
        lres = query(rt << 1, l, mid);
    if (qr >= mid + 1)
        rres = query(rt << 1 | 1, mid + 1, r);
    update(res, lres, rres);
    return res;
}

int main()
{
    int n = io, m = io;
    build(1, 1, n);
    while (m--)
    {
        if (io == 0)
        {
            int pos = io, nval = io;
            modify(pos, nval);
        }
        else
        {
            ql = io, qr = io;
            io.print(query(1, 1, n).max);
        }
    }
}