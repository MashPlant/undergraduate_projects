#include "common.h"
char a[N + 1];
int main()
{
    auto beg = now();
    char ch;
    int i = 0;
    while ((ch = getchar_unlocked()) != EOF)
        a[i++] = ch;
    fprintf(stderr, "%.2lfms\n", 1000 * duration<double>(now() - beg).count());
}