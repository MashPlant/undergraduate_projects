#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cctype>

template <typename T>
void shift_by(T *ptr, int len, int shift)
{
    memmove(ptr + shift, ptr, len * sizeof(T));
}

template <typename T>
struct Seq
{
    using pointer = unsigned short;
    const static int BF = 30;
    const static pointer N = 35000;

    struct Node
    {
        T keys[2 * BF - 1];
        pointer child[2 * BF];
        pointer key_num, is_inner;
        int sizes[2 * BF];

        void *operator new(size_t)
        {
            static pointer alloc = 0;
            return buf + ++alloc;
        }

        int get_size()
        {
            int ret = key_num;
            for (int i = 0; i <= key_num; ++i)
                ret += sizes[i];
            return ret;
        }

        void split(int i)
        {
            Node *z = new Node;
            Node *y = buf + child[i];
            z->is_inner = y->is_inner;
            memcpy(z->keys, y->keys + BF, (BF - 1) * sizeof(T));
            if (y->is_inner)
            {
                memcpy(z->child, y->child + BF, BF * sizeof(pointer));
                memcpy(z->sizes, y->sizes + BF, BF * sizeof(int));
            }
            z->key_num = y->key_num = BF - 1;
            shift_by(child + i + 1, key_num - i, 1);
            shift_by(sizes + i + 1, key_num - i, 1);
            sizes[i] = y->get_size();
            child[i + 1] = z - buf, sizes[i + 1] = z->get_size();
            shift_by(keys + i, key_num - i, 1);
            keys[i] = y->keys[BF - 1];
            ++key_num;
        }

        void merge(int i)
        {
            Node *y = buf + child[i], *z = buf + child[i + 1];
            y->key_num = 2 * BF - 1;
            y->keys[BF - 1] = keys[i];
            memcpy(y->keys + BF, z->keys, (BF - 1) * sizeof(T));
            if (y->is_inner)
            {
                memcpy(y->child + BF, z->child, BF * sizeof(pointer));
                memcpy(y->sizes + BF, z->sizes, BF * sizeof(int));
            }
            shift_by(keys + i + 1, key_num - i - 1, -1);
            shift_by(child + i + 2, key_num - i - 1, -1);
            sizes[i] += sizes[i + 1] + 1;
            shift_by(sizes + i + 2, key_num - i - 1, -1);
            --key_num;
        }

        // find the first i that the number of elements before keys[i] >= k
        int lower_bound(int &k)
        {
            if (!is_inner)
            {
                int ret = k - 1;
                k = 1;
                return ret;
            }
            int i = 0;
            for (int trip_count = (key_num + 1) >> 2; trip_count > 0; --trip_count)
            {
                if (k <= sizes[i] + 1)
                    return i;
                k -= sizes[i++] + 1;
                if (k <= sizes[i] + 1)
                    return i;
                k -= sizes[i++] + 1;
                if (k <= sizes[i] + 1)
                    return i;
                k -= sizes[i++] + 1;
                if (k <= sizes[i] + 1)
                    return i;
                k -= sizes[i++] + 1;
            }
            switch (key_num + 1 - i)
            {
            case 3:
                if (k <= sizes[i] + 1)
                    return i;
                k -= sizes[i++] + 1;
            case 2:
                if (k <= sizes[i] + 1)
                    return i;
                k -= sizes[i++] + 1;
            case 1:
                if (k <= sizes[i] + 1)
                    return i;
                k -= sizes[i++] + 1;
            case 0:
            default:
                return key_num + 1;
            }
        }
    } *root = new Node;

    static Node buf[N];

    int size() const
    {
        return root->get_size();
    }

    // [1, size] indexed
    T &operator[](int index)
    {
        Node *x = root;
        while (true)
        {
            int i = x->lower_bound(index);
            if (index == x->sizes[i] + 1)
                return x->keys[i];
            x = buf + x->child[i];
        }
    }

    void ins(int index, T key)
    {
        if (root->key_num == 2 * BF - 1) // if root is full, alloc a new node
        {
            Node *x = new Node;
            x->is_inner = 1, x->child[0] = root - buf, x->sizes[0] = root->get_size();
            root = x;
            x->split(0);
        }
        Node *x = root;
        while (true)
        {
            int i = x->lower_bound(index);
            if (!x->is_inner) // ins directly on a leaf
            {
                shift_by(x->keys + i, x->key_num - i, 1);
                x->keys[i] = key;
                ++x->key_num;
                return;
            }
            if (buf[x->child[i]].key_num == 2 * BF - 1) // split full node
            {
                x->split(i);
                if (x->sizes[i] + 1 < index)
                    index -= x->sizes[i++] + 1;
            }
            ++x->sizes[i];
            x = buf + x->child[i];
        }
    }

    // specially optimized for ins(size() + 1, key)
    void push_back(T key)
    {
        if (root->key_num == 2 * BF - 1)
        {
            Node *x = new Node;
            x->is_inner = 1, x->child[0] = root - buf, x->sizes[0] = root->get_size();
            root = x;
            x->split(0);
        }
        Node *x = root;
        while (true)
        {
            int i = x->key_num;
            if (!x->is_inner)
                return void(x->keys[x->key_num++] = key);
            if (buf[x->child[i]].key_num == 2 * BF - 1)
                x->split(i++);
            ++x->sizes[i];
            x = buf + x->child[i];
        }
    }

    void del(Node *x, int index)
    {
        int i = x->lower_bound(index);
        if (index == x->sizes[i] + 1) // found
        {
            if (!x->is_inner) // remove directly on a leaf
                shift_by(x->keys + i + 1, --x->key_num - i, -1);
            else
            {
                if (buf[x->child[i]].key_num >= BF) // the child(of x) containing the prec of key has enough key
                {
                    Node *y = buf + x->child[i];
                    while (y->is_inner)
                        y = buf + y->child[y->key_num];
                    x->keys[i] = y->keys[y->key_num - 1];

                    del(buf + x->child[i], x->sizes[i]--);
                }
                else if (buf[x->child[i + 1]].key_num >=
                         BF) // the child(of x) containing the succ of key has enough key
                {
                    Node *y = buf + x->child[i + 1];
                    while (y->is_inner)
                        y = buf + y->child[0];
                    x->keys[i] = y->keys[0];

                    --x->sizes[i + 1];
                    del(buf + x->child[i + 1], 1);
                }
                else // neither has, then merge both node into y
                {
                    x->merge(i);
                    if (root->key_num == 0)
                        root = buf + x->child[i];

                    --x->sizes[i];
                    del(buf + x->child[i], index);
                }
            }
        }
        else if (x->is_inner) // not found, must guarantee x->child[i]->key_num >= BF before going into it
        {
            if (buf[x->child[i]].key_num >= BF)
                --x->sizes[i], del(buf + x->child[i], index);
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
                        shift_by(y->sizes, y->key_num + 1, 1);
                        y->child[0] = z->child[z->key_num];
                        y->sizes[0] = z->sizes[z->key_num];
                        x->sizes[i] += y->sizes[0], x->sizes[i - 1] -= y->sizes[0];
                    }
                    --z->key_num, ++y->key_num;
                    --x->sizes[i - 1], ++x->sizes[i];

                    --x->sizes[i];
                    del(y, index + y->sizes[0] + 1);
                }
                else if (i < x->key_num && buf[x->child[i + 1]].key_num >= BF) // take the min child of right sibling
                {
                    Node *z = buf + x->child[i + 1];
                    y->keys[y->key_num] = x->keys[i];
                    x->keys[i] = z->keys[0];
                    if (y->is_inner)
                    {
                        y->child[y->key_num + 1] = z->child[0];
                        y->sizes[y->key_num + 1] = z->sizes[0];
                        x->sizes[i] += z->sizes[0], x->sizes[i + 1] -= z->sizes[0];
                        shift_by(z->child + 1, z->key_num, -1);
                        shift_by(z->sizes + 1, z->key_num, -1);
                    }
                    shift_by(z->keys + 1, z->key_num - 1, -1);
                    --z->key_num, ++y->key_num;
                    --x->sizes[i + 1], ++x->sizes[i];

                    --x->sizes[i];
                    del(y, index);
                }
                else // neither sibling has enough key to lend, then merge it with left or right sibling
                {
                    if (i != 0)
                        index += x->sizes[--i] + 1;
                    y = buf + x->child[i];
                    x->merge(i);
                    if (root->key_num == 0)
                        root = y;

                    --x->sizes[i];
                    del(y, index);
                }
            }
        }
    }

    void check(Node *x, int p_size = -1)
    {
        if (x == buf)
            return;
        if (p_size != -1)
            assert(x->get_size() == p_size);
        assert(x->key_num <= 2 * BF - 1);
        if (x != root)
            assert(x->key_num >= BF - 1);
        for (int i = 0; i <= x->key_num; ++i)
            check(buf + x->child[i], x->sizes[i]);
    }

    void del(int index)
    {
        del(root, index);
    }

    template <typename F>
    void foreach (Node *x, F f)
    {
        if (!x->is_inner)
            for (int i = 0; i < x->key_num; ++i)
                f(x->keys[i]);
        else
        {
            for (int i = 0; i < x->key_num; ++i)
                foreach (buf + x->child[i], f)
                    , f(x->keys[i]);
            foreach (buf + x->child[x->key_num], f)
                ;
        }
    }

    template <typename F>
    void foreach (F f)
    {
        foreach (root, f)
            ;
    }
};

template <typename T>
typename Seq<T>::Node Seq<T>::buf[N];

template <typename T>
T min(T a, T b)
{
    return a < b ? a : b;
}

template <typename T>
T max(T a, T b)
{
    return a < b ? b : a;
}

template <typename T>
void swap(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}

struct IO
{
    char *p_in = in, *p_out = out;
    char in[int(2e7 + 5)], out[int(1e6 + 5)];

    IO()
    {
        fread(in, 1, sizeof in, stdin);
    }

    ~IO()
    {
        fwrite(out, 1, p_out - out, stdout);
    }

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

const int N = 500000 + 5;

int main()
{
    Seq<char> s;
    while (!isspace(*io.p_in))
        s.push_back(*io.p_in++);
    int m = io;
    while (m--)
    {
        int pos = io + 1;
        ++io.p_in;
        char ch = *io.p_in++;
        s.ins(pos, ch);
        int lb = max(1, pos - 2), ub = min(s.size(), pos + 2);
        while (true)
        {
            char arr[6]{};
            int cnt = 0;
            bool find = false;
            for (int i = lb; i <= ub; ++i)
                arr[cnt++] = s[i];
            for (int i = 0, j; i < cnt; ++i)
            {
                for (j = i + 1; arr[j] == arr[i]; ++j)
                    ;
                if (j - i >= 3)
                {
                    for (int _ = i; _ < j; ++_)
                        s.del(lb + i);
                    lb = max(1, lb - (2 - i));
                    ub = min(s.size(), lb + 3);
                    find = true;
                    break;
                }
            }
            if (!find)
                break;
        }
    }
    s.foreach ([](char ch) { *io.p_out++ = ch; });
}