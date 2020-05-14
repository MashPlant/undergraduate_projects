#include <iostream>
struct A
{
    A() {std::cout<<"ctor";}
};
void f()
{
    static A a;
}

int main()
{
    std::cout << "test\n";
    f();
    f();
}