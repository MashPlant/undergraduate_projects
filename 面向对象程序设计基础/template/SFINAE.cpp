#include <bits/stdc++.h>
using namespace std;
//Substitution Failure Is Not An Error
template <typename T, typename = string>
struct HasToString : public false_type
{
};

template <typename T>
struct HasToString<T, decltype(declval<T>().toString())> : public true_type
{
};
struct A
{
};
struct B
{
    string toString() { return "B"; }
};
#define call(a, f) a.f()
#define conn(a, b) a##b
//#define ToChar(x) #@x CE
#define tostr(a) #a


int main()
{
    //cout << HasToString<A>::value << endl; //对于A,很显然只能匹配通例,从而是false
    //cout << HasToString<B>::value << endl;
    //对于B,由于默认参数string恰好就是decltype(declval<T>().toString()),故而匹配特例
    //cout<<call(B(), toString);
}
