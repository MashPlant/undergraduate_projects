#include <bits/stdc++.h> 
using namespace std;

struct A 
{
    A(int) { cout << "int cons called\n"; }
    A() { cout << "default cons called\n"; }
    void foo() { cout << "foo called\n"; }
};
struct B 
{
    function<void(void)> operator->*(int)
    {
        return []() { cout << "hacked\n"; };
    }
};
int main()
{
    auto func = &A::foo; //成员函数不能隐式转换为指针,要显式取地址
    A a;
    (a.*func)(); //可以把.*和->*当成是独立的运算符;由于优先级, 必须加()
    //auto cons = &A::~A;

    B b;
    (b->*(1))();
}
