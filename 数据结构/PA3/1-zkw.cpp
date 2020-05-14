#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#pragma GCC optimize("O3")

template <typename T>
T max(T a, T b)
{
    return a < b ? b : a;
}

template <typename T, typename... Args>
T max(T a, Args... bs)
{
    return max(a, max(bs...));
}

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
        blocks[0] = o;
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

struct Node
{
    int lmax, rmax, max, sum;
} seg[(1 << 20) + 5]; // smallest power of 2 that >= 2 * maxn
int n, N;

void update(Node *__restrict p, const Node *__restrict l, const Node *__restrict r)
{
    p->max = max(l->max, r->max, l->rmax + r->lmax);
    p->lmax = max(l->lmax, l->sum + r->lmax);
    p->rmax = max(r->rmax, l->rmax + r->sum);
    p->sum = l->sum + r->sum;
}

void update(int x)
{
    update(seg + x, seg + (x << 1), seg + (x << 1 | 1));
}
void build()
{
    for (N = 1; N <= n; N <<= 1)
        ;
    for (int i = N + 1; i <= N + n; ++i)
        seg[i].lmax = seg[i].rmax = seg[i].max = max(0, seg[i].sum = io);
    for (int i = N - 1; i > 0; --i)
        update(i);
}

void modify(int i, int val)
{
    i += N;
    seg[i].lmax = seg[i].rmax = seg[i].max = max(0, seg[i].sum = val);
    for (; i >>= 1;)
        update(i);
}

int query(int l, int r)
{
    int l_rmax = 0, l_max = 0, l_sum = 0; // a node missing field lmax
    int r_lmax = 0, r_max = 0, r_sum = 0; // a node missing field rmax
    for (l += N - 1, r += N + 1; l ^ r ^ 1 /* while l & r are not sibling */; l >>= 1, r >>= 1)
    {
        // basically it's just 'update(lres, lres, seg[l ^ 1])' and '...'
        // but a field of node is not used for answer, so manually remove it here
        if (~l & 1) // l is left child
        {
            const Node &r = seg[l ^ 1];
            l_max = max(l_max, r.max, l_rmax + r.lmax);
            l_rmax = max(r.rmax, l_rmax + r.sum);
            l_sum += r.sum;
        }
        if (r & 1) // r is right child
        {
            const Node &l = seg[r ^ 1];
            r_max = max(l.max, r_max, l.rmax + r_lmax);
            r_lmax = max(l.lmax, l.sum + r_lmax);
            r_sum += l.sum;
        }
    }
    return max(l_max, r_max, l_rmax + r_lmax);
}

int ans[500000 + 5];
int main()
{
    n = io;
    int m = io;
    build();
    int *ans_ptr = ans;
    while (m--)
    {
        if (io == 0)
        {
            int pos = io, nval = io;
            modify(pos, nval);
        }
        else
        {
            int ql = io, qr = io;
            *ans_ptr++ = query(ql, qr);
        }
    }
    // save all ans and print them together
    // this makes space locality better
    for (int *it = ans; it < ans_ptr; ++it)
        io.print(*it);
}