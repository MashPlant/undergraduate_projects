#include <bits/stdc++.h>
using namespace std;

/*
array<array<int, 10>, 10> arr; //array是在栈上创建的
auto x = sizeof(arr);
cout << x;
*/
template <bool, typename OnTrue, typename OnFalse>
struct IF
{
    typedef OnFalse type;
};
template <typename OnTrue, typename OnFalse>
struct IF<true, OnTrue, OnFalse>
{
    typedef OnTrue type;
};

template <size_t N, size_t C>
constexpr int Sum(const array<int, N> &arr, true_type)
{
    return 0;
}

template <size_t N, size_t C = 0>
constexpr int Sum(const array<int, N> &arr, false_type)
{
    return get<C>(arr) + Sum<N, C + 1>(arr, typename IF<C + 1 == N, true_type, false_type>::type());
}

template <size_t N>
constexpr int Sum(const array<int, N> &arr)
{
    return Sum<N, 0>(arr, typename IF<0 == N, true_type, false_type>::type());
}
template <int ...Args>
constexpr int SumSeq()
{
    //C++14 特性
    constexpr array<int, sizeof...(Args)> tmp({Args...});
    return Sum(tmp);
}
/*函数模板不允许偏特化
template <int N>
constexpr int Sum<N,N>(const array<int, N> &arr)
{
    return 0;
}
*/
template<int N>
struct Test //测试N是否真的为constexpr,比开数组更可靠
{
};

template <int Where, typename K, K... Args>
constexpr K fromArgs()
{
    //这是C++14特性，我不知道在C++11下怎么写
    constexpr std::array<K, sizeof...(Args)> tmp({Args...});
    return std::get<Where>(tmp);
}

template <int Pre, int... Nxt>
struct Layer
{
    //w,b都是用来生成本层的，也就是说w是由上层连出来的
    //Nxt的第0个元素是本层的大小
    const static int X = fromArgs<0, int, Nxt...>();
    Test<X> t;
    Test<(fromArgs<0, int, 1,2,3>())> tt;
    //Layer *prev = nullptr;  //这里的指针表示的是一种"关系"，而非"资源"
    Layer<Nxt...> next;
};
template <int... Nxt>
struct Layer<0, Nxt...> //特例化，第0层
{
    //原则上来说，用不到的东西都可以没有，即使开销再小
    //所以这里没有prev(注意:不存在!=nullptr)
    Layer<Nxt...> next;
};
template <int Pre>
struct Layer<Pre>
{
}; //特例化，最后一层的next
Layer<0, 2, 2> first;
int main()
{
    //constexpr array<int, 3ull> arr{1, 2, 3};
    //Test<Sum(arr)> t;
    //cout << SumSeq<1, 2, 3, 4, 5, 6>();
}
