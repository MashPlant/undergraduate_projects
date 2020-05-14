#define I __attribute__((always_inline)) inline
#ifndef MAX_NODE
#define MAX_NODE (2000000 + 5)
#endif
struct Node
{
    int key, ch[2], sz, cnt;
    int p;
} buf[MAX_NODE];
int treesz;
int root;
I int alloc(int p, int key)
{
    int ret = ++treesz;
    buf[ret].key = key, buf[ret].sz = buf[ret].cnt = 1, buf[ret].p = p;
    return ret;
}
I void update(int x) { buf[x].sz = buf[x].cnt + buf[buf[x].ch[0]].sz + buf[buf[x].ch[1]].sz; }
I void rot(int x)
{
    int p = buf[x].p, g = buf[p].p, l = x == buf[p].ch[0];
    buf[x].p = g;
    g ? buf[g].ch[p == buf[g].ch[1]] = x : root = x;
    buf[p].ch[l ^ 1] = buf[x].ch[l];
    buf[buf[x].ch[l]].p = p;
    buf[x].ch[l] = p;
    buf[p].p = x;
    update(p), update(x);
}
I void splay(int x, int goal = 0)
{
    for (int p = buf[x].p, g = buf[p].p; buf[x].p != goal; p = buf[x].p, g = buf[p].p)
    {
        if (g != goal)
            (x == buf[p].ch[0]) == (p == buf[g].ch[0]) ? rot(p) : rot(x);
        rot(x);
    }
    if (!goal)
        root = x;
}
I void ins(int key)
{
    int x = root, p = 0;
    while (x)
    {
        ++buf[p = x].sz;
        if (buf[x].key == key)
            return ++buf[x].cnt, splay(x);
        x = buf[x].ch[buf[x].key < key];
    }
    splay((p ? buf[p].ch[buf[p].key < key] : root) = alloc(p, key));
}
void find(int key)
{
    int x = root;
    while (true)
    {
        if (buf[x].key == key)
            return splay(x);
        x = buf[x].ch[buf[x].key < key];
    }
}
template <int D>
int _adj() // return pointer of the pre/nxt of root
{
    int x = buf[root].ch[D];
    while (buf[x].ch[D ^ 1])
        x = buf[x].ch[D ^ 1];
    return x;
}
void del(int key)
{
    find(key);
    --buf[root].sz, --buf[root].cnt;
    if (buf[root].cnt)
        return;
    if (!buf[root].ch[0])
        buf[root = buf[root].ch[1]].p = 0;
    else if (!buf[root].ch[1])
        buf[root = buf[root].ch[0]].p = 0;
    else
    {
        int pre = _adj<0>(), old = root;
        splay(pre); // now root is the r child of pre
        buf[buf[pre].ch[1] = buf[old].ch[1]].p = pre;
        update(pre);
    }
}
template <int D>
int adj(int key)
{
    ins(key);
    int ret = buf[_adj<D>()].key;
    del(key);
    return ret;
}
I int kth(int k)
{
    int x = root;
    while (true)
    {
        if (k <= buf[buf[x].ch[0]].sz)
            x = buf[x].ch[0];
        else if (k <= buf[buf[x].ch[0]].sz + buf[x].cnt)
            return splay(x), buf[root].key;
        else
            k -= buf[buf[x].ch[0]].sz + buf[x].cnt, x = buf[x].ch[1];
    }
}
I int lt(int key)
{
    ins(key);
    find(key);
    int ret = buf[buf[root].ch[0]].sz;
    del(key);
    return ret;
}