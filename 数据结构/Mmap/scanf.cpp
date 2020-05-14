#include "common.h"
char a[N + 1];
int main()
{
    auto beg = now();
    scanf("%s", a);
    int s = 0;
    for (int i = 0; i < N; ++i)
        s += a[i];
    if (rand() == 0)
        printf("%d\n", s);
    fprintf(stderr, "%.2lfms\n", 1000 * duration<double>(now() - beg).count());
}