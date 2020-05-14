// luogu-judger-enable-o2
// luogu-judger-enable-o2
//test P1919_000000001
#include <bits/stdc++.h>
#define getchar getchar_unlocked
#define putchar putchar_unlocked
using namespace std;
using i64 = long long;
using u8 = unsigned char;
using u32 = unsigned;
using u64 = unsigned long long;
using f80 = long double;
namespace ntt
{
using word_t = u64;
using dword_t = __uint128_t;
static constexpr word_t mul_inv(word_t n, int e = 6, word_t x = 1)
{
    return e == 0 ? x : mul_inv(n, e - 1, x * (2 - x * n));
}

template <word_t mod, word_t prim_root>
class UnsafeMod
{
  private:
    static const int word_bits = 8 * sizeof(word_t);

  public:
    static constexpr word_t inv = mul_inv(mod);
    static constexpr word_t r2 = -dword_t(mod) % mod;
    static constexpr int level = __builtin_ctzll(mod - 1);
    static_assert(inv * mod == 1, "invalid 1/M modulo 2^@.");

    UnsafeMod() {}
    UnsafeMod(word_t n) : x(init(n)){};
    static word_t modulus() { return mod; }
    static word_t init(word_t w) { return reduce(dword_t(w) * r2); }
    static word_t reduce(const dword_t w)
    {
        return word_t(w >> word_bits) + mod - word_t((dword_t(word_t(w) * inv) * mod) >> word_bits);
    }
    static UnsafeMod omega() { return UnsafeMod(prim_root).pow((mod - 1) >> level); }
    UnsafeMod &operator+=(UnsafeMod rhs)
    {
        x += rhs.x;
        return *this;
    }
    UnsafeMod &operator-=(UnsafeMod rhs)
    {
        x += 3 * mod - rhs.x;
        return *this;
    }
    UnsafeMod &operator*=(UnsafeMod rhs)
    {
        x = reduce(dword_t(x) * rhs.x);
        return *this;
    }
    UnsafeMod operator+(UnsafeMod rhs) const { return UnsafeMod(*this) += rhs; }
    UnsafeMod operator-(UnsafeMod rhs) const { return UnsafeMod(*this) -= rhs; }
    UnsafeMod operator*(UnsafeMod rhs) const { return UnsafeMod(*this) *= rhs; }
    word_t get() const { return reduce(x) % mod; }
    void set(word_t n) { x = n; }
    UnsafeMod pow(word_t e) const
    {
        UnsafeMod ret = UnsafeMod(1);
        for (UnsafeMod base = *this; e; e >>= 1, base *= base)
            if (e & 1)
                ret *= base;
        return ret;
    }
    UnsafeMod inverse() const { return pow(mod - 2); }
    friend ostream &operator<<(ostream &os, const UnsafeMod &m) { return os << m.get(); }
    static void debug() { printf("%llu %llu %llu %llu\n", mod, inv, r2, omega().get()); }
    word_t x;
};

template <typename mod_t>
void transform(mod_t *A, int n, const mod_t *roots, const mod_t *iroots)
{
    const int logn = __builtin_ctz(n), nh = n >> 1, lv = mod_t::level;
    const mod_t one = mod_t(1), imag = roots[lv - 2];

    mod_t dw[lv - 1];
    dw[0] = roots[lv - 3];
    for (int i = 1; i < lv - 2; ++i)
        dw[i] = dw[i - 1] * iroots[lv - 1 - i] * roots[lv - 3 - i];
    dw[lv - 2] = dw[lv - 3] * iroots[1];

    if (logn & 1)
        for (int i = 0; i < nh; ++i)
        {
            mod_t a = A[i], b = A[i + nh];
            A[i] = a + b;
            A[i + nh] = a - b;
        }
    for (int e = logn & ~1; e >= 2; e -= 2)
    {
        const int m = 1 << e, m4 = m >> 2;
        mod_t w2 = one;
        for (int i = 0; i < n; i += m)
        {
            const mod_t w1 = w2 * w2, w3 = w1 * w2;
            for (int j = i; j < i + m4; ++j)
            {
                mod_t a0 = A[j + m4 * 0] * one, a1 = A[j + m4 * 1] * w2;
                mod_t a2 = A[j + m4 * 2] * w1, a3 = A[j + m4 * 3] * w3;
                mod_t t02p = a0 + a2, t13p = a1 + a3;
                mod_t t02m = a0 - a2, t13m = (a1 - a3) * imag;
                A[j + m4 * 0] = t02p + t13p;
                A[j + m4 * 1] = t02p - t13p;
                A[j + m4 * 2] = t02m + t13m;
                A[j + m4 * 3] = t02m - t13m;
            }
            w2 *= dw[__builtin_ctz(~(i >> e))];
        }
    }
}

template <typename mod_t>
void itransform(mod_t *A, int n, const mod_t *roots, const mod_t *iroots)
{
    const int logn = __builtin_ctz(n), nh = n >> 1, lv = mod_t::level;
    const mod_t one = mod_t(1), imag = iroots[lv - 2];

    mod_t dw[lv - 1];
    dw[0] = iroots[lv - 3];
    for (int i = 1; i < lv - 2; ++i)
        dw[i] = dw[i - 1] * roots[lv - 1 - i] * iroots[lv - 3 - i];
    dw[lv - 2] = dw[lv - 3] * roots[1];

    for (int e = 2; e <= logn; e += 2)
    {
        const int m = 1 << e, m4 = m >> 2;
        mod_t w2 = one;
        for (int i = 0; i < n; i += m)
        {
            const mod_t w1 = w2 * w2, w3 = w1 * w2;
            for (int j = i; j < i + m4; ++j)
            {
                mod_t a0 = A[j + m4 * 0], a1 = A[j + m4 * 1];
                mod_t a2 = A[j + m4 * 2], a3 = A[j + m4 * 3];
                mod_t t01p = a0 + a1, t23p = a2 + a3;
                mod_t t01m = a0 - a1, t23m = (a2 - a3) * imag;
                A[j + m4 * 0] = (t01p + t23p) * one;
                A[j + m4 * 2] = (t01p - t23p) * w1;
                A[j + m4 * 1] = (t01m + t23m) * w2;
                A[j + m4 * 3] = (t01m - t23m) * w3;
            }
            w2 *= dw[__builtin_ctz(~(i >> e))];
        }
    }
    if (logn & 1)
        for (int i = 0; i < nh; ++i)
        {
            mod_t a = A[i], b = A[i + nh];
            A[i] = a + b;
            A[i + nh] = a - b;
        }
}

template <typename mod_t>
void convolve(mod_t *A, int s1, mod_t *B, int s2, bool cyclic = false)
{
    const int s = cyclic ? max(s1, s2) : s1 + s2 - 1;
    const int size = 1 << (31 - __builtin_clz(2 * s - 1));
    assert(size <= (i64(1) << mod_t::level));

    mod_t roots[mod_t::level], iroots[mod_t::level];
    roots[0] = mod_t::omega();
    for (int i = 1; i < mod_t::level; ++i)
        roots[i] = roots[i - 1] * roots[i - 1];
    iroots[0] = roots[0].inverse();
    for (int i = 1; i < mod_t::level; ++i)
        iroots[i] = iroots[i - 1] * iroots[i - 1];

    fill(A + s1, A + size, 0);
    transform(A, size, roots, iroots);
    const mod_t inv = mod_t(size).inverse();
    if (A == B && s1 == s2)
    {
        for (int i = 0; i < size; ++i)
            A[i] *= A[i] * inv;
    }
    else
    {
        fill(B + s2, B + size, 0);
        transform(B, size, roots, iroots);
        for (int i = 0; i < size; ++i)
            A[i] *= B[i] * inv;
    }
    itransform(A, size, roots, iroots);
}

} // namespace ntt

using m64_1 = ntt::UnsafeMod<1121333583512862721, 51>;
using m64_2 = ntt::UnsafeMod<1128298388379402241, 23>; // <= 1.14e18 (sub.D = 3)

template <u64 Modulus>
class FastDiv21
{
    using u128 = __uint128_t;
    static constexpr int s = __builtin_clzll(Modulus);
    static constexpr u64 m = Modulus << s;
    static constexpr u64 v = u64(~(u128(m) << 64) / m);

  public:
    pair<u64, u64> divmod(u128 a) const
    {
        a <<= s;
        u64 ah = a >> 64, al = a;
        u128 q = u128(ah) * v + a;
        u64 qh = u64(q >> 64) + 1, ql = q, r = al - qh * m;
        if (r > ql)
            --qh, r += m;
        if (r >= m)
            ++qh, r -= m;
        return {qh, r >> s};
    }
    friend u64 operator%(u128 a, const FastDiv21 &b) { return b.divmod(a).second; }
    friend u64 operator/(u128 a, const FastDiv21 &b) { return b.divmod(a).first; }
};

using word_t = u64;
using sword_t = i64;
using dword_t = __uint128_t;
constexpr int kWordBits = sizeof(word_t) * 8;
constexpr int base_tens = 16;

template <word_t Base>
class BigInteger : public vector<word_t>
{
    static_assert(word_t(Base) < (word_t(1) << (kWordBits - 1)), "Base is too large.");

  public:
    BigInteger() : BigInteger(0) {}
    BigInteger(word_t n) : vector<word_t>(1, n) {}
    BigInteger(size_t s, word_t n) : vector<word_t>(s, n) {}

    BigInteger(char *cstr) : vector<word_t>(0)
    {
        size_t nstr = strlen(cstr);
        reverse(cstr, cstr + nstr);
        for (; cstr[nstr - 1] == '0' && nstr > 1; --nstr)
            ;
        size_t curr = 0;
        word_t t = 0, e = 1;

        resize((nstr + base_tens - 1) / base_tens);
        for (size_t i = 0; i < nstr; ++i)
        {
            if (e == Base)
                (*this)[curr++] = t, t = 0, e = 1;
            t += e * (cstr[i] - '0');
            e *= 10;
        }
        if (t)
            (*this)[curr] = t;
    }

    void normalize()
    {
        while (size() > 1 && back() == 0)
            pop_back();
    }

    BigInteger operator+(const BigInteger &rhs) const
    {
        size_t s = max(size(), rhs.size()) + 1;
        BigInteger ret(s, 0);
        copy(begin(), end(), ret.begin());
        word_t carry = 0;
        for (size_t i = 0; i < rhs.size(); ++i)
        {
            word_t a = ret[i] + rhs[i] + carry;
            carry = 0;
            if (a >= Base)
                ++carry, a -= Base;
            ret[i] = a;
        }
        for (size_t i = rhs.size(); carry; ++i)
        {
            word_t a = ret[i] + carry;
            carry = 0;
            if (a >= Base)
                ++carry, a -= Base;
            ret[i] = a;
        }
        ret.normalize();
        return ret;
    }

    BigInteger operator-(const BigInteger &rhs) const
    {
        assert(size() > rhs.size() || (size() == rhs.size() && back() >= rhs.back()));
        BigInteger ret(size(), 0);
        copy(begin(), end(), ret.begin());
        word_t carry = 0;
        for (size_t i = 0; i < rhs.size(); ++i)
        {
            word_t a = ret[i] - rhs[i] - carry;
            carry = 0;
            if (sword_t(a) < 0)
                a += Base, carry = 1;
            ret[i] = a;
        }
        for (size_t i = rhs.size(); carry; ++i)
        {
            word_t a = ret[i] - carry;
            carry = 0;
            if (sword_t(a) < 0)
                a += Base, carry = 1;
            ret[i] = a;
        }
        ret.normalize();
        return ret;
    }

    BigInteger operator*(const BigInteger &rhs) const
    {
        int s1 = size(), s2 = rhs.size(), s = s1 + s2 - 1;
        int ntt_size = 1 << (31 - __builtin_clz(2 * s - 1));
        vector<m64_1> f1(ntt_size);
        copy(begin(), end(), f1.begin());
        if (this != &rhs)
        {
            vector<m64_1> g1(ntt_size);
            copy(rhs.begin(), rhs.end(), g1.begin());
            ntt::convolve(f1.data(), s1, g1.data(), s2);
        }
        else
        {
            ntt::convolve(f1.data(), s1, f1.data(), s1);
        }
        vector<m64_2> f2(ntt_size);
        copy(begin(), end(), f2.begin());
        if (this != &rhs)
        {
            vector<m64_2> g2(ntt_size);
            copy(rhs.begin(), rhs.end(), g2.begin());
            ntt::convolve(f2.data(), s1, g2.data(), s2);
        }
        else
        {
            ntt::convolve(f2.data(), s1, f2.data(), s1);
        }
        BigInteger ret(s1 + s2, 0);
        auto fdiv = FastDiv21<Base>();
        const auto p1 = m64_1::modulus(), p2 = m64_2::modulus();
        const auto inv = m64_2(p1).inverse();
        dword_t carry = 0;
        for (int i = 0; i < s1 + s2 - 1; ++i)
        {
            auto r1 = f1[i].get(), r2 = f2[i].get();
            auto prod = r1 + dword_t((m64_2(r2 + p2 - r1) * inv).get()) * p1;
            prod += carry;
            word_t ph = prod >> kWordBits, pl = prod;
            word_t qh = ph / Base, r = ph % Base;
            word_t ql;
            tie(ql, r) = fdiv.divmod(dword_t(r) << kWordBits | pl);
            carry = dword_t(qh) << kWordBits | ql;
            ret[i] = r;
        }
        ret[s1 + s2 - 1] = carry;
        ret.normalize();
        return ret;
    }

    BigInteger pow(word_t e) const
    {
        if (e == 0)
            return BigInteger(1);
        BigInteger ret = *this;
        for (int mask = (1 << __lg(e)) >> 1; mask; mask >>= 1)
        {
            ret = ret * ret;
            if (mask & e)
                ret = ret * (*this);
        }
        return ret;
    }
};

constexpr u64 ten_pow(int e, u64 x = 1)
{
    return e <= 0 ? x : ten_pow(e - 1, x * 10);
}

template <int Digits>
class DecimalBigInteger : public BigInteger<ten_pow(Digits)>
{
    using BigInt = BigInteger<ten_pow(Digits)>;

  public:
    DecimalBigInteger(i64 a) : BigInt(a) {}
    DecimalBigInteger(const BigInt &b) : BigInt(b) {}
    void print() const
    {
        printf("%llu", BigInt::back());
        char str[Digits + 1] = {};
        for (int i = BigInt::size() - 2; i >= 0; --i)
        {
            auto a = (*this)[i];
            for (int j = 0; j < Digits; ++j)
            {
                str[Digits - 1 - j] = a % 10 + '0';
                a /= 10;
            }
            for (int j = 0; j < Digits; ++j)
                putchar(str[j]);
        }
        puts("");
    }
};
using BigInt = DecimalBigInteger<base_tens>;

void solve()
{
    const int N_MAX = 60010;
    int N;
    char a[N_MAX], b[N_MAX];
    scanf("%d %s %s", &N, a, b);
    BigInt A(a), B(b);
    BigInt C = A * B;
    C.print();
}

int main()
{
    solve();
    return 0;
}
