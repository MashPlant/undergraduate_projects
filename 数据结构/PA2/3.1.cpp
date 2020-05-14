#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>

const int N = 500000 + 5, M = 1200000 + 5;

struct IO
{
    char *p_in = in;
    char in[N + M * 20];

    IO()
    {
        fread(in, 1, sizeof in, stdin);
    }

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
template <typename T>
T min(T a, T b) { return a < b ? a : b; }

struct
{
    int to, nxt;
} es[M];
int n, head[N], ecnt;
int low[N], dfn[N], stk[N], top, dfs_cnt, color_cnt;
bool on_stk[N];
void tarjan(int x)
{
    on_stk[stk[top++] = x] = true;
    low[x] = dfn[x] = ++dfs_cnt;
    for (int e = head[x]; __builtin_prefetch(es + es[e].nxt, 0), e; e = es[e].nxt)
    {
        int t = es[e].to;
        if (!dfn[t])
            tarjan(t), low[x] = min(low[x], low[t]);
        else if (on_stk[t])
            low[x] = min(low[x], dfn[t]);
    }
    if (dfn[x] == low[x])
    {
        ++color_cnt;
        while (stk[top - 1] != x)
            on_stk[stk[--top]] = false;
        on_stk[stk[--top]] = false; // x
    }
}

void add(int f, int t)
{
    es[++ecnt].to = t, es[ecnt].nxt = head[f], head[f] = ecnt;
}

int main()
{
    ::n = io;
    int m = io;
    while (isspace(*io.p_in))
        ++io.p_in;
    io.p_in = strchr(io.p_in, '\n');
    for (int i = 1, a, b; i <= m; ++i)
        a = io + 1, b = io + 1, add(a, b);
    for (int i = 1; i <= n; ++i)
        if (!dfn[i])
            tarjan(i);
    printf("%d\n", color_cnt);
}