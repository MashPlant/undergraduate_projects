#include <cstdlib>
#include <cstdio>
#include <cstring>

template<typename T>
void swap(T &a, T &b)
{
    T c = a;
    a = b;
    b = c;
}

template<typename T>
struct Seq
{
    const static int N = 1000000 + 5;

    struct Node
    {
        Node *ch[2], *p;
        int sz;
        bool c;
        T key;
    };

    static Node buf[N];
    using pointer = Node *;

    struct Iterator
    {
        pointer p;

        T &operator*()
        { return p->key; }

        template<int D>
        void inc()
        {
            if (p->ch[D] != buf)
                for (p = p->ch[D]; p->ch[D ^ 1] != buf; p = p->ch[D ^ 1]);
            else
            {
                for (; p->p->ch[D] == p; p = p->p);
                p = p->p;
            }
        }

        Iterator &operator++()
        { return inc<1>(), *this; }

        Iterator operator++(int)
        {
            Iterator ret = *this;
            ++*this;
            return ret;
        }

        // --begin() == end()
        Iterator &operator--()
        { return inc<0>(), *this; }

        Iterator operator--(int)
        {
            Iterator ret = *this;
            --*this;
            return ret;
        }

        bool operator==(Iterator rhs) const
        { return p == rhs.p; }

        bool operator!=(Iterator rhs) const
        { return p != rhs.p; }
    };

    using iterator = Iterator;

    static pointer alloc(pointer p, T key)
    {
        static pointer _alloc = buf;
        pointer ret = ++_alloc;
        ret->ch[0] = ret->ch[1] = buf, ret->p = p;
        ret->sz = 1, ret->c = 1, ret->key = key;
        return ret;
    }

    Node *_root = buf;

    void _trans(pointer t, pointer f) // f will replace t (in terms of t.p)
    {
        pointer p = t->p;
        (p != buf) ? p->ch[p->ch[1] == t] = f : _root = f;
        f->p = p;
    }

    void _rot(pointer x)
    {
        pointer p = x->p, g = p->p;
        int l = x == p->ch[0];
        x->p = g;
        (g != buf) ? g->ch[p == g->ch[1]] = x : _root = x;
        p->ch[l ^ 1] = x->ch[l];
        x->ch[l]->p = p;
        x->ch[l] = p;
        p->p = x;
        x->sz = p->sz;
        p->sz = 1 + p->ch[0]->sz + p->ch[1]->sz;
    }

    void _ins_fix(pointer x)
    {
        while (x->p->c) // p is always red (so p is never _root)
        {
            pointer p = x->p, g = p->p;
            int l = g->ch[0] == p;
            pointer u = g->ch[l];                             // u is uncle
            if (u->c)                                     // case 1: x.p & x.uncle are both red
                p->c = u->c = 0, g->c = 1, x = g; // case 1: make x.p & x.uncle black, lift z to g
            else
            {
                if ((p->ch[0] == x) != l)          // case 2: x.uncle is black & x and x.p are not on the same side
                    _rot(x), swap(x, p), g = p->p; // case 2: rot x and lift x to p, make x and x.p on the same side
                p->c = 0, g->c = 1, _rot(p);   // case 3: x.uncle is black & x and x.p are on the same side
            }
        }
        _root->c = 0;
    }

    void _del_fix(pointer x)
    {
        // first we give x a extra black, then the 'missing black count' (see in del) is fixed
        // when x is _root or x is red, we can make the 'extra black' a real black
        while (x != _root && !x->c) // x is always a non-root node with double black
        {
            pointer p = x->p;
            int l = p->ch[0] == x;
            pointer s = p->ch[l];                                      // s is sibling
            if (s->c)                                              // case 1: sibling is red
                s->c = 0, p->c = 1, _rot(s), s = p->ch[l]; // (p is still x.p)
            if (!s->ch[0]->c && !s->ch[1]->c)          // case 2: sibling's both child are black
                s->c = 1, x = p;
            else
            {
                if (!s->ch[l]->c)                                                                // case 3: sibling's child against x's side is black
                    s->ch[l ^ 1]->c = 0, s->c = 1, _rot(s->ch[l ^ 1]), s = p->ch[l]; // (p is still x.p)
                s->c = p->c, p->c = s->ch[l]->c = 0, _rot(
                        s);                        // case 4: sibling's child against x's side is red
                x = _root;                                                                               // break loop, by the way make sure root is black
            }
        }
        x->c = 0;
    }

    int size() const
    { return _root->sz; }

    // [1, n] indexed
    iterator ins(int index, T key)
    {
        pointer x = _root, p = buf;
        pointer ret;
        int which = 0;
        while (x != buf)
        {
            ++(p = x)->sz;
            if (index <= x->ch[0]->sz + 1)
                x = x->ch[which = 0];
            else
                index -= x->ch[0]->sz + 1, x = x->ch[which = 1];
        }
        _ins_fix(((p != buf) ? p->ch[which] : _root) = ret = alloc(p, key));
        return {ret};
    }

    void push_back(T key)
    {
        if (_root == buf)
            return _ins_fix(_root = alloc(buf, key));
        pointer x = _root;
        while (x->ch[1] != buf)
            ++x->sz, x = x->ch[1];
        _ins_fix(x->ch[1] = alloc(x, key));
    }

    T &operator[](int index)
    {
        Node *x = _root;
        while (true)
        {
            if (index <= x->ch[0]->sz)
                x = x->ch[0];
            else if (index == x->ch[0]->sz + 1)
                return x->key;
            else
                index -= x->ch[0]->sz + 1, x = x->ch[1];
        }
    }

    void del(Iterator it)
    {
        for (pointer x = it.p; x != buf; x = x->p)
            --x->sz;
        pointer x = it.p, y, z;
        bool old_c;
        old_c = (y = x)->c;
        if (x->ch[0] == buf)
            _trans(x, z = x->ch[1]);
        else if (x->ch[1] == buf)
            _trans(x, z = x->ch[0]);
        else
        {
            for (y = x->ch[1]; y->ch[0] != buf; y = y->ch[0]) // y is the succ of x
                --y->sz;
            old_c = y->c;
            z = y->ch[1];
            if (y->p == x) // if y is linked to x, y will replace x directly, no need to fill y's place
                z->p = y;  // set z.p = y (z may be 0)
            else               // z will fill y's place
                _trans(y, z), (y->ch[1] = x->ch[1])->p = y;
            _trans(x, y), (y->ch[0] = x->ch[0])->p = y, y->c = x->c;
            y->sz = y->ch[0]->sz + y->ch[1]->sz + 1;
        }
        if (!old_c) // if y is black, y's branch will miss a black count
            _del_fix(z);
    }

    void del(int index)
    {
        pointer x = _root, y, z;
        bool old_c;
        while (x != buf)
        {
            if (index <= x->ch[0]->sz)
                x = x->ch[0];
            else if (index == x->ch[0]->sz + 1)
                break;
            else
                index -= x->ch[0]->sz + 1, x = x->ch[1];
        }
        del({x});
    }

    template<typename F>
    void foreach(pointer x, F f)
    {
        if (x == buf)
            return;
        foreach(x->ch[0], f);
        f(x->key);
        foreach(x->ch[1], f);
    }

    template<typename F>
    void foreach(F f)
    { foreach(_root, f); }

    iterator end()
    { return {buf}; }
};

template<typename T>
typename Seq<T>::Node Seq<T>::buf[N] = {{.ch={buf, buf}, .p=buf}};

template<typename T>
T min(T a, T b)
{ return a < b ? a : b; }

template<typename T>
T max(T a, T b)
{ return a < b ? b : a; }

struct IO
{
    char *p_in = in, *p_out = out;
    char in[int(2e7 + 5)], out[int(1e6 + 5)];

    IO()
    {
        fread(in, 1, sizeof in, stdin);
    }

    ~IO()
    { fwrite(out, 1, p_out - out, stdout); }

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

    int read(char *s)
    {
        char *n = strchr(p_in, '\n');
        int len = n - p_in;
        memcpy(s, p_in, len), s[len] = '\0';
        p_in = n + 1;
        return len;
    }
} io;

const int N = 500000 + 5;
char init[N];

int main()
{
    Seq<char> s;
    using iterator = Seq<char>::iterator;

    int n = io.read(init + 1), m = io;
    for (int i = 1; i <= n; ++i)
        s.push_back(init[i]);
    while (m--)
    {
        int pos = io + 1;
        ++io.p_in;
        char ch = *io.p_in++;
        iterator l = s.ins(pos, ch), r = l;
        while (true)
        {
            const int offset = 2;
            int lb = 0, rb = 1, _;
            char arr[5];
            iterator its[5], tmp;
            bool find = false;
            if (l == r && l != s.end()) // initial, put *l-- into arr
                arr[offset] = *(its[offset] = l), --lb, --l, ++r;
            for (tmp = l, _ = 0; tmp != s.end() && _ < 2; --lb, --tmp, ++_)
                arr[offset + lb] = *(its[offset + lb] = tmp);
            for (tmp = r, _ = 0; tmp != s.end() && _ < 2; ++rb, ++tmp, ++_)
                arr[offset + rb] = *(its[offset + rb] = tmp);
            for (int i = offset + lb + 1, j; i < offset + rb; ++i)
            {
                for (j = i + 1; j < offset + rb && arr[j] == arr[i]; ++j);
                if (j - i >= 3)
                {
                    l = its[i], --l;
                    r = its[j - 1], ++r;
                    for (int k = i; k < j; ++k)
                        s.del(its[k]);
                    find = true;
                    break;
                }
            }
            if (!find)
                break;
        }
    }
    s.foreach([](char ch) { *io.p_out++ = ch; });
}