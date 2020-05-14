#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cctype>

const int N = 4000000 + 3200000 + 5;
const int S = 3200000 + 5;
char init[S];

struct Node
{
    int key, ch[2], sz, p, rev_tag;
} buf[N];
int treesz;
int root;

template<typename T>
void swap(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}

int alloc(int p, int key)
{
    int ret = ++treesz;
    buf[ret].key = key, buf[ret].sz = 1, buf[ret].p = p;
    return ret;
}

void update(int x)
{
    buf[x].sz = 1 + buf[buf[x].ch[0]].sz + buf[buf[x].ch[1]].sz;
}

void push(int x)
{
    if (buf[x].rev_tag)
    {
        swap(buf[x].ch[0], buf[x].ch[1]);
        buf[buf[x].ch[0]].rev_tag ^= 1;
        buf[buf[x].ch[1]].rev_tag ^= 1;
        buf[x].rev_tag = 0;
    }
}

void rot(int x)
{
    int p = buf[x].p, g = buf[p].p, l = x == buf[p].ch[0];
    buf[x].p = g;
    g ? buf[g].ch[p == buf[g].ch[1]] = x : root = x;
    buf[p].ch[l ^ 1] = buf[x].ch[l];
    buf[buf[x].ch[l]].p = p;
    buf[x].ch[l] = p;
    buf[p].p = x;
    update(p);
}

void splay(int x, int goal = 0)
{
    for (int p = buf[x].p, g = buf[p].p; buf[x].p != goal; p = buf[x].p, g = buf[p].p)
    {
        if (g != goal)
            (x == buf[p].ch[0]) == (p == buf[g].ch[0]) ? rot(p) : rot(x);
        rot(x);
    }
    update(x);
    if (!goal)
        root = x;
}

int kth(int k) // return pointer of kth
{
    int x = root;
    while (true)
    {
        push(x);
        if (k <= buf[buf[x].ch[0]].sz)
            x = buf[x].ch[0];
        else if (k <= buf[buf[x].ch[0]].sz + 1)
            return x;
        else
            k -= buf[buf[x].ch[0]].sz + 1, x = buf[x].ch[1];
    }
}

void build(int &rt, int l, int r)
{
    if (l == r)
        return buf[rt = ++treesz].key = init[l - 1], void(buf[rt].sz = 1);
    if (l > r)
        return;
    int mid = l + r >> 1;
    buf[rt = ++treesz].key = init[mid - 1];
    build(buf[rt].ch[0], l, mid - 1);
    build(buf[rt].ch[1], mid + 1, r);
    buf[buf[rt].ch[0]].p = buf[buf[rt].ch[1]].p = rt;
    update(rt);
}

int n;

void print(int rt)
{
    if (rt == 0)
        return;
    push(rt), push(buf[rt].ch[0]), push(buf[rt].ch[1]);
    print(buf[rt].ch[0]);
    if (buf[rt].key) // '\0' for placeholder
        putchar(buf[rt].key);
    print(buf[rt].ch[1]);
}

char next_char()
{
    char ch;
    while (isspace(ch = getchar_unlocked()));
    return ch;
}


int main()
{
    int t;
    scanf("%s%d", init + 1, &t);
    ::n = strlen(init + 1);
    build(root, 1, ::n + 2);
    char op, which, val;
    int lpos = 1, rpos = ::n + 1;
    while (t--)
    {
        op = next_char();
        switch (op)
        {
            case '<':
                which = next_char();
                if (which == 'L')
                    lpos == 1 ? puts("F") : (puts("T"), --lpos);
                else
                    rpos == 1 ? puts("F") : (puts("T"), --rpos);
                break;
            case '>':
                which = next_char();
                if (which == 'L')
                    lpos == ::n + 1 ? puts("F") : (puts("T"), ++lpos);
                else
                    rpos == ::n + 1 ? puts("F") : (puts("T"), ++rpos);
                break;
            case 'I':
                which = next_char(), val = next_char();
                puts("T");
                if (which == 'L')
                {
                    int l = kth(lpos);
                    splay(l);
                    int r = kth(lpos + 1);
                    splay(r, root);
                    buf[r].ch[0] = alloc(r, val);
                    ++buf[r].sz, ++buf[root].sz;
                    if (lpos <= rpos)
                        ++rpos;
                    ++lpos, ++n;
                }
                else
                {
                    int l = kth(rpos);
                    splay(l);
                    int r = kth(rpos + 1);
                    splay(r, root);
                    buf[r].ch[0] = alloc(r, val);
                    ++buf[r].sz, ++buf[root].sz;
                    if (rpos <= lpos)
                        ++lpos;
                    ++rpos, ++n;
                }
                break;
            case 'D':
                which = next_char();
                if (which == 'L')
                {
                    if (lpos == ::n + 1)
                        puts("F");
                    else
                    {
                        puts("T");
                        int l = kth(lpos);
                        splay(l);
                        int r = kth(lpos + 2);
                        splay(r, root);
                        buf[r].ch[0] = 0;
                        --buf[r].sz, --buf[root].sz;
                        if (rpos > lpos)
                            --rpos;
                        --n;
                    }
                }
                else
                {
                    if (rpos == ::n + 1)
                        puts("F");
                    else
                    {
                        puts("T");
                        int l = kth(rpos);
                        splay(l);
                        int r = kth(rpos + 2);
                        splay(r, root);
                        buf[buf[r].ch[0]].p = 0;
                        buf[r].ch[0] = 0;
                        --buf[r].sz, --buf[root].sz;
                        if (lpos > rpos)
                            --lpos;
                        --n;
                    }
                }
                break;
            case 'R':
                if (lpos >= rpos)
                    puts("F");
                else
                {
                    puts("T");
                    int l = kth(lpos);
                    splay(l);
                    int r = kth(rpos + 1);
                    splay(r, root);
                    buf[buf[r].ch[0]].rev_tag ^= 1;
                }
                break;
            case 'S':
                if (n)
                    for (int _ = 0; _ < 50; ++_)
                        splay(kth(rand() % n + 1));
                print(root), puts("");
                break;
        }
    }
}