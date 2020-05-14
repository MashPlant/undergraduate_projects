#include <cstdio>
#include <cstring>
#include <cassert>
#include <sys/mman.h>
#include <sys/stat.h>

const int N = 500000 + 5;
using u64 = unsigned long long;

struct IO
{
    char *p_in, *p_out = out;
    char out[N * 8];
    unsigned table[10000];

    IO()
    {
        struct stat st;
        int fn = fileno(stdin);
        fstat(fn, &st);
        p_in = (char *) mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fn, 0);

        for (int i = 0; i < 10000; ++i)
        {
            unsigned v = i;
            char *o = (char *) (table + i);
            o[3] = v % 10 + '0';
            o[2] = (v % 100) / 10 + '0';
            o[1] = (v % 1000) / 100 + '0';
            o[0] = (v % 10000) / 1000;
            if (o[0])
                o[0] |= 0x30;
            else if (o[1] != '0')
                o[0] |= 0x20;
            else if (o[2] != '0')
                o[0] |= 0x10;
            else
                o[0] |= 0x00;
        }
    }

    ~IO()
    { fwrite(out, 1, p_out - out, stdout); }

    operator u64()
    {
        u64 v = 0;
        while (*p_in < '0')
            p_in++;
        do
            v = v * 10 + *p_in++ - '0';
        while (*p_in > ' ');
        return v;
    }

    void print(unsigned o)
    {
        unsigned blocks[3], *b = blocks + 2;
        blocks[0] = o < 0 ? ~o + 1 : o;
        blocks[2] = blocks[0] % 10000;
        blocks[0] /= 10000;
        blocks[2] = table[blocks[2]];
        if (blocks[0])
        {
            blocks[1] = blocks[0] % 10000;
            blocks[0] /= 10000;
            blocks[1] = table[blocks[1]];
            blocks[2] |= 0x30303030;
            b--;
        }
        if (blocks[0])
        {
            blocks[0] = table[blocks[0] % 10000];
            blocks[1] |= 0x30303030;
            b--;
        }
        char *f = (char *) b;
        f += 3 - (*f >> 4);
        memcpy(p_out, f, ((char *) blocks + 12) - f);
        p_out += ((char *) blocks + 12) - f;
        *p_out++ = '\n';
    }
} io;

template<typename T>
T min(T a, T b)
{ return a < b ? a : b; }

u64 a[N];
int ans[N];
namespace list
{
    int val[N], nxt[N], alloc;

    void push_back(int &head, int &tail, int val)
    {
        int n = ++alloc;
        list::val[n] = val;
        if (head == 0 /* == tail*/)
            head = tail = n;
        else
            nxt[tail] = n, tail = n;
    }

    void pop_front(int &head, int &tail)
    {
        if (head == tail)
            head = tail = 0;
        else
            head = nxt[head];
    }
}

namespace trie
{
    int ch[N * 25][4]; // for leaf node, index 0/1 used as list head/tail
    int alloc;

    void ins(u64 key, int val)
    {
        int x = 0;
        for (int i = 31; i >= 0; --i)
        {
            int b = key >> i * 2 & 3;
            x = ch[x][b] < 0 ? (ch[x][b] = ~ch[x][b]) : ch[x][b] ?: (ch[x][b] = ++alloc);
        }
        list::push_back(ch[x][0], ch[x][1], val);
    }

    void del(u64 key)
    {
        int path[32];
        int x = 0;
        for (int i = 31; i >= 0; --i)
            path[i] = x, x = ch[x][key >> i * 2 & 3];
        list::pop_front(ch[x][0], ch[x][1]);
        if (ch[x][0] == 0) // nothing left, recursive del upwards
        {
            for (int i = 0; i <= 31; ++i)
            {
                int b = key >> i * 2 & 3, *p = ch[path[i]];
                p[b] = ~p[b];
                if (p[0] > 0 || p[1] > 0 || p[2] > 0 || p[3] > 0)
                    break;
            }
        }
    }

    int query(u64 key)
    {
        int x = 0;
        for (int i = 31; i >= 0; --i)
        {
            int b = key >> i * 2 & 3;
            if (ch[x][b ^ 3] > 0) x = ch[x][b ^ 3];
            else if (ch[x][b ^ 2] > 0) x = ch[x][b ^ 2];
            else if (ch[x][b ^ 1] > 0) x = ch[x][b ^ 1];
            else x = ch[x][b];
        }
        return list::val[ch[x][0]];
    }
} // namespace trie

int main()
{
    int n = io, k = io;
    k = min(k + 1, n);
    for (int i = 0; i < n; ++i)
        a[i] = io;
    for (int i = 0; i < k; ++i)
        trie::ins(a[i], i);
    for (int i = 0; i < n; ++i)
    {
        if (i + k < n)
            trie::ins(a[i + k], i + k);
        ans[i] = trie::query(a[i]);
        if (i - k >= 0)
            trie::del(a[i - k]);
    }
    for (int i = 0; i < n; ++i)
        io.print(ans[i]);
}