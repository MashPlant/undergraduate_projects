#include <iostream>

using namespace std;

struct A
{
    int a;
    A()
    {
        cout<<"default construct"<<endl;
    }
    A(int _a):a(_a)
    {
        cout<<"normal construct"<<endl;
    }
    A& operator=(const A &_A) //operator=() 1.const A& 2.A 3.A&& 12,13不能共存,23能
    {
        cout << "assgin construct";
        a = _A.a;
    }
    A(const A &_A):a(_A.a)
    {
        cout<<"copy construct"<<endl;
    }
    A &operator=(A &&A1) noexcept
    {
        cout << "move assign\n";
        a = A1.a;
        return *this;
    }
    A (A &&A1) noexcept
    {
        cout << "move construct\n";
        a = A1.a;
    }
};
//注意 A a = xxx表面上看起来是赋值,其实是构造
//至于A a = A(1),也许能优化成A a(1),但无论如何都是构造
int main()
{
    A b(1);
    A a;

    a = std::move(b);
}