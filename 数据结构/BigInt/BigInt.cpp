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

    BigInt() : _data(nullptr), _len(0), _cap(0)
    {}

    BigInt(int value) : BigInt()
    {
        if (value == 0)
            return;
        _len = value < 0 ? -1 : 1;
        _data = (u32 *) malloc(sizeof(u32) * (_cap = 2));
        _data[0] = value < 0 ? -value : value;
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
        _data = (u32 *) malloc(sizeof(u32) * _cap);
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
    { free(_data); }

    void _set_len(int abs_len)
    { _len = _len < 0 ? -abs_len : abs_len; }

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
            _data = (u32 *) realloc(_data, sizeof(u32) * (_cap = new_len + (new_len >> 1) + 1));
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
        else _neg_sub(rhs);
    }

    BigInt _div_mod(const BigInt &rhs)
    {
        // same memory layout for pointer cast, avoid destructor
        struct BigIntRef
        {
            u32 *_data;
            int _len, _cap;
        };
        BigInt m = *this;
        int l1 = len(), l2 = rhs.len();
        memset(_data, 0, sizeof(u32) * l1);
        int shift = l1 - l2;
        if (shift < 0)
            _len = 0;
        else
        {
            u32 *ext_mem = (u32 *) malloc((l1 + 1) * sizeof(u32));
            while (shift >= 0)
            {
                int l = 0, r = mod;
                // realloc will never be called for these two refs
                BigIntRef mul_test_{ext_mem, 0, l1 + 1}, m_high_{m._data + shift, m.len() - shift, l1};
                BigInt &mul_test = (BigInt &) mul_test_, &m_high = (BigInt &) m_high_;
                while (l <= r)
                {
                    int mid = (l + r) >> 1;
                    mul_test = rhs, mul_test *= mid;
                    mul_test._abs_comp(m_high) < 0 ? l = mid + 1 : r = mid - 1;
                }
                _data[shift] = r;
                mul_test = rhs, mul_test *= r;
                m_high -= mul_test, m._trim();
                --shift;
            }
            free(ext_mem);
            _trim();
            _len = rhs._len < 0 ? -_len : _len;
        }
        return m;
    }

    int len() const
    { return _len < 0 ? -_len : _len; }

    friend BigInt operator+(BigInt lhs, const BigInt &rhs)
    { return lhs += rhs; }

    friend BigInt operator-(BigInt lhs, const BigInt &rhs)
    { return lhs -= rhs; }

    friend BigInt operator*(BigInt lhs, int rhs)
    { return lhs *= rhs; }

    friend BigInt operator/(BigInt lhs, int rhs)
    { return lhs /= rhs; }

    BigInt operator-() const
    {
        BigInt ret = *this;
        ret._len = -ret._len;
        return ret;
    }

    BigInt &operator*=(const BigInt &rhs)
    { return *this = *this * rhs; }

    BigInt &operator/=(const BigInt &rhs)
    { return _div_mod(rhs), *this; }

    BigInt &operator%=(const BigInt &rhs)
    { return *this = _div_mod(rhs); }

    explicit operator bool() const 
    {  return _len; }

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

    BigInt &operator*=(int x)
    {
        if (x < 0)
            x = -x, _len = -_len;
        int l1 = len();
        _grow(l1 + 1);
        u64 carry = 0;
        for (int i = 0; i < l1; ++i)
        {
            carry += (u64) _data[i] * x;
            _data[i] = carry % mod;
            carry /= mod;
        }
        _data[l1] = carry;
        _trim();
        return *this;
    }

    BigInt &operator/=(int x)
    {
        if (x < 0)
            x = -x, _len = -_len;
        u64 carry = 0;
        for (int i = len() - 1; i >= 0; --i)
        {
            carry = carry * mod + _data[i];
            _data[i] = carry / x;
            carry %= x;
        }
        _trim();
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

    friend BigInt operator/(BigInt lhs, const BigInt &rhs)
    { return lhs._div_mod(rhs), lhs; }

    friend BigInt operator%(BigInt lhs, const BigInt &rhs)
    { return lhs._div_mod(rhs); }

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
};
#include <vector>
#include <iostream>

static std::vector<BigInt> g_f = {0, 1, 1, 2};
const BigInt &f(int n)
{
    if (g_f[n])
        return g_f[n];
    else if (n % 2 == 0)
        return g_f[n] = (f(n / 2 - 1) + f(n / 2 + 1)) * f(n / 2);
    else
        return g_f[n] = f(n / 2) * f(n / 2) + f(n / 2 + 1) * f(n / 2 + 1);
}
int main(int argc, char *argv[])
{
    int n;
    if (argc != 2 || (n = atoi(argv[1])) < 1)
        return 1;
    g_f.resize(n + 1);
    f(n);
}