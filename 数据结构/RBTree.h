#define I __attribute__((always_inline)) inline
#ifndef MAX_NODE
#define MAX_NODE (1000000 + 5)
#endif
#ifndef USE_STRUCT
#define USE_STRUCT 1
#endif
#ifndef SWAP
#define SWAP(a, b)     \
    ({                 \
        int __tmp = a; \
        a = b;         \
        b = __tmp;     \
    })
#endif

struct Node
{
    int key, ch[2], sz, cnt;
    int p : 31;
    int c : 1;
} buf[MAX_NODE];
int treesz;
I int alloc(int p, int key)
{
    int ret = ++treesz;
    buf[ret].key = key, buf[ret].sz = buf[ret].cnt = 1, buf[ret].p = p;
    buf[ret].c = 1;
    return ret;
}

#if USE_STRUCT
struct RBTree
{
#endif // USE_STRUCT
    int _root = 0;
    I void _trans(int t, int f) // f will replace t (in terms of t.p)
    {
        int p = buf[t].p;
        p ? buf[p].ch[buf[p].ch[1] == t] = f : _root = f;
        buf[f].p = p;
    }
    I void _rot(int x)
    {
        int p = buf[x].p, g = buf[p].p, l = x == buf[p].ch[0];
        buf[x].p = g;
        g ? buf[g].ch[p == buf[g].ch[1]] = x : _root = x;
        buf[p].ch[l ^ 1] = buf[x].ch[l];
        buf[buf[x].ch[l]].p = p;
        buf[x].ch[l] = p;
        buf[p].p = x;
        buf[x].sz = buf[p].sz;
        buf[p].sz = buf[p].cnt + buf[buf[p].ch[0]].sz + buf[buf[p].ch[1]].sz;
    }
    I void _ins_fix(int x)
    {
        while (buf[buf[x].p].c) // p is always red (so p is never _root)
        {
            int p = buf[x].p, g = buf[p].p, l = buf[g].ch[0] == p;
            int u = buf[g].ch[l];                             // u is uncle
            if (buf[u].c)                                     // case 1: x.p & x.uncle are both red
                buf[p].c = buf[u].c = 0, buf[g].c = 1, x = g; // case 1: make x.p & x.uncle black, lift z to g
            else
            {
                if ((buf[p].ch[0] == x) != l)          // case 2: x.uncle is black & x and x.p are not on the same side
                    _rot(x), SWAP(x, p), g = buf[p].p; // case 2: rot x and lift x to p, make x and x.p on the same side
                buf[p].c = 0, buf[g].c = 1, _rot(p);   // case 3: x.uncle is black & x and x.p are on the same side
            }
        }
        buf[_root].c = 0;
    }
    I void _del_fix(int x)
    {
        // first we give x a extra black, then the 'missing black count' (see in del) is fixed
        // when x is _root or x is red, we can make the 'extra black' a real black
        while (x != _root && !buf[x].c) // x is always a non-root node with double black
        {
            int p = buf[x].p, l = buf[p].ch[0] == x;
            int s = buf[p].ch[l];                                      // s is sibling
            if (buf[s].c)                                              // case 1: sibling is red
                buf[s].c = 0, buf[p].c = 1, _rot(s), s = buf[p].ch[l]; // (p is still x.p)
            if (!buf[buf[s].ch[0]].c && !buf[buf[s].ch[1]].c)          // case 2: sibling's both child are black
                buf[s].c = 1, x = p;
            else
            {
                if (!buf[buf[s].ch[l]].c)                                                                // case 3: sibling's child against x's side is black
                    buf[buf[s].ch[l ^ 1]].c = 0, buf[s].c = 1, _rot(buf[s].ch[l ^ 1]), s = buf[p].ch[l]; // (p is still x.p)
                buf[s].c = buf[p].c, buf[p].c = buf[buf[s].ch[l]].c = 0, _rot(s);                        // case 4: sibling's child against x's side is red
                x = _root;                                                                               // break loop, by the way make sure root is black
            }
        }
        buf[x].c = 0;
    }
    I void ins(int key)
    {
        int x = _root, p = 0;
        while (x)
        {
            ++buf[p = x].sz;
            if (buf[x].key == key)
                return void(++buf[x].cnt);
            x = buf[x].ch[buf[x].key < key];
        }
        _ins_fix((p ? buf[p].ch[buf[p].key < key] : _root) = alloc(p, key));
    }
    I void del(int key)
    {
        // y is (to be deleted) or (to be moved to replace x)
        // z is to fill y's place
        int x = _root, y, z, old_c;
        while (x)
        {
            --buf[x].sz;
            if (buf[x].key == key)
            {
                if (--buf[x].cnt)
                    return;
                break;
            }
            x = buf[x].ch[buf[x].key < key];
        }
        old_c = buf[y = x].c;
        if (!buf[x].ch[0])
            _trans(x, z = buf[x].ch[1]);
        else if (!buf[x].ch[1])
            _trans(x, z = buf[x].ch[0]);
        else
        {
            for (y = buf[x].ch[1]; buf[y].ch[0]; y = buf[y].ch[0]) // y is the succ of x
                ;
            for (int y_ = buf[y].p, cnt = buf[y].cnt; y_ != x; y_ = buf[y_].p)
                buf[y_].sz -= cnt;
            old_c = buf[y].c;
            z = buf[y].ch[1];
            if (buf[y].p == x) // if y is linked to x, y will replace x directly, no need to fill y's place
                buf[z].p = y;  // set z.p = y (z may be 0)
            else               // z will fill y's place
                _trans(y, z), buf[buf[y].ch[1] = buf[x].ch[1]].p = y;
            _trans(x, y), buf[buf[y].ch[0] = buf[x].ch[0]].p = y, buf[y].c = buf[x].c;
            buf[y].sz = buf[buf[y].ch[0]].sz + buf[buf[y].ch[1]].sz + buf[y].cnt;
        }
        if (!old_c) // if y is black, y's branch will miss a black count
            _del_fix(z);
    }
    I int kth(int k)
    {
        int x = _root;
        while (true)
        {
            if (k <= buf[buf[x].ch[0]].sz)
                x = buf[x].ch[0];
            else if (k <= buf[buf[x].ch[0]].sz + buf[x].cnt)
                return buf[x].key;
            else
                k -= buf[buf[x].ch[0]].sz + buf[x].cnt, x = buf[x].ch[1];
        }
    }
    I int lt(int key)
    {
        int x = _root, ret = 0;
        while (x)
        {
            if (buf[x].key < key)
                ret += buf[x].cnt + buf[buf[x].ch[0]].sz, x = buf[x].ch[1];
            else
                x = buf[x].ch[0];
        }
        return ret;
    }
    I int nxt(int key)
    {
        int x = _root, p = 0;
        while (x)
        {
            if (key < buf[x].key)
                p = x, x = buf[x].ch[0];
            else
                x = buf[x].ch[1];
        }
        return buf[p].key;
    }
    I int pre(int key)
    {
        int x = _root, p = 0;
        while (x)
        {
            if (buf[x].key < key)
                p = x, x = buf[x].ch[1];
            else
                x = buf[x].ch[0];
        }
        return buf[p].key;
    }
    template <int M>
    I int ext() // extreme
    {
        int x = _root;
        while (buf[x].ch[M])
            x = buf[x].ch[M];
        return buf[x].key;
    }
#if USE_STRUCT
};
#endif // USE_STRUCT