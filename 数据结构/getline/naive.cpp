#include <cstdio>
#include <cstring>
char s[1 << 18];
int main()
{
    int n;
    scanf("%d", &n);
    while (n--)
    {
        scanf("%s", s);
        puts(s);
    }
}