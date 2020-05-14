#include <cstring>
#include <cstdio>
#include <cstdlib>
const int N = 1600000 + 5;
struct IO
{
    char *p_in = in, *p_out = out;
    char in[N * 15 + 2333], out[N * 10 + 2333];

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
    ~IO() { fwrite(out, 1, p_out - out, stdout); }
    void push()
    {
        *p_out++ = 'p', *p_out++ = 'u', *p_out++ = 's', *p_out++ = 'h', *p_out++ = '\n';
    }
    void pop()
    {
        *p_out++ = 'p', *p_out++ = 'o', *p_out++ = 'p', *p_out++ = '\n';
    }
    void no()
    {
        p_out = out;
        *p_out++ = 'N', *p_out++ = 'o', *p_out++ = '\n';
    }
} io;
int que[N], stk[N];

int main()
{
    int n = io, m = io;
    int head = 0, top = 0;
    for (int i = 0; i < n; ++i)
        que[i] = io;
    for (int i = 1; i <= n; ++i)
    {
        io.push();
        stk[top++] = i;
        if (top > m)
            return io.no(), 0;
        while (top && stk[top - 1] == que[head])
            io.pop(), --top, ++head;
    }
    if (top)
        io.no();
}