#include <cstdio>
#include <cmath>
const int N = 2e5;
double dp[N + 1];

int main()
{
    dp[1] = 0, dp[2] = 1;
    for (int i = 3; i <= N; ++i)
    {
        double fac = 6.0 / i / (i - 1) / (i - 2);
        double ans = 0.0;
        for (int j = 1; j <= i - 2; ++j)
            ans += fac * (i - 1 - j) * j * (dp[j] + dp[i - 1 - j]);
        dp[i] = ans + i + 5.0 / 3.0;
    }
    for (int i = N / 20; i <= N; i += N / 20)
        printf("%d %.4lf\n", i, dp[i]);
}