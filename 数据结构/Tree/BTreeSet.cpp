#include <cstring>
#include <cstdio>
#include <cassert>
#include <climits>
#include <algorithm>

template<typename It, typename T>
It lower_bound(It first, It last, const T &val)
{
    for (ptrdiff_t trip_count = (last - first) >> 2; trip_count > 0; --trip_count)
    {
        if (!(*first < val)) return first;
        ++first;
        if (!(*first < val)) return first;
        ++first;
        if (!(*first < val)) return first;
        ++first;
        if (!(*first < val)) return first;
        ++first;
    }
    switch (last - first)
    {
        case 3:
            if (!(*first < val)) return first;
            ++first;
        case 2:
            if (!(*first < val)) return first;
            ++first;
        case 1:
            if (!(*first < val)) return first;
            ++first;
        case 0:
        default:
            return last;
    }
}

template<typename It, typename T>
It upper_bound(It first, It last, const T &val)
{
    for (; first < last; ++first)
        if (val < *first)
            break;
    return first;
}

template<typename T>
void shift_by(T *ptr, int len, int shift)
{ memmove(ptr + shift, ptr, len * sizeof(T)); }

struct Set
{
    using pointer =  unsigned short;
    const static int BF = 64;
    const static pointer N = 60000;

    struct Node
    {
        int keys[2 * BF - 1];
        pointer child[2 * BF];
        pointer key_num, is_inner;

        void *operator new(size_t)
        { return buf + ++alloc; }
    };

    static Node buf[N];
    static pointer alloc;

    Node *root = new Node;
    int size = 0;

    // split x->child[where] into 2 children(still belong to x)
    // x->child[where]->key_num = 2 * BF - 1, and both 2 new children will have BF - 1
    // one extra key is lifted to x->keys[where]
    static void split(Node *x, int where)
    {
        Node *z = new Node;
        Node *y = buf + x->child[where];
        z->is_inner = y->is_inner;
        memcpy(z->keys, y->keys + BF, (BF - 1) * sizeof(int));
        if (y->is_inner)
            memcpy(z->child, y->child + BF, BF * sizeof(pointer));
        z->key_num = y->key_num = BF - 1;
        shift_by(x->child + where + 1, x->key_num - where, 1);
        x->child[where + 1] = z - buf;
        shift_by(x->keys + where, x->key_num - where, 1);
        x->keys[where] = y->keys[BF - 1];
        ++x->key_num;
    }

    // merge x->child[i] and x->child[i+1](put in x->child[i]), both key_num = BF - 1
    // after merge the key_num will be 2 * BF - 1
    // one missing key is from x->keys, it is put down to x->child[i]->keys
    static void merge(Node *x, int i)
    {
        Node *y = buf + x->child[i], *z = buf + x->child[i + 1];
        y->key_num = 2 * BF - 1;
        y->keys[BF - 1] = x->keys[i];
        memcpy(y->keys + BF, z->keys, (BF - 1) * sizeof(int));
        if (y->is_inner)
            memcpy(y->child + BF, z->child, BF * sizeof(pointer));
        shift_by(x->keys + i + 1, x->key_num - i - 1, -1);
        shift_by(x->child + i + 2, x->key_num - i - 1, -1);
        --x->key_num;
    }

    void ins_empty(Node *x, int key)
    {
        while (true)
        {
            int i = lower_bound(x->keys, x->keys + x->key_num, key) - x->keys;
            if (i != x->key_num && key == x->keys[i]) // duplicate ins
                return;
            if (!x->is_inner) // ins directly on a leaf
            {
                shift_by(x->keys + i, x->key_num - i, 1);
                x->keys[i] = key, ++x->key_num;
                ++size;
                return;
            }
            if (buf[x->child[i]].key_num == 2 * BF - 1) // split full node
            {
                split(x, i);
                // x->keys[i] was just lifted from its x->child[i], so it needs to be tested
                if (x->keys[i] < key)
                    ++i;
                else if (key == x->keys[i]) // duplicate ins
                    return;
            }
            x = buf + x->child[i];
        }
    }

    void ins(int key)
    {
        if (root->key_num == 2 * BF - 1) // if root is full, alloc a new node
        {
            Node *x = new Node;
            x->is_inner = 1, x->child[0] = root - buf;
            root = x;
            split(x, 0);
        }
        ins_empty(root, key);
    }

    bool exist(int key)
    {
        Node *x = root;
        while (x != buf)
        {
            int i = lower_bound(x->keys, x->keys + x->key_num, key) - x->keys;
            if (x->keys[i] == key)
                return true;
            x = buf + x->child[i];
        }
        return false;
    }

    void del(Node *x, int key)
    {
        int i = lower_bound(x->keys, x->keys + x->key_num, key) - x->keys;
        if (i != x->key_num && key == x->keys[i]) // found
        {
            if (!x->is_inner) // remove directly on a leaf
                shift_by(x->keys + i + 1, --x->key_num - i, -1), --size;
            else
            {
                if (buf[x->child[i]].key_num >= BF) // the child(of x) containing the prec of key has enough key
                {
                    Node *y = buf + x->child[i];
                    while (y->is_inner)
                        y = buf + y->child[y->key_num];
                    x->keys[i] = y->keys[y->key_num - 1];
                    del(buf + x->child[i], x->keys[i]);
                }
                else if (buf[x->child[i + 1]].key_num >=
                         BF) // the child(of x) containing the succ of key has enough key
                {
                    Node *y = buf + x->child[i + 1];
                    while (y->is_inner)
                        y = buf + y->child[0];
                    x->keys[i] = y->keys[0];
                    del(buf + x->child[i + 1], x->keys[i]);
                }
                else // neither has, then merge both node into y
                {
                    merge(x, i);
                    if (root->key_num == 0)
                        root = buf + x->child[i];
                    del(buf + x->child[i], key);
                }
            }
        }
        else if (x->is_inner) // not found, must guarantee x->child[i]->key_num >= BF before going into it
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
                    if (y->is_inner)
                    {
                        shift_by(y->child, y->key_num + 1, 1);
                        y->child[0] = z->child[z->key_num];
                    }
                    --z->key_num, ++y->key_num;
                    del(y, key);
                }
                else if (i < x->key_num && buf[x->child[i + 1]].key_num >= BF) // take the min child of right sibling
                {
                    Node *z = buf + x->child[i + 1];
                    y->keys[y->key_num] = x->keys[i];
                    x->keys[i] = z->keys[0];
                    if (y->is_inner)
                    {
                        y->child[y->key_num + 1] = z->child[0];
                        shift_by(z->child + 1, z->key_num, -1);
                    }
                    shift_by(z->keys + 1, z->key_num - 1, -1);
                    --z->key_num, ++y->key_num;
                    del(y, key);
                }
                else // neither sibling has enough key to lend, then merge it with left or right sibling
                {
                    if (i != 0)
                        --i;
                    y = buf + x->child[i];
                    merge(x, i);
                    if (root->key_num == 0)
                        root = y;
                    del(y, key);
                }
            }
        }
    }

    void del(int key)
    { del(root, key); }

    int min()
    {
        Node *x = root;
        while (x->is_inner)
            x = buf + x->child[0];
        return x->keys[0];
    }

    int max()
    {
        Node *x = root;
        while (x->is_inner)
            x = buf + x->child[x->key_num];
        return x->keys[x->key_num - 1];
    }

    int nxt(int key)
    {
        Node *x = root;
        int ret = INT_MAX;
        while (x != buf)
        {
            int i = upper_bound(x->keys, x->keys + x->key_num, key) - x->keys;
            if (i != x->key_num)
                ret = x->keys[i];
            x = buf + x->child[i];
        }
        return ret;
    }

    int pre(int key)
    {
        Node *x = root;
        int ret = INT_MIN;
        while (x != buf)
        {
            int i = lower_bound(x->keys, x->keys + x->key_num, key) - x->keys;
            if (i)
                ret = x->keys[i - 1];
            x = buf + x->child[i];
        }
        return ret;
    }
};

Set::Node Set::buf[N];
Set::pointer Set::alloc = 0;

#include <iostream>
#include <chrono>
#include <set>

using std::cout;
using namespace std::chrono;
const int N = 3e6;
int in[N];

int main()
{
    auto now = high_resolution_clock::now;
    for (auto &i : in)
        i = rand() % N;
    {
        auto beg = now();
        std::set<int> s;
        for (int i = 0; i < N; ++i)
            s.insert(in[i]);
        cout << s.size() << ' ';
        for (int i = 0; i < N; ++i)
            s.erase(in[i]);
        cout << s.size() << ' ';
        cout << duration<double>(now() - beg).count() << '\n';
    }
    {
        auto beg = now();
        Set s;
        for (int i = 0; i < N; ++i)
            s.ins(in[i]);
        cout << s.size << ' ';
        for (int i = 0; i < N; ++i)
            s.del(in[i]);
        cout << s.size << ' ';
        cout << duration<double>(now() - beg).count() << '\n';
    }
}
