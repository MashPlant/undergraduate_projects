#include <iostream>
#include <string>
using namespace std;
void foo(int)
{
    cout << "outter\n";

}
void foo(string); 
//根据C++ Primer,友元函数即使在类内直接定义,也不能直接使用
//按照标准,仍然必须提供这个函数的前置声明,但是有些编译器没有这么严格
struct A
{
    friend void foo(int);
    friend void foo(string)
    {
        cout << "friend foo\n";
    }
    void foo() { cout << "inner\n"; }
    void bar()
    {
        foo();
        ::foo(1);
        ::foo("123");
    }
};


int main()
{
    A a;
    a.bar();
    
}