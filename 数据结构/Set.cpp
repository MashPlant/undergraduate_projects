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

template <typename K>
struct Set
{
    const static int N = 1000000 + 5;
    struct Node
    {
        K key;
        int p : 31, c : 1, ch[2];
    };
    static Node buf[N];
    static int treesz;
    static int alloc(int p, const K &key)
    {
        int ret = ++treesz;
        buf[ret].key = key, buf[ret].p = p, buf[ret].c = 1;
        return ret;
    }
    struct It
    {
        int pos;
        bool operator==(It rhs) const { return pos == rhs.pos; }
        bool operator!=(It rhs) const { return pos != rhs.pos; }
        K *operator->() const { return &buf[pos].key; }
        K &operator*() const { return buf[pos].key; }
        template <int D>
        void inc()
        {
            if (buf[pos].ch[D])
                for (pos = buf[pos].ch[D]; buf[pos].ch[D ^ 1]; pos = buf[pos].ch[D ^ 1])
                    ;
            else
            {
                for (; buf[buf[pos].p].ch[D] == pos; pos = buf[pos].p)
                    ;
                pos = buf[pos].p;
            }
        }
        It &operator++() { return inc<1>(), *this; }
        It operator++(int)
        {
            It ret = *this;
            return ++*this, ret;
        }
        It &operator--() { return inc<0>(), *this; }
        It operator--(int)
        {
            It ret = *this;
            return --*this, ret;
        }
    };
    int _root = 0, _size = 0;
    void _trans(int t, int f)
    {
        int p = buf[t].p;
        p ? buf[p].ch[buf[p].ch[1] == t] = f : _root = f;
        buf[f].p = p;
    }
    void _rot(int x)
    {
        int p = buf[x].p, g = buf[p].p, l = x == buf[p].ch[0];
        buf[x].p = g;
        g ? buf[g].ch[p == buf[g].ch[1]] = x : _root = x;
        buf[p].ch[l ^ 1] = buf[x].ch[l];
        buf[buf[x].ch[l]].p = p;
        buf[x].ch[l] = p;
        buf[p].p = x;
    }
    void _ins_fix(int x)
    {
        while (buf[buf[x].p].c)
        {
            int p = buf[x].p, g = buf[p].p, l = buf[g].ch[0] == p;
            int u = buf[g].ch[l];
            if (buf[u].c)
                buf[p].c = buf[u].c = 0, buf[g].c = 1, x = g;
            else
            {
                if ((buf[p].ch[0] == x) != l)
                    _rot(x), SWAP(x, p), g = buf[p].p;
                buf[p].c = 0, buf[g].c = 1, _rot(p);
            }
        }
        buf[_root].c = 0;
    }
    void _del_fix(int x)
    {
        while (x != _root && !buf[x].c)
        {
            int p = buf[x].p, l = buf[p].ch[0] == x;
            int s = buf[p].ch[l];
            if (buf[s].c)
                buf[s].c = 0, buf[p].c = 1, _rot(s), s = buf[p].ch[l];
            if (!buf[buf[s].ch[0]].c && !buf[buf[s].ch[1]].c)
                buf[s].c = 1, x = p;
            else
            {
                if (!buf[buf[s].ch[l]].c)
                    buf[buf[s].ch[l ^ 1]].c = 0, buf[s].c = 1, _rot(buf[s].ch[l ^ 1]), s = buf[p].ch[l];
                buf[s].c = buf[p].c, buf[p].c = buf[buf[s].ch[l]].c = 0, _rot(s);
                x = _root;
            }
        }
        buf[x].c = 0;
    }
    int size() const { return _size; }
    It find(const K &key)
    {
        int x = _root;
        while (x)
        {
            if (buf[x].key == key)
                return {x};
            x = buf[x].ch[buf[x].key < key];
        }
        return {0};
    }
    void ins(const K &key)
    {
        int x = _root, p = 0;
        while (x)
        {
            p = x;
            if (buf[x].key == key)
                return;
            x = buf[x].ch[buf[x].key < key];
        }
        ++_size;
        _ins_fix((p ? buf[p].ch[buf[p].key < key] : _root) = alloc(p, key));
    }
    void del(const K &key)
    {
        It it = find(key);
        if (it.pos)
            del(it);
    }
    void del(It it)
    {
        --_size;
        int x = it.pos, y = x, z, old_c = buf[y].c;
        if (!buf[x].ch[0])
            _trans(x, z = buf[x].ch[1]);
        else if (!buf[x].ch[1])
            _trans(x, z = buf[x].ch[0]);
        else
        {
            for (y = buf[x].ch[1]; buf[y].ch[0]; y = buf[y].ch[0])
                ;
            old_c = buf[y].c;
            z = buf[y].ch[1];
            if (buf[y].p == x)
                buf[z].p = y;
            else
                _trans(y, z), buf[buf[y].ch[1] = buf[x].ch[1]].p = y;
            _trans(x, y), buf[buf[y].ch[0] = buf[x].ch[0]].p = y, buf[y].c = buf[x].c;
        }
        if (!old_c)
            _del_fix(z);
    }
    It begin() const
    {
        int x = _root;
        while (buf[x].ch[0])
            x = buf[x].ch[0];
        return {x};
    }
    It end() const { return {0}; }
};
template <typename K>
int Set<K>::treesz = 0;
template <typename K>
typename Set<K>::Node Set<K>::buf[Set<K>::N];

#include <cstdio>
#include <random>
#include <iostream>
#include <algorithm>
#include <vector>
using std::cout;

static double sys_time()
{
    return static_cast<double>(clock()) / CLOCKS_PER_SEC;
}

int main()
{
    Set<int> s;

    const int num = 1000000;
    std::vector<int> elements(num), search_elements(num);
    for (int i = 0; i < num; ++i)
        elements[i] = search_elements[i] = i;
    std::random_shuffle(elements.begin(), elements.end());
    std::random_shuffle(search_elements.begin(), search_elements.end());

    double start_insert = sys_time();
    for (int j = 0; j < num; ++j)
        s.ins(elements[j]);
    cout << "-- Inserted " << num << " elements in " << (sys_time() - start_insert) << " secs" << '\n';

    double start_search = sys_time();
    int num_found = 0;
    for (int j = 0; j < num; ++j)
    {
        if (s.find(search_elements[j]) != s.end())
            ++num_found;
    }
    cout << "-- Found " << num_found << " elements in " << (sys_time() - start_search) << " secs" << '\n';

    double start_erase = sys_time();
    int num_erased = 0;
    for (int j = 0; j < num; ++j)
    {
        s.del(search_elements[j]);
        ++num_erased;
    }
    cout << "-- Erased " << num_found << " elements in " << (sys_time() - start_erase) << " secs" << '\n';
}