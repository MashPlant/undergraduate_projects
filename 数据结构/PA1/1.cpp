#include <cstdio>
#include <cstring>
#include <cstdlib>

#define assert(x)

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

typedef int i32;
typedef unsigned u32;
typedef unsigned long long u64;

struct BigInt
{
    const static u32 exp = 9;
    const static u32 mod = 1000000000;
    u32 *val;
    mutable u32 len, sgn;

    void alloc(u32 size)
    {
        len = sgn = 0;
        val = (u32 *)calloc(size, sizeof(u32));
    }

    void copy_to(BigInt &dst) const
    {
        dst.len = len, dst.sgn = sgn;
        memcpy(dst.val, val, sizeof(u32) * len);
    }

    void trim()
    {
        while (len && !val[len - 1])
            --len;
        if (len == 0)
            sgn = 0;
    }

    u32 &operator[](u32 i)
    {
        return val[i];
    }

    u32 operator[](u32 i) const
    {
        return val[i];
    }

    void _add(const BigInt &x)
    {
        u32 carry = 0;
        len = max(len, x.len);
        for (u32 i = 0; i < x.len; ++i)
            val[i] += x[i];
        for (u32 i = 0; i < len; ++i)
        {
            val[i] += carry, carry = 0;
            if (val[i] >= mod)
                val[i] -= mod, carry = 1;
        }
        if (carry)
            val[len++] = carry;
        trim();
    }

    void _sub(const BigInt &x)
    {
        u32 carry = 0;
        len = max(len, x.len);
        for (u32 i = 0; i < x.len; ++i)
            val[i] -= x[i];
        for (u32 i = 0; i < len; ++i)
        {
            val[i] += carry, carry = 0;
            if (val[i] & 0x80000000)
                val[i] += mod, carry = -1;
        }
        if (carry)
        {
            carry = 0;
            sgn ^= 1;
            for (u32 i = 0; i < len; ++i)
                if (carry || val[i])
                    val[i] = mod + carry - val[i], carry = -1;
        }
        trim();
    }

    void add(const BigInt &x)
    {
        (sgn ^ x.sgn) ? _sub(x) : _add(x);
    }

    void sub(const BigInt &x)
    {
        (sgn ^ x.sgn) ? _add(x) : _sub(x);
    }

    void read(const char *s)
    {
        sgn = len = 0;
        i32 bound = 0, pos;
        if (s[0] == '-')
            sgn = bound = 1;
        u64 cur = 0, pow = 1;
        for (pos = strlen(s) - 1; pos + 1 >= exp + bound; pos -= exp, val[len++] = cur, cur = 0, pow = 1)
            for (i32 i = pos; i + exp > pos; --i)
                cur += (s[i] - '0') * pow, pow *= 10;
        for (cur = 0, pow = 1; pos >= bound; --pos)
            cur += (s[pos] - '0') * pow, pow *= 10;
        if (cur)
            val[len++] = cur;
    }

    void print() const
    {
        if (len)
        {
            if (sgn)
                putchar('-');
            printf("%u", val[len - 1]);
            for (u32 i = len - 2; ~i; --i)
                printf("%0*u", exp, val[i]);
        }
        else
            putchar('0');
        puts("");
    }
};

void b_mul(BigInt &dst, const BigInt &x, const BigInt &y)
{
    memset(dst.val, 0, sizeof(u32) * (x.len + y.len));
    dst.len = x.len + y.len, dst.sgn = x.sgn ^ y.sgn;
    for (u32 i = 0; i < x.len; ++i)
    {
        u64 carry = 0, f = x[i];
        u32 *pd = dst.val + i;
        u32 *py = y.val, *end = y.val + y.len;
        while (py < end)
        {
            carry += *pd + *py++ * f;
            *pd++ = carry % BigInt::mod;
            carry /= BigInt::mod;
        }
        if (carry)
            *pd += carry;
    }
    dst.trim();
}

void k_mul_impl(BigInt &dst, const BigInt &x, const BigInt &y)
{
    if (x.len < 32)
        return b_mul(dst, x, y);
    memset(dst.val, 0, x.len * 2 * sizeof(u32));
    u32 len = x.len, h = len >> 1, alloc_len = len + 4;
    // (a * [h] + b) * (c * [h] + d)
    BigInt a{x.val + h, len - h}, b{x.val, h}, c{y.val + h, len - h}, d{y.val, h};
    BigInt axc{(u32 *)__builtin_alloca(alloc_len * sizeof(u32))},
        bxd{(u32 *)__builtin_alloca(alloc_len * sizeof(u32))},
        a_b{(u32 *)__builtin_alloca(alloc_len * sizeof(u32))},
        c_d{(u32 *)__builtin_alloca(alloc_len * sizeof(u32))},
        a_bxc_d{(u32 *)__builtin_alloca(alloc_len * sizeof(u32))};
    a.copy_to(a_b), a_b.add(b);
    c.copy_to(c_d), c_d.add(d);
    if (a_b.len > c_d.len)
        memset(c_d.val + c_d.len, 0, sizeof(u32) * (a_b.len - c_d.len)), c_d.len = a_b.len;
    else if (a_b.len < c_d.len)
        memset(a_b.val + a_b.len, 0, sizeof(u32) * (c_d.len - a_b.len)), a_b.len = c_d.len;
    k_mul_impl(axc, a, c);
    k_mul_impl(bxd, b, d);
    k_mul_impl(a_bxc_d, a_b, c_d);
    axc.copy_to(dst);
    memmove(dst.val + h, dst.val, sizeof(u32) * dst.len), memset(dst.val, 0, sizeof(u32) * h), dst.len += h;
    a_bxc_d.sub(axc), a_bxc_d.sub(bxd);
    dst.add(a_bxc_d);
    memmove(dst.val + h, dst.val, sizeof(u32) * dst.len), memset(dst.val, 0, sizeof(u32) * h), dst.len += h;
    dst.add(bxd);
}

void k_mul(BigInt &dst, const BigInt &x, const BigInt &y)
{
    const BigInt *xp = &x, *yp = &y;
    if (xp->len > yp->len)
        swap(xp, yp);
    u32 old_len = xp->len;
    memset(xp->val + xp->len, 0, sizeof(u32) * (yp->len - xp->len)), xp->len = yp->len;
    k_mul_impl(dst, x, y);
    xp->len = old_len;
}

const int N = 5050;
char a_[N], b_[N];

int main()
{
    setvbuf(stdin, new char[1 << 22], _IOFBF, 1 << 22);
    setvbuf(stdout, new char[1 << 22], _IOFBF, 1 << 22);
    int t;
    scanf("%d", &t);
    BigInt a, b, r;
    a.alloc(N / 9 + 10), b.alloc(N / 9 + 10), r.alloc(2 * (N / 9 + 10));
    while (t--)
    {
        scanf("%s%s", a_, b_);
        a.read(a_), b.read(b_);
        k_mul(r, a, b);
        r.print();
    }
    free(a.val), free(b.val), free(r.val);
}