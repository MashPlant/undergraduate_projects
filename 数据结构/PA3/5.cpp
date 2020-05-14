#include <cstdio>
#include <sys/mman.h>
#include <sys/stat.h>
#pragma GCC optimize("O3")

const int N = 20000000 + 5;
int next[N];

int main()
{
    struct stat sb;
    fstat(fileno(stdin), &sb);
    char *a = (char *)mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, fileno(stdin), 0);

    int n = sb.st_size - 1;
    for (int i = 0, j = next[0] = -1; i < n;)
        if (j == -1 || a[i] == a[j])
            next[++i] = ++j;
        else
            j = next[j];
    // the space of `next` can be reused as `dp`
    // next[next[i]] uses the old `next`, next[i] = ... store the new `dp`
    long long ans = 0;
    next[0] = 0;
    for (int i = 1; i <= n; ++i)
        ans += next[i] = 1 + next[next[i]];
    printf("%lld\n", ans);
}