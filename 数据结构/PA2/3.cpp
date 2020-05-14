#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <sys/mman.h>
#include <sys/stat.h>
#define foreach(head) for (int e = head; __builtin_prefetch(es + es[e].nxt, 0), e; e = es[e].nxt)

const int N = 500000 + 5, M = 1200000 + 5;

struct IO
{
    char *p_in;

    IO()
    {
#ifndef _OJ_
        freopen("in.txt", "r", stdin);
#endif
        struct stat st;
        int fn = fileno(stdin);
        fstat(fn, &st);
        p_in = (char *)mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fn, 0);
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
template <typename T>
T max(T a, T b) { return a < b ? b : a; }

struct
{
    int to, nxt;
} es[2 * M];
int n;
int head0[N], ecnt;
bool money[N];

void add(int f, int t)
{
    es[++ecnt].to = t, es[ecnt].nxt = head0[f], head0[f] = ecnt;
}

namespace q1
{
int que[N], que1[N];
bool vis[N];

int solve()
{
    int ret = !money[1];
    int beg = 0, end = 0;
    que[end++] = 1, vis[1] = true;
    while (true)
    {
        int beg1 = 0, end1 = 0;
        while (beg != end)
        {
            int x = que[beg++];
            if (x == n)
                return ret;
            que1[end1++] = x;
            foreach (head0[x])
            {
                int t = es[e].to;
                if (vis[t] || !money[t])
                    continue;
                vis[t] = true;
                que[end++] = t;
            }
        }
        ++ret;
        beg = end = 0;
        while (beg1 != end1)
        {
            int x = que1[beg1++];
            foreach (head0[x])
            {
                int t = es[e].to;
                if (vis[t] || money[t])
                    continue;
                vis[t] = true;
                que[end++] = t;
            }
        }
    }
}
} // namespace q1

namespace q2
{
int head1[N], val[N], dp[N];
int low[N], dfn[N], stk[N], top, dfs_cnt, color[N], color_cnt;
bool on_stk[N];

void tarjan(int x)
{
    on_stk[stk[top++] = x] = true;
    low[x] = dfn[x] = ++dfs_cnt;
    foreach (head0[x])
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
        int cur;
        do
        {
            cur = stk[--top];
            on_stk[cur] = false;
            color[cur] = color_cnt;
        } while (cur != x);
    }
}

void add1(int f, int t)
{
    es[++ecnt].to = t, es[ecnt].nxt = head1[f], head1[f] = ecnt;
}

int solve()
{
    tarjan(1);
    for (int i = 1; i <= n; ++i)
        if (dfn[i])
        {
            val[color[i]] += money[i];
            foreach (head0[i])
                if (color[es[e].to] != color[i])
                    add1(color[es[e].to], color[i]);
        }
    for (int i = color_cnt; i >= 1; --i)
    {
        foreach (head1[i])
            dp[i] = max(dp[i], dp[es[e].to]);
        dp[i] += val[i];
    }
    return dp[color[n]];
}
} // namespace q2

int main()
{
    ::n = io;
    int m = io;
    while (isspace(*io.p_in))
        ++io.p_in;
    for (int i = 1; i <= n; ++i)
        money[i] = *io.p_in++ == 'm';
    for (int i = 1, a, b; i <= m; ++i)
        a = io + 1, b = io + 1, add(a, b);
    printf("%d\n%d\n", q1::solve(), q2::solve());
}