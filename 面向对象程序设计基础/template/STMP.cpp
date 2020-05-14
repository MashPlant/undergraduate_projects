#include <iostream>
using namespace std;

template <int N>
struct flag
{
    friend constexpr int adl_flag(flag<N>);
    constexpr operator int() { return N; }
};

template <int N>
struct write
{
    friend constexpr int adl_flag(flag<N>) { return N; }
    static constexpr int value = N;
};

template <int N, int = adl_flag(flag<N>{})>
constexpr int read(int, flag<N>, int R = read(0, flag<N + 1>{}))
{
    return R;
}

template <int N>
constexpr int read(float, flag<N>)
{
    return N;
}

template <int N = 0>
constexpr int counter(int R = write<read(0, flag<0>{}) + N>::value)
{
    return R;
}
int main()
{
    static_assert(counter() != counter(), "Your compiler is mad at you");
    cout << "hello\n";
}