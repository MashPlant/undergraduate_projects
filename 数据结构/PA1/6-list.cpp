template<typename F, typename S>
struct Pair
{
    F first;
    S second;
};

template<typename T, int N>
struct List
{
    static int alloc;
    struct Node
    {
        T value;
        int prev, next;
    };
    static Node buf[N];

    int nil;

    struct iterator
    {
        Node *x;
        List *p;

        T *operator->() const
        { return &x->value; }

        T &operator*() const
        { return x->value; }

        iterator &operator++()
        { return x = x->next + p->buf, *this; }

        iterator &operator--()
        { return x = x->prev + p->buf, *this; }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            --*this;
            return tmp;
        }

        iterator next() const
        {
            iterator tmp = *this;
            return ++tmp;
        }

        iterator prev() const
        {
            iterator tmp = *this;
            return --tmp;
        }

        bool operator==(iterator rhs) const
        { return x == rhs.x; }

        bool operator!=(iterator rhs) const
        { return x != rhs.x; }

        iterator insert_before(const T &value) const
        { return p->insert_before(*this, value); }

        iterator insert_after(const T &value) const
        { return p->insert_after(*this, value); }

        Pair<iterator, iterator> erase() const
        { return p->erase(*this); }
    };

    List()
    {
        nil = ++alloc;
        buf[nil].next = buf[nil].prev = nil;
    }

    iterator begin()
    { return {buf + buf[nil].next, this}; }

    iterator end()
    { return {buf + nil, this}; }

    iterator push_back(const T &value)
    { return insert_before(end(), value); }

    iterator push_front(const T &value)
    { return insert_after(end(), value); }

    iterator insert_before(iterator it, const T &value)
    {
        Node *x = buf + ++alloc;
        int prev = it.x->prev;
        x->value = value;
        buf[prev].next = alloc;
        x->prev = prev;
        x->next = it.x - buf;
        it.x->prev = alloc;
        return {x, this};
    }

    iterator insert_after(iterator it, const T &value)
    {
        Node *x = buf + ++alloc;
        int next = it.x->next;
        x->value = value;
        x->next = next;
        buf[next].prev = alloc;
        it.x->next = alloc;
        x->prev = it.x - buf;
        return {x, this};
    }

    Pair<iterator, iterator> erase(iterator it)
    {
        int prev = it.x->prev, next = it.x->next;
        buf[prev].next = next;
        buf[next].prev = prev;
        return {iterator{buf + prev, this}, iterator{buf + next, this}};
    }

    void splice_after(iterator it, iterator first, iterator last)
    {
        int this_next = it.x->next;
        int other_prev = first.x->prev;
        it.x->next = first.x - buf;
        first.x->prev = it.x - buf;
        buf[last.x->prev].next = this_next;
        buf[this_next].prev = last.x->prev;
        buf[other_prev].next = last.x - buf;
        last.x->prev = other_prev;
    }

    void splice_before(iterator it, iterator first, iterator last)
    {
        int this_prev = it.x->prev;
        int other_prev = first.x->prev;
        buf[this_prev].next = first.x - buf;
        first.x->prev = this_prev;
        buf[last.x->prev].next = it.x - buf;
        it.x->prev = last.x->prev;
        buf[other_prev].next = last.x - buf;
        last.x->prev = other_prev;
    }
};

template<typename T, int N>
int List<T, N>::alloc = 0;

template<typename T, int N>
typename List<T, N>::Node List<T, N>::buf[N];


#include <cstdio>
#include <cstring>
#include <cctype>

const int N = 7200000 + 5;
List<char, N * 2> lst, rev;
using iterator = decltype(lst.begin());

struct IO
{
    char *p_in = in, *p_out = out;
    char in[3200000 + 4000000 * 6 + 2333], out[20 * 1024 * 1024 + 5];

    IO()
    { fread(in, 1, sizeof in, stdin); }

    ~IO()
    { fwrite(out, 1, p_out - out, stdout); }

    char next_char()
    {
        char ch;
        while (isspace(ch = *p_in++));
        return ch;
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

    void println(char ch)
    { *p_out++ = ch, *p_out++ = '\n'; }
} io;


int main()
{
    {
        char ch;
        while (!isspace(ch = *io.p_in++))
            lst.push_back(ch), rev.push_front(ch);
    }
    int t = io;
    char op, w, val;
    bool ok = true;
    iterator left = lst.begin(), right = lst.end();
    iterator rev_left = rev.end(), rev_right = rev.end().prev();
    while (t--)
    {
        op = io.next_char();
        switch (op)
        {
            case '<':
                w = io.next_char();
                if (w == 'L')
                {
                    if (left == lst.begin())
                        io.println('F');
                    else
                    {
                        io.println('T');
                        if (left-- == right)
                            ok = true;
                        ++rev_right;
                    }
                }
                else
                {
                    if (right == lst.begin())
                        io.println('F');
                    else
                    {
                        io.println('T');
                        if (--right == left)
                            ok = false;
                        ++rev_left;
                    }
                }
                break;
            case '>':
                w = io.next_char();
                if (w == 'L')
                {
                    if (left == lst.end())
                        io.println('F');
                    else
                    {
                        io.println('T');
                        if (++left == right)
                            ok = false;
                        --rev_right;
                    }
                }
                else
                {
                    if (right == lst.end())
                        io.println('F');
                    else
                    {
                        io.println('T');
                        if (right++ == left)
                            ok = true;
                        --rev_left;
                    }
                }
                break;
            case 'I':
                w = io.next_char(), val = io.next_char();
                io.println('T');
                if (w == 'L')
                {
                    left.insert_before(val);
                    rev_right.insert_after(val);
                }
                else
                {
                    right.insert_before(val);
                    rev_left.insert_after(val);
                }
                break;
            case 'D':
                w = io.next_char();
                if (w == 'L')
                {
                    if (left == lst.end())
                        io.println('F');
                    else
                    {
                        io.println('T');
                        if (left == right)
                        {
                            left = right = left.erase().second;
                            rev_left = rev_right = rev_right.erase().first;
                        }
                        else
                        {
                            left = left.erase().second;
                            rev_right = rev_right.erase().first;
                        }
                        if (left == right)
                            ok = false;

                    }
                }
                else
                {
                    if (right == lst.end())
                        io.println('F');
                    else
                    {
                        io.println('T');
                        if (left == right)
                        {
                            left = right = right.erase().second;
                            rev_left = rev_right = rev_left.erase().first;
                        }
                        else
                        {
                            right = right.erase().second;
                            rev_left = rev_left.erase().first;
                        }
                        if (left == right)
                            ok = false;
                    }
                }
                break;
            case 'R':
                if (!ok)
                    io.println('F');
                else
                {
                    io.println('T');
                    auto nl = rev_left.next(), nr = right.prev();
                    lst.splice_before(left, nl, rev_right.next());
                    rev.splice_after(rev_left, left, right);
                    left = nl;
                    rev_right = nr;
                }
                break;
            case 'S':
                for (char ch : lst)
                    *io.p_out++ = ch;
                *io.p_out++ = '\n';
                break;
        }
    }
}