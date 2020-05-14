#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cctype>

const int N = 500000 + 5;
char init[N];

struct Node
{
    int key, ch[2], sz, p;
} buf[N * 2];
int treesz;
int root;

template<typename T>
T min(T a, T b)
{ return a < b ? a : b; }

template<typename T>
T max(T a, T b)
{ return a < b ? b : a; }

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

int kth(int k) // return pointer of kth from root_
{
    int x = root;
    while (true)
    {
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

void print(int rt)
{
    if (rt == 0)
        return;
    print(buf[rt].ch[0]);
    if (buf[rt].key) // '\0' for placeholder
        putchar(buf[rt].key);
    print(buf[rt].ch[1]);
}

void listlize(int rt, char *dst, int &n)
{
    if (rt == 0)
        return;
    listlize(buf[rt].ch[0], dst, n);
    if (buf[rt].key)
        dst[n++] = buf[rt].key;
    listlize(buf[rt].ch[1], dst, n);
}

void getline(char *s)
{
    char ch;
    while (!isspace(ch = getchar_unlocked()))
        *s++ = ch;
}

int main()
{
    int n, m, pos;
    char ch;
    getline(init + 1);
    scanf("%d", &m);
    n = strlen(init + 1);
    build(root, 1, n + 2);
    while (m--)
    {
        scanf("%d %c", &pos, &ch), pos += 2;
        splay(kth(pos - 1));
        int r = kth(pos);
        splay(r, root);
        buf[r].ch[0] = alloc(r, ch);
        ++buf[r].sz, ++buf[root].sz;
        int lb = max(1, pos - 3), ub = min(buf[root].sz, pos + 3);
        while (true)
        {
            splay(kth(lb));
            int r = kth(ub);
            splay(r, root);
            char arr[6]{}; // arr[5] will never be filled
            int cnt = 0; // assert(cnt <= 5)
            bool find = false;
            listlize(buf[r].ch[0], arr, cnt);
            for (int i = 0, j; i < cnt; ++i)
            {
                for (j = i + 1; arr[j] == arr[i]; ++j);
                if (j - i >= 3)
                {
                    int remain = cnt - (j - i);
                    int sub = buf[r].ch[0];
                    memmove(arr + i, arr + j, cnt - j);
                    switch (remain)
                    {
                        case 0:
                            buf[r].ch[0] = 0;
                            break;
                        case 1:
                            buf[sub].key = arr[0];
                            buf[sub].ch[0] = buf[sub].ch[1] = 0;
                            buf[sub].sz = 1;
                            break;
                        case 2:
                        {
                            buf[sub].sz = 2;
                            int sub1;
                            if (buf[sub].ch[0])
                            {
                                sub1 = buf[sub].ch[0];
                                buf[sub1].key = arr[0];
                                buf[sub].ch[1] = 0;
                                buf[sub].key = arr[1];
                            }
                            else
                            {
                                sub1 = buf[sub].ch[1];
                                buf[sub1].key = arr[1];
                                buf[sub].ch[0] = 0;
                                buf[sub].key = arr[0];
                            }
                            buf[sub1].sz = 1;
                            buf[sub1].ch[0] = buf[sub1].ch[1] = 0;
                        }
                            break;
                    }
                    update(r), update(root);
                    lb = max(1, lb - (2 - i));
                    ub = min(buf[root].sz, lb + 5);
                    find = true;
                    break;
                }
            }
            if (!find)
                break;
        }
    }
    print(root);
}