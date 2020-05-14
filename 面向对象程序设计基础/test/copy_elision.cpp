#include <iostream>

struct A
{
    int a;
    A()
    {
        std::cout << "default construct" << std::endl;
    }
    A(int _a) : a(_a)
    {
        std::cout << "normal construct" << std::endl;
    }

    A &operator=(const A &) = delete;
    A(const A &) = delete;
    A &operator=(A &&) = delete;
    A(A &&) = delete;
};

void f(A a)
{
    return;
}
int main()
{
    A a;
    f(a);
}