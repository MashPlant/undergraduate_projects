#include "common.h"
char a[N + 1];
int main()
{
    auto beg = now();
    fread(a, 1, N, stdin);
    fprintf(stderr, "%.2lfms\n", 1000 * duration<double>(now() - beg).count());
}