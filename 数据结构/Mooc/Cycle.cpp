#include <cstdio>
#include <cstring>
#include <cstdlib>
const int N = 200000 + 5;
char a[N], b[N];
int min_pre(char *s, int n)
{
    memcpy(s + n, s, n);
    int i = 0, j = 1, k;
    while (i < n && j < n)
    {
        for (k = 0; k < n && s[i + k] == s[j + k]; ++k)
            ;
        if (k == n)
            break;
        if (s[i + k] > s[j + k])
            i += k + 1, i += i == j;
        else
            j += k + 1, j += i == j;
    }
    return i < j ? i : j;
}

int main()
{
    while (~scanf("%s%s", a, b))
    {
        int n = strlen(a), m = strlen(b);
        if (n != m)
        {
            puts("NO");
            continue;
        }
        int a_min = min_pre(a, n), b_min = min_pre(b, n);
        puts(memcmp(a + a_min, b + b_min, n) == 0 ? "YES" : "NO");
    }
}