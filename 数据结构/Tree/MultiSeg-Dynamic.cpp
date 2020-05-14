// #include <cstdio>
// #include <cstring>
// const int N = 1 << 20;

// namespace array
// {
// typedef int T;
// const int block = 16;
// union {
//     T val[block];
//     int ch[block];
// } buf[N * 6];
// int root[N], buf_sz, remain;

// template <int Len = N>
// void build(int &rt)
// {
//     rt = ++buf_sz;
//     for (int i = 0; i < block; ++i)
//         build<Len / block>(buf[rt].ch[i]);
// }

// template <>
// void build<block>(int &rt)
// {
//     rt = ++buf_sz;
//     for (int i = 0; i < block && remain; ++i, --remain)
//         buf[rt].val[i] = io;
// }

// template <int Len = N>
// int get(int rt, int pos)
// {
//     constexpr int ch_block = Len / block;
//     int which = pos / ch_block;
//     return get<ch_block>(buf[rt].ch[which], pos - which * ch_block);
// }

// template <>
// int get<block>(int rt, int pos)
// {
//     return buf[rt].val[pos];
// }

// template <int Len = N>
// int modify(int rt, int pos, int val)
// {
//     constexpr int ch_block = Len / block;
//     int ret = ++buf_sz, which = pos / ch_block;
//     buf[ret] = buf[rt];
//     buf[ret].ch[which] = modify<Len / block>(buf[rt].ch[which], pos - which * ch_block, val);
//     return ret;
// }

// template <>
// int modify<block>(int rt, int pos, int val)
// {
//     int ret = ++buf_sz;
//     buf[ret] = buf[rt];
//     buf[ret].val[pos] = val;
//     return ret;
// }
// } // namespace array
#define Inline __attribute__((always_inline)) inline
const int N = 100000 + 5;
namespace type
{
template <bool B>
struct Bool
{
};
using True = Bool<true>;
using False = Bool<false>;
} // namespace type
namespace seg
{
using namespace type;
using u32 = unsigned;
const u32 U = 1 << 20;
const u32 B = 16;
struct
{
    u32 val[B];
    u32 ch[B];
} buf[N * 6];
u32 root[N], treesz;

// user defined
const u32 Init = 0;
void f(u32 &a, u32 b) { a += b; }

template <u32 L = U>
void modify(u32 &rt, u32 pos, u32 val)
{
    constexpr u32 Ch = L / B;
    if (rt == 0)
        rt = ++treesz;
    u32 which = pos / Ch;
    f(buf[rt].val[which], val);
    modify<Ch>(buf[rt].ch[which], pos - which * Ch, val);
}
template <>
void modify<B>(u32 &rt, u32 pos, u32 val)
{
    if (rt == 0)
        rt = ++treesz;
    f(buf[rt].val[pos], val);
}
template <u32 L, u32 R, u32 Now>
Inline void visit(u32 rt, u32 ql, u32 qr, True)
{
    constexpr u32 Len = R - L + 1;
    constexpr u32 Ch = Len / B;
    constexpr u32 Nl = L + Ch * Now, Nr = Nl + Ch;
    
}
template <u32 L, u32 R, u32 Now>
void visit(u32 rt, u32 ql, u32 qr, False) {}

template <u32 L = 1, u32 R = U>
u32 query(u32 rt, u32 ql, u32 qr)
{
    constexpr u32 Len = R - L + 1;
    constexpr u32 Ch = Len / B;
    u32 ret = Init;
    u32 ql_block = ql / Ch, qr_block = qr / Ch;

    return ret;
}

} // namespace seg
int main()
{
    // using namespace array;
    // int n = io, m = io;
    // remain = n;
    // build(root[0]);
    // for (int i = 1, v, op, pos; i <= m; ++i)
    // {
    //     v = io, op = io, pos = io - 1;
    //     if (op == 1)
    //         root[i] = modify(root[v], pos, io);
    //     else
    //         io.print(get(root[i] = root[v], pos));
    // }
}