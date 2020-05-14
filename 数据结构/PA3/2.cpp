#include <cstdio>
#include <cstdlib>

const int N = 16777216;
using u32 = unsigned;
using u64 = unsigned long long;

// io
char s[1 << 19];
void print(u32 x, u32 len)
{
    for (u32 i = len - 1; ~i; --i)
        putchar('0' + (x >> i & 1));
}

// bitset
u64 tbl[N >> 6];
void ins(u32 x) { tbl[x >> 6] |= 1ull << (x & 63); }
void ins(u32 x, u64 v) { tbl[x >> 6] |= v << (x & 63); }
bool get(u32 x) { return tbl[x >> 6] & (1ull << (x & 63)); }

u32 min(u32 a, u32 b) { return a < b ? a : b; }

// magic, or adjacent bit of 64 bit int and get a 32 bit int
// e.g.: 0b11100100 -> 0b1110
u32 or_adjacent_bit(u64 x)
{
    x = (x | (x >> 1)) & 0x5555555555555555ull;
    x = (x | (x >> 1)) & 0x3333333333333333ull;
    x = (x | (x >> 2)) & 0x0F0F0F0F0F0F0F0Full;
    x = (x | (x >> 4)) & 0x00FF00FF00FF00FFull;
    x = (x | (x >> 8)) & 0x0000FFFF0000FFFFull;
    x = (x | (x >> 16)) & 0x00000000FFFFFFFFull;
    return x;
}

// magic: extract the '1' in string to a 8-bit int
// x is the bit presentation of char[8] containing only '0' & '1'
// notice that '0' is even and '1' is odd so in the first step, '0' is filtered out
// warning: be cautious about endian
u32 extract1(u64 x)
{
    x &= 0x0101010101010101ull;
    x = (x | (x << 9)) & 0x0300030003000300ull;
    x = (x | (x << 18)) & 0x0f0000000f000000ull;
    x = (x | (x << 36)) & 0xff00000000000000ull;
    return x >> 56;
}

// return the remain postfix
u32 input()
{
#ifndef _OJ_
    freopen("in.txt", "r", stdin);
#endif
    char *end = s + fread(s, 1, sizeof s, stdin), *it;
    int cnt;
    u32 x = 0;
    if (end - s < 25) // consider the last '\n'
    {
        // trivial case, handle it all here, will not return to main
        for (u32 l = 1; l <= 5; ++l)
            for (u32 i = 0; i < (1 << l); ++i)
            {
                bool find = false;
                for (x = 0, it = s; it < end - 1; ++it)
                    if ((x = (x << 1 | (it[0] & 1)) & ((1 << l) - 1)) == i)
                        find = true;
                if (!find)
                    print(i, l), exit(0);
            }
        exit(0);
    }
    else
    {
        // specially handle first input
        for (it = s, cnt = 23; cnt; ++it, --cnt)
            x = x << 1 | (it[0] & 1);
        if (end == s + sizeof s)
            do
            {
                for (; it < end - 7; it += 8)
                {
                    x = x << 8 | extract1(*(u64 *)it);
                    ins((x >> 7) & ((1 << 24) - 1));
                    ins((x >> 6) & ((1 << 24) - 1));
                    ins((x >> 5) & ((1 << 24) - 1));
                    ins((x >> 4) & ((1 << 24) - 1));
                    ins((x >> 3) & ((1 << 24) - 1));
                    ins((x >> 2) & ((1 << 24) - 1));
                    ins((x >> 1) & ((1 << 24) - 1));
                    ins(x & ((1 << 24) - 1));
                }
                for (; it < end; ++it)
                    ins(x = (x << 1 | (it[0] & 1)) & ((1 << 24) - 1));
                end = (it = s) + fread(s, 1, sizeof s, stdin);
            } while (end == s + sizeof s);
        for (; it < end - 1; ++it)
            ins(x = (x << 1 | (it[0] & 1)) & ((1 << 24) - 1));
        return x;
    }
}

const u32 inf = -1u;


u32 check_empty(u32 max)
{
    if (max < 64)
    {
        for (u32 i = 0; i < max; ++i)
            if (!get(i))
                return i;
        return inf;
    }
    else
    {
        for (u32 i = 0; i < (max >> 6); ++i)
            if (tbl[i] != -1ull)
            {
                for (u32 j = 0; j < 64; ++j)
                    if (~tbl[i] >> j & 1)
                        return i << 6 | j;
            }
        return inf;
    }
}


int main()
{
    u32 rem = input(), ans = check_empty(1 << 24), ans_len = 24, tmp;
    for (u32 u = 1 << 23; u >= (1 << 6); u >>= 1)
    {
        // handle i = 0 specially
        for (u32 i = 0; i < 64; ++i)
            ins(i, get(i << 1) || get(i << 1 | 1));
        for (u32 i = 1; i < (u >> 6); ++i)
        {
            ((u32 *)(tbl + i))[1] |= or_adjacent_bit(tbl[i << 1 | 1]);
            ((u32 *)(tbl + i))[0] |= or_adjacent_bit(tbl[i << 1]);
        }
        ins(rem & (u - 1));
        if ((tmp = check_empty(u)) != inf)
            ans = tmp, --ans_len;
    }
    for (u32 u = 1 << 5; u >= 2; u >>= 1)
    {
        for (u32 i = 0; i < u; ++i)
            ins(i, get(i << 1) || get(i << 1 | 1));
        ins(rem & (u - 1));
        if ((tmp = check_empty(u)) != inf)
            ans = tmp, --ans_len;
    }
    print(ans, ans_len);
}