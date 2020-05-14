#include <cstdio>
const int N = 4000000 + 5;
int pre[N], post[N], map[N];

void work(int l1, int r1, int l2, int r2)
{
    if (l1 == r1)
        return void(printf("%d ", pre[l1]));
    int x = map[pre[l1 + 1]];
    work(l1 + 1, l1 + 1 + x - l2, l2, x);
    printf("%d ", pre[l1]);
    work(l1 + 2 + x - l2, r1, x + 1, r2);
}

int main()
{
    int n;
    scanf("%d", &n);
    for (int i = 1; i <= n; ++i)
        scanf("%d", pre + i);
    for (int i = 1; i <= n; ++i)
        scanf("%d", post + i), map[post[i]] = i;
    work(1, n, 1, n);
}