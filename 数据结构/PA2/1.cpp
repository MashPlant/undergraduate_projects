#include <cstring>
#include <cstdlib>
#include <cstdio>

struct BigInt
{
    using u32 = unsigned;
    using u64 = unsigned long long;
    const static int exp = 9;
    const static u32 mod = 1000000000;

    u32 *_data; // _data[_len : _cap] is undefined!
    int _len, _cap;

    BigInt() : _data(nullptr), _len(0), _cap(0) {}

    BigInt(int value) : BigInt()
    {
        if (value == 0)
            return;
        bool sgn = value < 0;
        value = sgn ? -value : value;
        _data = (u32 *)malloc(sizeof(u32) * (_cap = 2));
        if (value >= mod)
            _len = 2, _data[0] = value % mod, _data[1] = value / mod;
        else
            _len = 1, _data[0] = value;
        _len = sgn ? -_len : _len;
    }

    BigInt(const char *s, int s_len = -1) : BigInt()
    {
        if (s_len == -1)
            s_len = strlen(s);
        _grow(s_len / exp + 1), _len = 0;
        int bound = 0, sgn = 0, pos;
        if (s[0] == '-')
            sgn = bound = 1;
        u64 cur = 0, pow = 1;
        for (pos = s_len - 1; pos + 1 >= exp + bound; pos -= exp, _data[_len++] = cur, cur = 0, pow = 1)
            for (int i = pos; i + exp > pos; --i)
                cur += (s[i] - '0') * pow, pow *= 10;
        for (cur = 0, pow = 1; pos >= bound; --pos)
            cur += (s[pos] - '0') * pow, pow *= 10;
        if (cur)
            _data[_len++] = cur;
        _len = sgn ? -_len : _len;
    }

    BigInt(const BigInt &rhs)
    {
        _len = rhs._len, _cap = rhs._cap;
        _data = (u32 *)malloc(sizeof(u32) * _cap);
        memcpy(_data, rhs._data, sizeof(u32) * len());
    }

    BigInt &operator=(const BigInt &rhs)
    {
        if (this != &rhs)
        {
            if (len() < rhs.len())
                _grow(rhs.len());
            _len = rhs._len;
            memcpy(_data, rhs._data, sizeof(u32) * len());
        }
        return *this;
    }

    BigInt(BigInt &&rhs)
    {
        _data = rhs._data, _len = rhs._len, _cap = rhs._cap;
        rhs._data = nullptr, rhs._len = rhs._cap = 0;
    }

    BigInt &operator=(BigInt &&rhs)
    {
        if (this != &rhs)
        {
            _data = rhs._data, _len = rhs._len, _cap = rhs._cap;
            rhs._data = nullptr, rhs._len = rhs._cap = 0;
        }
        return *this;
    }

    ~BigInt()
    {
        free(_data);
    }

    void _set_len(int abs_len)
    {
        _len = _len < 0 ? -abs_len : abs_len;
    }

    int _abs_comp(const BigInt &rhs) const
    {
        int l1 = len(), l2 = rhs.len();
        if (l1 != l2)
            return l1 < l2 ? -1 : 1;
        for (int i = l1 - 1; i >= 0; --i)
            if (_data[i] != rhs._data[i])
                return _data[i] < rhs._data[i] ? -1 : 1;
        return 0;
    }

    void _trim()
    {
        int l = len();
        while (l && _data[l - 1] == 0)
            --l;
        _set_len(l);
    }

    void _grow(int new_len)
    {
        // assert(len() <= new_len)
        if (_cap < new_len)
            _data = (u32 *)realloc(_data, sizeof(u32) * (_cap = new_len + (new_len >> 1) + 1));
        memset(_data + len(), 0, sizeof(u32) * (new_len - len()));
        _set_len(new_len);
    }

    void _add(const BigInt &rhs)
    {
        int l1 = len(), l2 = rhs.len();
        int l3 = (l1 < l2 ? l2 : l1) + 1;
        _grow(l3);
        u32 carry = 0, res;
        for (int i = 0; i < l2; ++i)
        {
            res = _data[i] + rhs._data[i] + carry;
            carry = res >= mod;
            _data[i] = carry ? res - mod : res;
        }
        for (int i = l2; i < l3 && carry; ++i)
        {
            res = _data[i] + carry;
            carry = res >= mod;
            _data[i] = carry ? res - mod : res;
        }
        _trim();
    }

    void _pos_sub(const BigInt &rhs) // *this > rhs, no need to _grow
    {
        int l1 = len(), l2 = rhs.len();
        u32 carry = 0, res;
        for (int i = 0; i < l2; ++i)
        {
            res = _data[i] - rhs._data[i] - carry;
            carry = res >> 31;
            _data[i] = carry ? res + mod : res;
        }
        for (int i = l2; i < l1 && carry; ++i)
        {
            res = _data[i] - carry;
            carry = res >> 31;
            _data[i] = carry ? res + mod : res;
        }
        _trim();
    }

    void _neg_sub(const BigInt &rhs) // *this < rhs, _grow to hold l2
    {
        int l1 = len(), l2 = rhs.len();
        _grow(l2);
        u32 carry = 0, res;
        for (int i = 0; i < l2; ++i)
        {
            res = rhs._data[i] - _data[i] - carry;
            carry = res >> 31;
            _data[i] = carry ? res + mod : res;
        }
        _trim();
        _len = -_len;
    }

    void _sub(const BigInt &rhs)
    {
        int cmp = _abs_comp(rhs);
        if (cmp == 0)
            _len = 0;
        else if (cmp > 0)
            _pos_sub(rhs);
        else
            _neg_sub(rhs);
    }

    int len() const
    {
        return _len < 0 ? -_len : _len;
    }

    friend BigInt operator+(BigInt lhs, const BigInt &rhs)
    {
        return lhs += rhs;
    }

    friend BigInt operator-(BigInt lhs, const BigInt &rhs)
    {
        return lhs -= rhs;
    }

    BigInt operator-() const
    {
        BigInt ret = *this;
        ret._len = -ret._len;
        return ret;
    }

    BigInt &operator*=(const BigInt &rhs)
    {
        return *this = *this * rhs;
    }

    explicit operator bool() const
    {
        return _len;
    }

    BigInt &operator+=(const BigInt &rhs)
    {
        (_len ^ rhs._len) < 0 ? _sub(rhs) : _add(rhs);
        return *this;
    }

    BigInt &operator-=(const BigInt &rhs)
    {
        (_len ^ rhs._len) < 0 ? _add(rhs) : _sub(rhs);
        return *this;
    }

    friend BigInt operator*(const BigInt &lhs, const BigInt &rhs)
    {
        BigInt ret;
        int l1 = lhs.len(), l2 = rhs.len();
        ret._grow(l1 + l2);
        for (int i = 0; i < l1; ++i)
        {
            u64 carry = 0, f = lhs._data[i];
            u32 *p_ret = ret._data + i;
            u32 *p_rhs = rhs._data, *end = rhs._data + l2;
            while (p_rhs < end)
            {
                carry += *p_ret + *p_rhs++ * f;
                *p_ret++ = carry % mod;
                carry /= mod;
            }
            *p_ret += carry;
        }
        ret._trim();
        ret._len = (lhs._len ^ rhs._len) < 0 ? -ret._len : ret._len;
        return ret;
    }

    void print() const
    {
        if (len())
        {
            if (_len < 0)
                putchar('-');
            printf("%u", _data[len() - 1]);
            for (int i = len() - 2; i >= 0; --i)
                printf("%0*u", exp, _data[i]);
        }
        else
            putchar('0');
        puts("");
    }

    bool operator<(const BigInt &rhs) const
    {
        return _abs_comp(rhs) > 0;
    }
};
template <typename T>
void swap(T &a, T &b)
{
    T c = a;
    a = b;
    b = c;
}

template <typename T>
T &&move(T &t)
{
    return static_cast<T &&>(t);
}

void swap(BigInt &a, BigInt &b)
{
    swap(a._data, b._data);
    swap(a._len, b._len);
    swap(a._cap, b._cap);
}

// [first, last - 2] is a max heap
// last[-1] is newly added by 'push_back' etc.
// first == last => UB, will access first[0]
template <typename It>
void push_heap(It first, It last)
{
    int n = last - first--, fa = n >> 1;
    auto val = move(first[n]);
    while (fa && first[fa] < val)
    {
        first[n] = move(first[fa]);
        n = fa, fa = n >> 1;
    }
    first[n] = move(val);
}

// [first, last - 1] is a max heap
// after pop, original top will be moved to last - 1, can be removed by `pop_back` etc.
// and [first, last - 2] wiil be a max heap
// first == last => UB, will access first[0] & first[-1]
template <typename It>
void pop_heap(It first, It last)
{
    int n = last - first, fa = 0, son = 1;
    swap(first[0], first[--n]);
    auto val = move(first[0]);
    while (true)
    {
        if (son + 1 < n && first[son] < first[son + 1])
            ++son;
        if (son < n && val < first[son])
            first[fa] = move(first[son]), fa = son, son = (fa << 1) + 1;
        else
            break;
    }
    first[fa] = move(val);
}

template <typename It>
void make_heap(It first, It last)
{
    int n = last - first;
    for (int i = n >> 1; i >= 0; --i)
    {
        int cur = i, son = (cur << 1) + 1;
        auto val = move(first[cur]);
        while (true)
        {
            if (son + 1 < n && first[son] < first[son + 1])
                ++son;
            if (son < n && val < first[son])
                first[cur] = move(first[son]), cur = son, son = (cur << 1) + 1;
            else
                break;
        }
        first[cur] = move(val);
    }
}

int main()
{
    int n;
    BigInt a[2005], ans = 1, *last;
    scanf("%d", &n), last = a + n;
    for (int i = 0, x; i < n; ++i)
        scanf("%d", &x), a[i] = x;
    make_heap(a, last);
    while (last - a > 1)
    {
        BigInt x = a[0];
        pop_heap(a, last--);
        BigInt y = a[0];
        pop_heap(a, last--);
        ans *= y *= x;
        *last++ = y;
        push_heap(a, last);
    }
    ans.print();
}