#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

const int N = 501000 + 5;
char in[N * 2 + 100];
int dp_[2 * N];
constexpr int *dp = dp_ + N;

int read(char *&p_in)
{
    unsigned v = 0;
    while (*p_in < '0')
        p_in++;
    do
        v = v * 10 + *p_in++ - '0';
    while (*p_in > ' ');
    return v;
}

int main()
{
#ifndef _OJ_
    freopen("in.txt", "r", stdin);
#endif
    fread(in, 1, sizeof in, stdin);
    char *p_in = in;
    int n = read(p_in), m = read(p_in), k = read(p_in);
    while (isspace(*p_in))
        ++p_in;
    char *a = p_in, *b = strchr(p_in, '\n') + 1;
    a[n] = -1, b[m] = -2;
    // algorithm of EUGENE W. MYERS
    // find lcs in O(ND) time and O(N) space
    for (int d = 0; d <= k; ++d)
    {
        for (int i = -d; i <= d; i += 2)
        {
            int x = (i == -d || (i != d && dp[i - 1] < dp[i + 1])) ? dp[i + 1] : dp[i - 1] + 1;
            int y = x - i;
            while (a[x] == b[y])
                ++x, ++y;
            dp[i] = x;
            if (x >= n && y >= m)
                return printf("%d\n", d), 0;
        }
    }
    puts("-1");
}