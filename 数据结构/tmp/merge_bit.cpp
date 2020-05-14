#include <stdint.h>
#include <x86intrin.h>
#include <cstdio>
#pragma GCC target("bmi2")
using u32 = unsigned;
using u64 = unsigned long long;

u32 calc(u64 a)
{
    return __builtin_ia32_pext_di(a, 0x5555555555555555ull) |
           __builtin_ia32_pext_di(a, 0xaaaaaaaaaaaaaaaaull);
}

u64 calc2(u64 n)
{
    // (odd | even)
    u64 x = (n | (n >> 1)) & 0x5555555555555555ull; // single bits

    // ... the restdeinterleave
    x = (x | (x >> 1)) & 0x3333333333333333ull;  // bit pairs
    x = (x | (x >> 2)) & 0x0F0F0F0F0F0F0F0Full;  // nibbles
    x = (x | (x >> 4)) & 0x00FF00FF00FF00FFull;  // octets
    x = (x | (x >> 8)) & 0x0000FFFF0000FFFFull;  // halfwords
    x = (x | (x >> 16)) & 0x00000000FFFFFFFFull; // words

    return x;
}
void print_bit(u64 x)
{
    for (int i = 63; i >= 0; --i)
        printf("%d", (int)(x >> i & 1));
    puts("");
}

u32 extract1(u64 x)
{
    print_bit(x);
    x &= 0x0101010101010101ull;
    print_bit(x);
    print_bit(x);
    x = (x | (x >> 8)) & 0x0000FFFF0000FFFFull;
    print_bit(x);
    x = (x | (x >> 16)) & 0x00000000FFFFFFFFull;
    print_bit(x);
    return x;
}

int main()
{
    printf("%x\n", extract1(*(u64 *)"01010101"));
}