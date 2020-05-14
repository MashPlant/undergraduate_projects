#include <cstdio>

int f(int n)
{
    switch (n)
    {
    case 0:
    case 1:
        return 0;
    case 2:
    case 3:
        return 1;
    // case 4:
    //     return 3;
    default:
    {
        int n4 = n / 4;
        return 3 * f(n4) + f(n - n4 * 3) + n;
    }
    }
}
void p(int n) { printf("T(%d)=%d\n", n, f(n)); }
int main()
{
    p(10);
    p(100);
    p(1000);
    p(10000);
    p(100000);
    p(1000000);
}