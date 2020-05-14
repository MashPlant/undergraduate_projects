#include <cstdio>
#include <cstdlib>

using u32 = unsigned;
using u64 = unsigned long long;
u32 extract1(u64 x)
{
    x &= 0x0101010101010101ull;
    x = (x | (x << 9)) & 0x0300030003000300ull;
    x = (x | (x << 18)) & 0x0f0000000f000000ull;
    x = (x | (x << 36)) & 0xff00000000000000ull;
    // x = (x | (x >> 7)) & 0x0003000300030003ull;
    // x = (x | (x >> 14)) & 0x0000000f0000000full;
    // x = (x | (x >> 28)) & 0x00000000000000ffull;
    return x >> 56;
}

u32 extract2(char *it)
{
    return ((it[0] & 1) << 7) | ((it[1] & 1) << 6) | ((it[2] & 1) << 5) |
           ((it[3] & 1) << 4) | ((it[4] & 1) << 3) | ((it[5] & 1) << 2) | ((it[6] & 1) << 1) | ((it[7] & 1));
}

char ch[9];
void test(int i)
{
    if (i == 8)
    {
        if (extract1(*(u64 *)ch) != extract2(ch))
        {
            puts(ch);
            printf("%d\n", extract1(*(u64 *)ch));
            printf("%d\n", extract2(ch));
        }
        return;
    }
    ch[i] = '0';
    test(i + 1);
    ch[i] = '1';
    test(i + 1);
}

int main()
{
    // printf("%d\n", extract1(*(u64 *)"00000001"));
    test(0);
}
// #include <stdio.h>
// int main() {
//     int i = 0x11223344;
//     char *p;

//     p = (char *) &i;
//     if (*p == 0x44) {
//         printf("Little endian\n");
//     }
//     else {
//         printf("Big endian\n");
//     }
//     return 0;
// }