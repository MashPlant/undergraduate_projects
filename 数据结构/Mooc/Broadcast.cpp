#include <cstdio>
#include <cstdlib>
const int N = 10000 + 5, M = 30000 * 2 + 5;
struct IO
{
    char *p_in = in;
    char in[1 << 20];
    IO() { fread(in, 1, sizeof in, stdin); }
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
struct
{
    int t, nxt;
} es[M];
int head[N], ecnt;
int color[N];
void add(int a, int b)
{
    es[++ecnt].t = b, es[ecnt].nxt = head[a], head[a] = ecnt;
    es[++ecnt].t = a, es[ecnt].nxt = head[b], head[b] = ecnt;
}
void dfs(int x, int c)
{
    color[x] = c;
    for (int e = head[x]; e; e = es[e].nxt)
    {
        if (!color[es[e].t])
            dfs(es[e].t, -c);
        else if (color[es[e].t] == c)
            exit(puts("-1") & 0);
    }
}
int main()
{
    int n = io, m = io;
    for (int i = 0; i < m; ++i)
        add(io, io);
    for (int i = 1; i <= n; ++i)
        if (!color[i])
            dfs(i, 1);
    puts("1");
}