#include <bits/stdc++.h>
using namespace std;
//constexpr int *a = new int[50];
//c++14下才能工作
/*template<typename T,size_t N>
constexpr T sum(array<T,N> arr)
{
    static_assert(N>0,"length must be positive");
    auto x = get<0>(arr);
    for (int i = 1; i < N;++i)
        x += get<i>(arr);
    return x;
}*/
//constexpr int x = f(40);

template <typename... Args>
struct Sum;

//基本定义
template <typename First, typename... Rest>
struct Sum<First, Rest...>
{
    enum
    {
        value = Sum<First>::value + Sum<Rest...>::value
    };
};

//递归终止
template <typename Last>
struct Sum<Last>
{
    enum
    {
        value = sizeof(Last)
    };
};

/*template <typename,int... Agrs>
struct Mul;
template <int x, int... Agrs>
struct Mul
{
    const static int value = x * Mul<Args...>::value;
};
template <>
struct Mul<0>
{
    const static int value = 1;
};*/
/*template <typename T, size_t N, size_t I = 0>
T sum(const array<T, N> &arr)
{
//if (I >= N)
// return 0;
#if I < N
    return get<I>(arr) + sum<T, N, I + 1>(arr);
#else
    return 0;
#endif
}*/
template <typename T, size_t N>
T sum(const array<T, N> &arr)
{
    auto ret = get<0>(arr) - get<0>(arr);
    return ret;
}
int main()
{
    constexpr int x = sum(array<int, 5>{1, 2, 3, 4, 5});
    int a[x];
}
