#include <bits/stdc++.h> 
using namespace std;
template<typename T>
void print(const T &t)
{
    cout << t << ' ';
}
template <typename... Args>
void f(Args ...args)
{
    ;
}
template<typename T,typename ...Args>
void print(const T &t,Args ...args)
{
    print(t);
    print(forward<Args>(args)...); //注意std::forward本身就是接受参数包，返回参数包的，所以调用它(括号内)不需解包,使用它的结果(括号外)需要解包
}
int main()
{
    print(1, 2, 3);
}
