#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>

#ifndef MIN_NTT
#define MIN_NTT 32
#endif

using u32 = unsigned;
using u64 = unsigned long long;
using u128 = __uint128_t;
const int u64_bits = 8 * sizeof(u64);

void *operator new(size_t, void *p)
{ return p; }

namespace stl
{
    template<typename T>
    T max(T a, T b)
    { return a < b ? a : b; }

    template<typename T>
    void swap(T &a, T &b)
    {
        T c = a;
        a = b;
        b = c;
    }

    template<typename T>
    T &&move(T &t)
    { return static_cast<T &&>(t); }

    template<typename It, typename T>
    void fill(It first, It last, T val)
    {
        auto trip_count = (last - first) >> 2;
        for (; trip_count > 0; --trip_count)
            *first++ = val, *first++ = val, *first++ = val, *first++ = val;
        switch (last - first)
        {
            case 3:
                *first++ = val;
            case 2:
                *first++ = val;
            case 1:
                *first++ = val;
            case 0:
            default:
                *first++ = val;
        }
    }

    template<typename InIt, typename OutIt>
    void copy(InIt first, InIt last, OutIt result)
    {
        auto trip_count = (last - first) >> 2;
        for (; trip_count > 0; --trip_count)
            *result++ = *first++, *result++ = *first++, *result++ = *first++, *result++ = *first++;
        switch (last - first)
        {
            case 3:
                *result++ = *first++;
            case 2:
                *result++ = *first++;
            case 1:
                *result++ = *first++;
            case 0:
            default:
                *result++ = *first++;
        }
    }

    // [first, last - 2] is a max heap
    // last[-1] is newly added by 'push_back' etc.
    // first == last => UB, will access first[0]
    template<typename It>
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
    // and [first, last - 2] will be a max heap
    // first == last => UB, will access first[0] & first[-1]
    template<typename It>
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

    template<typename It>
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

    template<typename F, typename S>
    struct Pair
    {
        F first;
        S second;
    };

    template<typename T>
    struct Vec
    {
        // typedef
        typedef T value_type;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef int size_type;
        typedef int difference_type;
        typedef pointer iterator;
        typedef const_pointer const_iterator;

        // initial status or after move : capacity = 0, _data = nullptr (be memset-ed to 0)
        pointer _data;
        size_type _size, _capacity;

        // life of object
        Vec(size_type size = 0, const_reference init = value_type()) : _data(nullptr), _size(size), _capacity(0)
        {
            reserve(_size);
            for (pointer it = _data, end = _data + _size; it != end; ++it)
                new(it) value_type(init);
        }

        Vec(const Vec &rhs) : _size(rhs._size), _capacity(rhs._capacity)
        {
            _data = reinterpret_cast<pointer>(malloc(sizeof(value_type) * _capacity));
            for (pointer it = _data, end = _data + _size, rit = rhs._data; it != end; ++it, ++rit)
                new(it) value_type(*rit);
        }

        Vec &operator=(const Vec &rhs)
        {
            if (this != &rhs)
            {
                clear();
                reserve(rhs._size);
                _size = rhs._size;
                for (pointer it = _data, end = _data + _size, rit = rhs._data; it != end; ++it, ++rit)
                    new(it) value_type(*rit);
            }
            return *this;
        }

        ~Vec()
        {
            clear();
            free(_data);
        }

        // element access
        reference operator[](size_type pos)
        { return _data[pos]; }

        const_reference operator[](size_type pos) const
        { return _data[pos]; }

        reference at(size_type pos)
        {
            assert(pos >= 0 && pos < _size);
            return _data[pos];
        }

        const_reference at(size_type pos) const
        {
            assert(pos >= 0 && pos < _size);
            return _data[pos];
        }

        reference front()
        { return _data[0]; }

        const_reference front() const
        { return _data[0]; }

        reference back()
        { return _data[_size - 1]; }

        const_reference back() const
        { return _data[_size - 1]; }

        pointer data()
        { return _data; }

        const_pointer data() const
        { return _data; }

        // modifiers
        void clear()
        {
            // doesn't free memory
            for (pointer it = _data, end = _data + _size; it != end; ++it)
                it->~value_type();
            _size = 0;
        }

        void insert(iterator pos, const_reference value)
        {
            // `insert and be there`
            int index = pos - _data;
            reserve(_size + 1); // pos may be invalidated here
            pos = _data + index;
            memmove(pos + 1, pos, sizeof(value_type) * (_size - index));
            pos->~value_type();
            new(pos) value_type(value);
            ++_size;
        }

        void erase(iterator pos)
        {
            // equivalent to erase(pos, pos + 1)
            pos->~value_type();
            memmove(pos, pos + 1, sizeof(value_type) * (_data + _size - pos - 1));
            --_size;
        }

        void erase(iterator first, iterator last)
        {
            for (pointer it = first; it != last; ++it)
                it->~value_type();
            memmove(first, last, sizeof(value_type) * (_data + _size - last));
            _size -= last - first;
        }

        void push_back(const_reference value)
        {
            reserve(_size + 1);
            new(_data + _size++) value_type(value);
        }

        void pop_back()
        { (_data + --_size)->~value_type(); }

        void resize(size_type size, const_reference value = value_type())
        {
            if (size < _size)
            {
                for (pointer it = _data + size, end = _data + _size; it != end; ++it)
                    it->~value_type();
            }
            else if (size > _size)
            {
                reserve(size);
                for (pointer it = _data + _size, end = _data + size; it != end; ++it)
                    new(it) value_type(value);
            }
            _size = size;
        }

        // capacity
        bool empty() const
        { return _size == 0; }

        size_type size() const
        { return _size; }

        void reserve(size_type capacity)
        {
            // standard says that if new capacity is greater than capacity()
            // capacity() can be `greater or equal to` new capacity after reserve
            if (_capacity < capacity)
                _data = reinterpret_cast<pointer>(realloc(_data, sizeof(value_type) * (_capacity = capacity << 1)));
        }

        size_type capacity() const
        { return _capacity; }

        void shrink_to_fit(size_type capacity)
        {
            if (capacity < _capacity && _size <= capacity)
                _data = reinterpret_cast<pointer>(realloc(_data, sizeof(value_type) * (_capacity = capacity)));
        }

        // iterators
        iterator begin()
        { return _data; }

        const_iterator begin() const
        { return _data; }

        iterator end()
        { return _data + _size; }

        const_iterator end() const
        { return _data + _size; }
    };
}

using namespace stl;

namespace montgomery
{
    constexpr u64 mul_inv(u64 n, u64 x = 1, int rec_cnt = 6)
    { return rec_cnt == 0 ? x : mul_inv(n, x * (2 - x * n), rec_cnt - 1); }

    template<u64 Mod, u64 PrimRoot>
    struct Int
    {
        const static u64 mod = Mod;
        // inv * Mod = 1 % (2 ^ 64)
        const static u64 inv = mul_inv(Mod);
        // r = 2 ^ 64 % Mod
        // r2 = 2 ^ 128 % Mod
        const static u64 r2 = ((u128(1) << 64) % Mod * (u128(1) << 64) % Mod) % Mod;
        const static int level = __builtin_ctzll(Mod - 1);
        static_assert(inv * Mod == 1, "invalid Mod");

        u64 x;

        Int() = default;

        Int(u64 n) : x(reduce(u128(n) * r2))
        {};

        static u64 reduce(u128 w)
        { return u64(w >> u64_bits) + Mod - u64((u128(u64(w) * inv) * Mod) >> u64_bits); }

        static Int omega()
        { return Int(PrimRoot).pow((Mod - 1) >> level); }

        Int &operator+=(Int rhs)
        { return x += rhs.x, *this; }

        Int &operator-=(Int rhs)
        { return x += 3 * Mod - rhs.x, *this; }

        Int &operator*=(Int rhs)
        { return x = reduce(u128(x) * rhs.x), *this; }

        Int operator+(Int rhs) const
        { return Int(*this) += rhs; }

        Int operator-(Int rhs) const
        { return Int(*this) -= rhs; }

        Int operator*(Int rhs) const
        { return Int(*this) *= rhs; }

        u64 get() const
        { return reduce(x) % Mod; }

        Int pow(u64 e) const
        {
            Int ret = Int(1);
            for (Int base = *this; e; e >>= 1, base *= base) if (e & 1) ret *= base;
            return ret;
        }

        Int inverse() const
        { return pow(Mod - 2); }
    };
}


namespace ntt
{
    template<typename Int>
    void transform(Int *A, int n, const Int *roots, const Int *inv_roots)
    {
        const int lg = __builtin_ctz(n), nh = n >> 1, lv = Int::level;
        const Int one = Int(1), img = roots[lv - 2];

        Int dw[lv - 1];
        dw[0] = roots[lv - 3];
        for (int i = 1; i < lv - 2; ++i) dw[i] = dw[i - 1] * inv_roots[lv - 1 - i] * roots[lv - 3 - i];
        dw[lv - 2] = dw[lv - 3] * inv_roots[1];

        if (lg & 1)
            for (int i = 0; i < nh; ++i)
            {
                Int a = A[i], b = A[i + nh];
                A[i] = a + b;
                A[i + nh] = a - b;
            }
        for (int e = lg & ~1; e >= 2; e -= 2)
        {
            const int m = 1 << e, m4 = m >> 2;
            Int w2 = one;
            for (int i = 0; i < n; i += m)
            {
                const Int w1 = w2 * w2, w3 = w1 * w2;
                for (int j = i; j < i + m4; ++j)
                {
                    Int a0 = A[j + m4 * 0] * one, a1 = A[j + m4 * 1] * w2;
                    Int a2 = A[j + m4 * 2] * w1, a3 = A[j + m4 * 3] * w3;
                    Int t02p = a0 + a2, t13p = a1 + a3;
                    Int t02m = a0 - a2, t13m = (a1 - a3) * img;
                    A[j + m4 * 0] = t02p + t13p;
                    A[j + m4 * 1] = t02p - t13p;
                    A[j + m4 * 2] = t02m + t13m;
                    A[j + m4 * 3] = t02m - t13m;
                }
                w2 *= dw[__builtin_ctz(~(i >> e))];
            }
        }
    }

    template<typename Int>
    void inv_transform(Int *A, int n, const Int *roots, const Int *inv_roots)
    {
        const int lg = __builtin_ctz(n), nh = n >> 1, lv = Int::level;
        const Int one = Int(1), img = inv_roots[lv - 2];

        Int dw[lv - 1];
        dw[0] = inv_roots[lv - 3];
        for (int i = 1; i < lv - 2; ++i) dw[i] = dw[i - 1] * roots[lv - 1 - i] * inv_roots[lv - 3 - i];
        dw[lv - 2] = dw[lv - 3] * roots[1];

        for (int e = 2; e <= lg; e += 2)
        {
            const int m = 1 << e, m4 = m >> 2;
            Int w2 = one;
            for (int i = 0; i < n; i += m)
            {
                const Int w1 = w2 * w2, w3 = w1 * w2;
                for (int j = i; j < i + m4; ++j)
                {
                    Int a0 = A[j + m4 * 0], a1 = A[j + m4 * 1];
                    Int a2 = A[j + m4 * 2], a3 = A[j + m4 * 3];
                    Int t01p = a0 + a1, t23p = a2 + a3;
                    Int t01m = a0 - a1, t23m = (a2 - a3) * img;
                    A[j + m4 * 0] = (t01p + t23p) * one;
                    A[j + m4 * 2] = (t01p - t23p) * w1;
                    A[j + m4 * 1] = (t01m + t23m) * w2;
                    A[j + m4 * 3] = (t01m - t23m) * w3;
                }
                w2 *= dw[__builtin_ctz(~(i >> e))];
            }
        }
        if (lg & 1)
            for (int i = 0; i < nh; ++i)
            {
                Int a = A[i], b = A[i + nh];
                A[i] = a + b;
                A[i + nh] = a - b;
            }
    }

    template<typename Int>
    void convolve(Int *A, int s1, Int *B, int s2, bool cyclic = false)
    {
        const int s = cyclic ? max(s1, s2) : s1 + s2 - 1;
        const int size = 1 << (31 - __builtin_clz(2 * s - 1));

        Int roots[Int::level], inv_roots[Int::level];
        roots[0] = Int::omega();
        for (int i = 1; i < Int::level; ++i) roots[i] = roots[i - 1] * roots[i - 1];
        inv_roots[0] = roots[0].inverse();
        for (int i = 1; i < Int::level; ++i) inv_roots[i] = inv_roots[i - 1] * inv_roots[i - 1];

        fill(A + s1, A + size, 0);
        transform(A, size, roots, inv_roots);
        const Int inv = Int(size).inverse();
        if (A == B && s1 == s2)
            for (int i = 0; i < size; ++i) A[i] *= A[i] * inv;
        else
        {
            fill(B + s2, B + size, 0);
            transform(B, size, roots, inv_roots);
            for (int i = 0; i < size; ++i) A[i] *= B[i] * inv;
        }
        inv_transform(A, size, roots, inv_roots);
    }
}

template<u64 Mod>
class FastDiv21
{
    using u128 = __uint128_t;
    static constexpr int s = __builtin_clzll(Mod);
    static constexpr u64 m = Mod << s;
    static constexpr u64 v = u64(~(u128(m) << 64) / m);
public:
    Pair<u64, u64> div_mod(u128 a) const
    {
        a <<= s;
        u64 ah = a >> 64, al = a;
        u128 q = u128(ah) * v + a;
        u64 qh = u64(q >> 64) + 1, ql = q, r = al - qh * m;
        if (r > ql) --qh, r += m;
        if (r >= m) ++qh, r -= m;
        return {qh, r >> s};
    }

    friend u64 operator%(u128 a, const FastDiv21 &b)
    { return b.div_mod(a).second; }

    friend u64 operator/(u128 a, const FastDiv21 &b)
    { return b.div_mod(a).first; }
};

namespace bigint
{
    using u64_1 = montgomery::Int<1121333583512862721, 51>;
    using u64_2 = montgomery::Int<1128298388379402241, 23>;

    struct BigInt : Vec<u64>
    {
        const static int digits = 16;
        const static u64 base = 10000000000000000ull;

        BigInt() = default;

        BigInt(u64 n) : Vec(1, n)
        {}

        BigInt(size_t s, u64 n) : Vec(s, n)
        {}

        BigInt(const char *s, int s_len = -1)
        {
            if (s_len == -1)
                s_len = strlen(s);
            reserve(s_len / digits + 1);
            int pos = s_len - 1;
            u64 cur = 0, pow = 1;
            for (; pos + 1 >= digits; pos -= digits, _data[_size++] = cur, cur = 0, pow = 1)
                for (int i = pos; i + digits > pos; --i)
                    cur += (s[i] - '0') * pow, pow *= 10;
            for (cur = 0, pow = 1; pos >= 0; --pos)
                cur += (s[pos] - '0') * pow, pow *= 10;
            if (cur)
                _data[_size++] = cur;
        }

        void normalize()
        { while (size() && back() == 0) pop_back(); }

        BigInt operator*(const BigInt &rhs) const
        {
            int s1 = size(), s2 = rhs.size(), s = s1 + s2 - 1;
            if (s1 < MIN_NTT || s2 < MIN_NTT)
            {
                BigInt ret(s1 + s2, 0);
                auto diver = FastDiv21<base>();
                for (int i = 0; i < s1; ++i)
                {
                    u128 carry = 0, f = _data[i];
                    u64 *p_ret = ret._data + i;
                    u64 *p_rhs = rhs._data, *end = rhs._data + s2;
                    while (p_rhs < end)
                    {
                        u128 tmp = *p_ret + *p_rhs++ * f + carry;
                        auto pr = diver.div_mod(tmp);
                        *p_ret++ = pr.second; // mod
                        carry = pr.first; // div
                    }
                    *p_ret += carry;
                }
                ret.normalize();
                return ret;
            }
            int ntt_size = 1 << (31 - __builtin_clz(2 * s - 1));
            Vec<u64_1> f1(ntt_size);
            copy(begin(), end(), f1.begin());
            if (this != &rhs)
            {
                Vec<u64_1> g1(ntt_size);
                copy(rhs.begin(), rhs.end(), g1.begin());
                ntt::convolve(f1.data(), s1, g1.data(), s2);
            }
            else
                ntt::convolve(f1.data(), s1, f1.data(), s1);
            Vec<u64_2> f2(ntt_size);
            copy(begin(), end(), f2.begin());
            if (this != &rhs)
            {
                Vec<u64_2> g2(ntt_size);
                copy(rhs.begin(), rhs.end(), g2.begin());
                ntt::convolve(f2.data(), s1, g2.data(), s2);
            }
            else
                ntt::convolve(f2.data(), s1, f2.data(), s1);
            BigInt ret(s1 + s2, 0);
            auto diver = FastDiv21<base>();
            const auto p1 = u64_1::mod, p2 = u64_2::mod;
            const auto inv = u64_2(p1).inverse();
            u128 carry = 0;
            for (int i = 0; i < s1 + s2 - 1; ++i)
            {
                auto r1 = f1[i].get(), r2 = f2[i].get();
                auto prod = r1 + u128((u64_2(r2 + p2 - r1) * inv).get()) * p1;
                prod += carry;
                u64 ph = prod >> u64_bits, pl = prod;
                u64 qh = ph / base, r = ph % base;
                auto div_mod = diver.div_mod(u128(r) << u64_bits | pl);
                u64 ql = div_mod.first;
                r = div_mod.second;
                carry = u128(qh) << u64_bits | ql;
                ret[i] = r;
            }
            ret[s1 + s2 - 1] = carry;
            ret.normalize();
            return ret;
        }

        BigInt &operator*=(const BigInt &rhs)
        { return *this = *this * rhs; }

        void print() const
        {
            printf("%llu", back());
            char str[digits + 1] = {};
            for (int i = size() - 2; i >= 0; --i)
            {
                auto a = (*this)[i];
                for (int j = 0; j < digits; ++j)
                    str[digits - 1 - j] = a % 10 + '0', a /= 10;
                for (int j = 0; j < digits; ++j) putchar(str[j]);
            }
            puts("");
        }

        int cmp(const BigInt &rhs) const
        {
            if (_size != rhs._size)
                return _size < rhs._size ? -1 : 1;
            for (int i = _size - 1; i >= 0; --i)
                if ((*this)[i] != rhs[i])
                    return (*this)[i] < rhs[i] ? -1 : 1;
            return 0;
        }

        bool operator<(const BigInt &rhs) const
        { return cmp(rhs) > 0; }
    };

    void swap(BigInt &a, BigInt &b)
    {
        stl::swap(a._data, b._data);
        stl::swap(a._size, b._size);
        stl::swap(a._capacity, b._capacity);
    }
}

//#include <chrono>
using namespace bigint;
BigInt ans[2005], a[2005];

BigInt calc(int l, int r)
{
    if (l == r - 1)
        return ans[l];
    if (l == r)
        return 1;
    int s = 0, s_2 = 0;
    for (int i = l; i < r; ++i)
        s += ans[i]._size;
    s >>= 1;
    for (int i = l; i < r; ++i)
    {
        if (s_2 <= s && s < s_2 + ans[i]._size)
            return calc(l, i) * calc(i, r);
        s_2 += ans[i]._size;
    }
}

int main()
{
    using namespace bigint;
//    auto beg = std::chrono::high_resolution_clock::now();
    int n;
    scanf("%d", &n);
    BigInt *last = a + n;
    for (int i = 0, x; i < n; ++i)
        scanf("%d", &x), a[i] = x;
    make_heap(a, last);
    for (int i = 0; i < n - 1; ++i)
    {
        BigInt x = a[0];
        pop_heap(a, last--);
        BigInt y = a[0];
        pop_heap(a, last--);
//        fprintf(stderr, "%d ", ans._size);
//        ans *= y *= x
        ans[i] = y *= x;
//        fprintf(stderr, "%d\n", y._size);
        *last++ = y;
        push_heap(a, last);
    }
    calc(0, n - 1).print();
//    fprintf(stderr, "%lf\n", std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - beg).count());
}

