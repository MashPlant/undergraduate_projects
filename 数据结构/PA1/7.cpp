#include <cstdio>
#include <cstdlib>
#include <cstring>
const int N = 105;
int mod_inc(int x, int p) { return (++x >= p) ? 0 : x; }
int main()
{
    int n, m, ans = 0, cur = 0;
    int que[N] = {};
    bool ok[N] = {};
    scanf("%d%d", &n, &m);
    for (int _ = 0, x; _ < m; ++_)
    {
        scanf("%d", &x);
        if (ok[x])
            continue;
        ok[x] = true, ++ans;
        cur = mod_inc(cur, n);
        ok[que[cur]] = false;
        que[cur] = x;
    }
    printf("%d\n", ans);
}