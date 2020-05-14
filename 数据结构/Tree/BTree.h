#include <cstring>
#include <cstdio>
#include <cassert>

const int BF = 41;
const int N = 2505;
struct Node
{
    struct Pair
    {
        int key;
        int size;
    } keys[2 * BF - 1];
    int child[2 * BF];
    int key_num, size;
    int is_leaf = 1;
} buf[N];
int tsize;

template<typename It, typename T, typename Comp>
It lower_bound(It first, It last, const T &val, Comp comp)
{
    // loop invariant:
    // *(first - 1) is the biggest which < val (already found)
    // *last is the smallest which >= val (already found)
    while (first < last)
    {
        It mid = first + (last - first) / 2;
        if (comp(*mid, val))
            first = mid + 1;
        else
            last = mid;
    }
    return first;
}

bool cmp(const Node::Pair &key_pair, int key)
{
    return key_pair.key < key;
}

template<typename T>
void shift_by(T *ptr, int len, int shift)
{
    memmove(ptr + shift, ptr, len * sizeof(T));
}

Node *alloc()
{
    return buf + ++tsize;
}

Node *root = alloc();

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
    Node *z = alloc();
    Node *y = buf + x->child[where];
    z->is_leaf = y->is_leaf;
    memcpy(z->keys, y->keys + BF, (BF - 1) * sizeof(Node::Pair));
    if (!y->is_leaf)
        memcpy(z->child, y->child + BF, BF * sizeof(int));
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
        memcpy(y->child + BF, z->child, BF * sizeof(int));
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
        Node *x = alloc();
        x->is_leaf = 0, x->child[0] = root - buf, x->size = root->size;
        root = x;
        split(x, 0);
    }
    ins_empty(root, key);
}

int count(int key)
{
    Node *x = root;
    while (x != buf)
    {
        int i = lower_bound(x->keys, x->keys + x->key_num, key, cmp) - x->keys;
        if (x->keys[i].key == key)
            return x->keys[i].size;
        x = buf + x->child[i];
    }
    return 0;
}

void del(Node *x, int key, int num)
{
    x->size -= num;
    int i = lower_bound(x->keys, x->keys + x->key_num, key, cmp) - x->keys;
    if (i != x->key_num && key == x->keys[i].key) // found
    {
        if (x->keys[i].size > num) // more than 'num' key
            return void(x->keys[i].size -= num);
        if (x->is_leaf) // remove directly on a leaf
            shift_by(x->keys + i + 1, --x->key_num - i, -1);
        else
        {
            if (buf[x->child[i]].key_num >= BF) // the child(of x) containing the prec of key has enough key
            {
                Node *y = buf + x->child[i];
                while (!y->is_leaf)
                    y = buf + y->child[y->key_num];
                x->keys[i] = y->keys[y->key_num - 1];
                del(buf + x->child[i], x->keys[i].key, x->keys[i].size);
            }
            else if (buf[x->child[i + 1]].key_num >= BF) // the child(of x) containing the succ of key has enough key
            {
                Node *y = buf + x->child[i + 1];
                while (!y->is_leaf)
                    y = buf + y->child[0];
                x->keys[i] = y->keys[0];
                del(buf + x->child[i + 1], x->keys[i].key, x->keys[i].size);
            }
            else // neither has, then merge both node into y
            {
                merge(x, i);
                if (root->key_num == 0)
                    root = buf + x->child[i];
                del(buf + x->child[i], key, num);
            }
        }
    }
    else if (!x->is_leaf) // not found, must guarantee x->child[i]->key_num >= BF before going into it
    {
        if (buf[x->child[i]].key_num >= BF)
            del(buf + x->child[i], key, num);
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
                    y->child[0] = z->child[z->key_num];
                }
                --z->key_num, ++y->key_num;
                y->size = calc_size(y), z->size = calc_size(z);
                del(y, key, num);
            }
            else if (i < x->key_num && buf[x->child[i + 1]].key_num >= BF) // take the min child of right sibling
            {
                Node *z = buf + x->child[i + 1];
                y->keys[y->key_num] = x->keys[i];
                x->keys[i] = z->keys[0];
                if (!y->is_leaf)
                {
                    y->child[y->key_num + 1] = z->child[0];
                    shift_by(z->child + 1, z->key_num, -1);
                }
                shift_by(z->keys + 1, z->key_num - 1, -1);

                --z->key_num, ++y->key_num;
                y->size = calc_size(y), z->size = calc_size(z);
                del(y, key, num);
            }
            else // neither sibling has enough key to lend, then merge it with left or right sibling
            {
                if (i != 0)
                    --i;
                y = buf + x->child[i];
                merge(x, i);
                if (root->key_num == 0)
                    root = y;
                del(y, key, num);
            }
        }
    }
}

void del(int key)
{ del(root, key, 1); }

bool try_del(int key)
{
    if (!count(key))
        return false;
    return del(root, key, 1), true;
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

// note: kth is strictly O(log_(BF)(n) * BF)
// it may be the slowest operation
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

void check(Node *x, int lb = -1e9, int ub = 1e9)
{
    if (x == buf)
        return;
    assert(x->size == calc_size(x));
    assert(x->key_num <= 2 * BF - 1);
    if (x != root)
        assert(x->key_num >= BF - 1);
    for (int i = 0; i < x->key_num; ++i)
    {
        assert(x->keys[i].key < ub);
        assert(x->keys[i].key > lb);
        check(buf + x->child[i], i == 0 ? lb : x->keys[i - 1].key, x->keys[i].key);
    }
    check(buf + x->child[x->key_num], x->keys[x->key_num - 1].key, ub);
}