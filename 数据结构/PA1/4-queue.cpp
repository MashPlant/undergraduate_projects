#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
struct IO
{
    char *p_in = in;
    char in[1 << 24];
    IO() { fread(in, 1, sizeof in, stdin); }
    operator int()
    {
        unsigned v = 0;
        while (*p_in < '0')
            p_in++;
        do
            v = v * 10 + *p_in++ - '0';
        while (*p_in > ' ');
        return v;
    }
} io;
const int N = 1000000 + 5;
struct P
{
    int id, val;
} que[N];
int main()
{
    int n = io * 2;
    P *beg = que + 1, *end = que + 1;
    int in_cnt = 0, out_cnt = 0;
    int cur = 0;
    long long ans = 0;
    while (n--)
    {
        int d = io;
        ans += (long long)d * cur;
        if (*io.p_in == ' ')
        {
            ++in_cnt;
            int x = io;
            beg[-1].val = INT_MAX;
            while (end[-1].val < x)
                --end;
            *end++ = {in_cnt, x};
            cur = beg->val;
        }
        else
        {
            if (beg->id == ++out_cnt)
                cur = (++beg)->val;
        }
    }
    printf("%lld\n", ans);
}