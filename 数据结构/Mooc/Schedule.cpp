#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdlib>

using u32 = unsigned;
using u64 = unsigned long long;
const int N = 4000000 + 5;
u64 pri[N];
char name[N][9];
int tree[N * 2];

struct IO
{
    char *p_in = in, *p_out = out;
    char in[4000005 * 25 + 2333], out[2000000 * 10 + 2333];

    IO()
    { fread(in, 1, sizeof in, stdin); }

    ~IO()
    { fwrite(out, 1, p_out - out, stdout); }

    operator unsigned()
    {
        unsigned v = 0;
        while (*p_in < '0')
            p_in++;
        do
            v = v * 10 + *p_in++ - '0';
        while (*p_in > ' ');
        return v;
    }

    void read(char *s)
    {
        char *old = s;
        while (!isspace(*s++ = *p_in++));
        s[-1] = '\0';
    }

    void print(const char *s)
    {
        if (*s)
        {
            while (*p_out++ = *s++);
            p_out[-1] = '\n';
        }
    }
} io;

bool cmp(int a, int b)
{
    if (pri[a] != pri[b])
        return pri[a] < pri[b];
    return strcmp(name[a], name[b]) < 0;
}

void build(int n)
{
    for (int i = n; i < 2 * n; ++i)
        tree[i] = i - n;
    for (int i = 2 * n - 1; i > 0; --i)
        tree[i >> 1] = cmp(tree[i], tree[i ^ 1]) ? tree[i] : tree[i ^ 1];
}

void update(int i, int n)
{
    for (i += n; i > 0; i >>= 1)
        tree[i >> 1] = cmp(tree[i], tree[i ^ 1]) ? tree[i] : tree[i ^ 1];
}

int main()
{
    int n = io, m = io;
    for (int i = 0; i < n; ++i)
        pri[i] = io, ++io.p_in, io.read(name[i]);
    build(n);
    while (m--)
    {
        int top = tree[1];
        if (pri[top] > 0xffffffff)
            break;
        io.print(name[top]);
        pri[top] <<= 1;
        update(top, n);
    }
}