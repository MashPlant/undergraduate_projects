#include <cstring>
#include <cstdio>

struct IO
{
    char *p_in = in;
    char in[1 << 24];

    IO()
    {
#ifndef _OJ_
        freopen("in.txt", "r", stdin);
#endif
        fread(in, 1, sizeof in, stdin);
    }

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
} io;

const int N = 100000 + 5;
const int M = 300000 + 5;
const int inf = 0x3f3f3f3f;
struct
{
    int to, val, nxt;
} es[M * 2];
int n, ecnt, head[N];
int dis[N], seg[N * 2];

void add(int f, int t, int val)
{
    es[++ecnt].to = t, es[ecnt].nxt = head[f], es[ecnt].val = val;
    head[f] = ecnt;
    es[++ecnt].to = f, es[ecnt].nxt = head[t], es[ecnt].val = val;
    head[t] = ecnt;
}

void modify(int i, int val)
{
    // set a seg node(instead of dis) to val
    for (seg[i += n] = val; i > 1; i >>= 1)
        seg[i >> 1] = dis[seg[i ^ 1]] < dis[seg[i]] ? seg[i ^ 1] : seg[i];
}

int main()
{
    n = io;
    int m = io;
    for (int i = 0; i < m; ++i)
    {
        int f = io, t = io, val = io;
        add(f, t, val);
    }
    memset(dis, 0x3f, sizeof dis);
    dis[1] = 0, modify(1, 1);
    while (true)
    {
        int cur = seg[1], w = dis[cur];
        if (cur == n)
            return printf("%d\n", dis[cur]), 0;
        for (int e = head[cur]; e; e = es[e].nxt)
        {
            int t = es[e].to, tmp;
            if ((tmp = w + es[e].val) < dis[t])
                dis[t] = tmp, modify(t, t);
        }
        modify(cur, 0);
    }
}