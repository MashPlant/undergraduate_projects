#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <functional>

using namespace std::chrono;
auto now = high_resolution_clock::now;

int *next_of(const char *p, int n)
{
    int *next = new int[n];
    for (int i = 0, j = next[0] = -1; i < n - 1;)
        if (j == -1 || p[i] == p[j])
            next[++i] = ++j;
        else
            j = next[j];
    return next;
}
int *im_next_of(const char *p, int n)
{
    int *next = new int[n];
    for (int i = 0, j = next[0] = -1; i < n - 1;)
        if (j == -1 || p[i] == p[j])
        {
            ++i, ++j;
            next[i] = p[i] != p[j] ? j : next[j];
        }
        else
            j = next[j];
    return next;
}
int match(const char *s, int n, const char *p, int m, int *next)
{
    int i = 0, j = 0;
    while (i < n && j < m)
        if (j == -1 || s[i] == p[j])
            ++i, ++j;
        else
            j = next[j];
    return i - j;
}
int match(const char *s, int n)
{
    int i = -1;
s_:
    if (n <= ++i)
        return -1;
s0:
    if (s[i] != 'c')
        goto s_;
    if (n <= ++i)
        return -1;
s1:
    if (s[i] != 'h')
        goto s0;
    if (n <= ++i)
        return -1;
s2:
    if (s[i] != 'i')
        goto s0;
    if (n <= ++i)
        return -1;
s3:
    if (s[i] != 'n')
        goto s0;
    if (n <= ++i)
        return -1;
s4:
    if (s[i] != 'c')
        goto s_;
    if (n <= ++i)
        return -1;
s5:
    if (s[i] != 'h')
        goto s0;
    if (n <= ++i)
        return -1;
s6:
    if (s[i] != 'i')
        goto s0;
    if (n <= ++i)
        return -1;
s7:
    if (s[i] != 'l')
        goto s3;
    if (n <= ++i)
        return -1;
s8:
    if (s[i] != 'l')
        goto s0;
    if (n <= ++i)
        return -1;
s9:
    if (s[i] != 'a')
        goto s0;
    return i - 9;
}
const int N = 1e8;
char s[N];
int main()
{
    const char *p = "chinchilla";
    int m = strlen(p);
    int *next = im_next_of(p, m);
    for (int i = 0; i < m; ++i)
        printf("%3c ", p[i]);
    puts("");
    for (int i = 0; i < m; ++i)
        printf("%3d ", next[i]);
    puts("");
    for (int i = 0; i < N; ++i)
        s[i] = "chinla"[rand() % 6];
    memcpy(s + N - m, p, m);
    {
        auto beg = now();
        printf("%d\n", match(s, N));
        printf("%.5lf\n", duration<double>(now() - beg).count());
    }
    {
        auto beg = now();
        printf("%d\n", match(s, N, p, m, next));
        printf("%.5lf\n", duration<double>(now() - beg).count());
    }
    {
        auto beg = now();
        printf("%d\n", (int)(std::search(s, s + N, std::default_searcher(p, p + m)) - s));
        printf("%.5lf\n", duration<double>(now() - beg).count());
    }
    {
        auto beg = now();
        printf("%d\n", (int)(std::search(s, s + N, std::boyer_moore_searcher(p, p + m)) - s));
        printf("%.5lf\n", duration<double>(now() - beg).count());
    }
    {
        auto beg = now();
        printf("%d\n", (int)(std::search(s, s + N, std::boyer_moore_horspool_searcher(p, p + m)) - s));
        printf("%.5lf\n", duration<double>(now() - beg).count());
    }
}