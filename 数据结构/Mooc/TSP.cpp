#include <cstdio>
#include <cstring>
const int N = 1000000 + 5, M = 1000000 + 5;
struct 
{
    int to, nxt;
} es[M];
int head[N], ecnt, dp[N];
void add(int f, int t)
{
    es[++ecnt].to = t, es[ecnt].nxt = head[f], head[f] = ecnt;
}
int max(int a, int b) { return a < b ? b : a; }
int dfs(int x)
{
    if (dp[x])
        return dp[x];
    int ans = 0;
    for (int e = head[x]; e; e = es[e].nxt)
        ans = max(ans, dfs(es[e].to));
    return dp[x] = ans + 1;
}
int main()
{
    int n, m, ans = 0;
    scanf("%d%d", &n, &m);
    for (int i = 0, a, b; i < m; ++i)
        scanf("%d%d", &a, &b), add(a, b);
    for (int i = 1; i <= n; ++i)
        ans = max(ans, dfs(i));
    printf("%d\n", ans);
}