#include <cstdio>
#include <cstdlib>
#include <cstring>
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

const static int BF = 40;
const int M = 25005;
const int N = 1000000 + 5;
struct Node
{
    struct Pair
    {
        int key;
        int size;
    } keys[2 * BF - 1];
    int child[2 * BF];
    int key_num, size;
    int p, is_leaf = 1;
} buf[M];
int tsize;

template <typename It, typename T, typename Comp>
It lower_bound(It first, It last, const T &val, Comp comp)
{
    --last;
    while (first <= last)
    {
        It mid = first + (last - first) / 2;
        if (comp(*mid, val))
            first = mid + 1;
        else
            last = mid - 1;
    }
    return first;
}

bool cmp(const Node::Pair &key_pair, int key)
{
    return key_pair.key < key;
}

template <typename T>
void shift_by(T *ptr, int len, int shift)
{
    memmove(ptr + shift, ptr, len * sizeof(T));
}

Node *alloc(int p)
{
    int ret = ++tsize;
    buf[ret].p = p;
    return buf + ret;
}

Node *root = alloc(0);

int calc_size(Node *x)
{
    int size = 0;
    for (int i = 0; i < x->key_num; ++i)
        size += buf[x->child[i]].size + x->keys[i].size;
    size += buf[x->child[x->key_num]].size;
    return size;
}

// split x->child[where] into 2 children(still belong to x)
// x->child[where]->key_num = 2 * BF - 1, and both 2 new children will have BF - 1
// one extra key is lifted to x->keys[where]
void split(Node *x, int where)
{
    Node *z = alloc(x - buf);
    Node *y = buf + x->child[where];
    z->is_leaf = y->is_leaf;
    memcpy(z->keys, y->keys + BF, (BF - 1) * sizeof(Node::Pair));
    if (!y->is_leaf)
    {
        memcpy(z->child, y->child + BF, BF * sizeof(int));
        for (int i = 0; i < BF; ++i)
            buf[z->child[i]].p = z - buf;
    }
    z->key_num = y->key_num = BF - 1;
    shift_by(x->child + where + 1, x->key_num - where, 1);
    x->child[where + 1] = z - buf;
    shift_by(x->keys + where, x->key_num - where, 1);
    x->keys[where] = y->keys[BF - 1];
    y->size = calc_size(y), z->size = calc_size(z);
    ++x->key_num;
}

// merge x->child[i] and x->child[i+1](put in x->child[i]), both key_num = BF - 1
// after merge the key_num will be 2 * BF - 1
// one missing key is from x->keys, it is put down to x->child[i]->keys
void merge(Node *x, int i)
{
    Node *y = buf + x->child[i], *z = buf + x->child[i + 1];
    y->key_num = 2 * BF - 1;
    y->keys[BF - 1] = x->keys[i];
    memcpy(y->keys + BF, z->keys, (BF - 1) * sizeof(Node::Pair));
    if (!y->is_leaf)
    {
        memcpy(y->child + BF, z->child, BF * sizeof(int));
        for (int j = BF; j <= 2 * BF - 1; ++j)
            buf[y->child[j]].p = y - buf;
    }
    shift_by(x->keys + i + 1, x->key_num - i - 1, -1);
    shift_by(x->child + i + 2, x->key_num - i - 1, -1);
    --x->key_num;
    y->size = calc_size(y);
}

void ins_empty(Node *x, int key)
{
    while (true)
    {
        ++x->size;
        int i = lower_bound(x->keys, x->keys + x->key_num, key, cmp) - x->keys;
        if (i != x->key_num && key == x->keys[i].key) // duplicate ins
            return void(++x->keys[i].size);
        if (x->is_leaf) // ins directly on a leaf
        {
            shift_by(x->keys + i, x->key_num - i, 1);
            x->keys[i] = {key, 1};
            ++x->key_num;
            return;
        }
        if (buf[x->child[i]].key_num == 2 * BF - 1) // split full node
        {
            split(x, i);
            // x->keys[i] was just lifted from its x->child[i], so it needs to be tested
            if (x->keys[i].key < key)
                ++i;
            else if (key == x->keys[i].key) // duplicate ins
                return void(++x->keys[i].size);
        }
        x = buf + x->child[i];
    }
}

void ins(int key)
{
    if (root->key_num == 2 * BF - 1) // if root is full, alloc a new node
    {
        Node *x = alloc(0);
        x->is_leaf = 0, x->child[0] = root - buf, x->size = root->size;
        root->p = x - buf, root = x;
        split(x, 0);
    }
    ins_empty(root, key);
}

void del(Node *x, int key)
{
    int i = lower_bound(x->keys, x->keys + x->key_num, key, cmp) - x->keys;
    if (i != x->key_num && key == x->keys[i].key) // found
    {
        if (x->keys[i].size > 1) // more than one key
        {
            --x->keys[i].size;
            while (x != buf)
                --x->size, x = buf + x->p;
            return;
        }
        if (x->is_leaf) // remove directly on a leaf
        {
            shift_by(x->keys + i + 1, --x->key_num - i, -1);
            while (x != buf)
                --x->size, x = buf + x->p;
        }
        else
        {
            if (buf[x->child[i]].key_num >= BF) // the child(of x) containing the prec of key has enough key
            {
                Node *y = buf + x->child[i];
                while (!y->is_leaf)
                    y = buf + y->child[y->key_num];
                x->keys[i] = y->keys[y->key_num - 1];
                if (x->keys[i].size != 1)
                {
                    y->keys[y->key_num - 1].size = 1;
                    while (y != x)
                        y->size -= x->keys[i].size - 1, y = buf + y->p;
                }
                del(buf + x->child[i], x->keys[i].key);
            }
            else if (buf[x->child[i + 1]].key_num >= BF) // the child(of x) containing the succ of key has enough key
            {
                Node *y = buf + x->child[i + 1];
                while (!y->is_leaf)
                    y = buf + y->child[0];
                x->keys[i] = y->keys[0];
                if (x->keys[i].size != 1)
                {
                    y->keys[0].size = 1;
                    while (y != x)
                        y->size -= x->keys[i].size - 1, y = buf + y->p;
                }
                del(buf + x->child[i + 1], x->keys[i].key);
            }
            else // neither has, then merge both node into y
            {
                merge(x, i);
                if (root->key_num == 0)
                    root = buf + x->child[i], root->p = 0;
                del(buf + x->child[i], key);
            }
        }
    }
    else if (!x->is_leaf) // not found, must guarantee x->child[i]->key_num >= BF before going into it
    {
        if (buf[x->child[i]].key_num >= BF)
            del(buf + x->child[i], key);
        else
        {
            Node *y = buf + x->child[i];
            if (i >= 1 && buf[x->child[i - 1]].key_num >= BF) // take the max child of left sibling
            {
                Node *z = buf + x->child[i - 1];
                shift_by(y->keys, y->key_num, 1);
                y->keys[0] = x->keys[i - 1];
                x->keys[i - 1] = z->keys[z->key_num - 1];
                if (!y->is_leaf)
                {
                    shift_by(y->child, y->key_num + 1, 1);
                    y->child[0] = z->child[z->key_num], buf[y->child[0]].p = y - buf;
                }
                --z->key_num, ++y->key_num;
                y->size = calc_size(y), z->size = calc_size(z);
                del(y, key);
            }
            else if (i < x->key_num && buf[x->child[i + 1]].key_num >= BF) // take the min child of right sibling
            {
                Node *z = buf + x->child[i + 1];
                y->keys[y->key_num] = x->keys[i];
                x->keys[i] = z->keys[0];
                if (!y->is_leaf)
                {
                    y->child[y->key_num + 1] = z->child[0], buf[y->child[y->key_num + 1]].p = y - buf;
                    shift_by(z->child + 1, z->key_num, -1);
                }
                shift_by(z->keys + 1, z->key_num - 1, -1);

                --z->key_num, ++y->key_num;
                y->size = calc_size(y), z->size = calc_size(z);
                del(y, key);
            }
            else // neither sibling has enough key to lend, then merge it with left or right sibling
            {
                if (i != 0)
                    --i;
                y = buf + x->child[i];
                merge(x, i);
                if (root->key_num == 0)
                    root = y, root->p = 0;
                del(y, key);
            }
        }
    }
}

void del(int key)
{
    del(root, key);
}

int nxt(int key)
{
    Node *x = root;
    int ret;
    while (x != buf)
    {
        int i = lower_bound(x->keys, x->keys + x->key_num, key, cmp) - x->keys;
        if (x->keys[i].key == key)
            ++i;
        if (i != x->key_num)
            ret = x->keys[i].key;
        x = buf + x->child[i];
    }
    return ret;
}

int pre(int key)
{
    Node *x = root;
    int ret;
    while (x != buf)
    {
        int i = lower_bound(x->keys, x->keys + x->key_num, key, cmp) - x->keys;
        if (i)
            ret = x->keys[i - 1].key;
        x = buf + x->child[i];
    }
    return ret;
}

int lt(int key)
{
    Node *x = root;
    int ret = 0;
    while (x != buf)
    {
        if (x->keys[x->key_num - 1].key < key)
        {
            ret += x->size - buf[x->child[x->key_num]].size;
            x = buf + x->child[x->key_num];
            continue;
        }
        for (int i = 0; i < x->key_num; ++i)
        {
            if (x->keys[i].key < key)
                ret += buf[x->child[i]].size + x->keys[i].size;
            else if (x->keys[i].key == key)
                return ret + buf[x->child[i]].size;
            else
            {
                x = buf + x->child[i];
                break;
            }
        }
    }
    return ret;
}

int kth(int k)
{
    Node *x = root;
    while (true)
    {
        for (int i = 0; i <= x->key_num; ++i)
        {
            int lb = buf[x->child[i]].size + 1, ub = buf[x->child[i]].size + (i == x->key_num ? 1 : x->keys[i].size);
            if (k >= lb && k <= ub)
                return x->keys[i].key;
            if (k < lb)
            {
                x = buf + x->child[i];
                break;
            }
            k -= ub;
        }
    }
}

int min()
{
    Node *x = root;
    while (!x->is_leaf)
        x = buf + x->child[0];
    return x->keys[0].key;
}

int max()
{
    Node *x = root;
    while (!x->is_leaf)
        x = buf + x->child[x->key_num];
    return x->keys[x->key_num - 1].key;
}

int que[N];
int main()
{
    int n = io * 2, beg = 0, end = 0;
    int cur = 0;
    long long ans = 0;
    while (n--)
    {
        int d = io;
        ans += (long long)d * cur;
        if (*io.p_in == ' ')
            ins(que[end++] = io);
        else
            del(que[beg++]);
        cur = max();
    }
    printf("%lld\n", ans);
}