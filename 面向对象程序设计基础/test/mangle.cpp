#include <cxxabi.h>
#include <exception>
#include <iostream>
#include <typeinfo>
struct empty
{
};

template <typename T, int N>
struct bar
{
};

//void f(int){ }
void f(int,int = 0){ }
template <typename T>
void getName(T t)
{
    int status;
    char *realname;
    const std::type_info &ti = typeid(t);
    realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
    std::cout << ti.name() << "\t=> " << realname << std::endl;
    free(realname);
}
int main()
{
    //freopen("out.txt","w",stdout);
    getName(bar<empty, 17>());
    using fp1 = void (*)(int,int);
    getName<fp1>(f);
    return 0;
}