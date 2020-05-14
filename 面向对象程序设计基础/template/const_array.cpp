#include <iostream>
using namespace std;

template <int N>
struct Test{
};
template <int ...Args>
struct Array
{
    //const static -> CE,static 非整形对象不能类内赋值
    //constexpr static -> OK
    constexpr static int arr[sizeof...(Args)] = {Args...};
};

//没用的,不可能的
//模板元与普通变量永远是两个世界的
/*template <int *Out,int N>
struct Fac
{
    const static int value = Out[N] = N * Fac<Out, N - 1>::value;
};
template <int *Out>
struct Fac<Out,0>
{
    const static int value = Out[0] = 1;
};
int x[100];
Fac<x, 10> f;*/
int main()
{
    
}