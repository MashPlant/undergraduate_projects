#pragma GCC optimize("O2")
#include <bits/stdc++.h>
inline void bar()
{
    for (int i = 0; i < 100; ++i)
        rand();
    printf("%p\n", __builtin_return_address(0));
}
inline void foo()
{
    for (int i = 0; i < 100; ++i)
        rand();
    printf("%p\n", __builtin_return_address(0));
    bar();
}

int fib(int x)
{
    if (x == 0 || x == 1)
        return 1;
    printf("%p\n", __builtin_return_address(0));
    return fib(x - 1) + fib(x - 2);
}
template <int N>
int staticFib(std::true_type)
{
    return staticFib<N - 1>(typename std::integral_constant < bool, N - 1 == 1 || N - 1 == 0 > ::type()) +
           staticFib<N - 2>(typename std::integral_constant < bool, N - 2 == 1 || N - 2 == 0 > ::type());
}

template<int N>
int staticFib(std::false_type)
{
    return 1;
}
int main()
{
    fib(5);
}