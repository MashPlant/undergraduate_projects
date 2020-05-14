#include "common.h"
char a[N];
int main()
{
    for (int i = 0; i < N; ++i)
        a[i] = "qwertyuiopasdfghjklzxcvbnm"[i % 26];
    fwrite(a, 1, N, stdout);
}