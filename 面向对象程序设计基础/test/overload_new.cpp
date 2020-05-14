#include <cstdio>
#include <iostream>
#include <string>
//using namespace std;

int *pool = reinterpret_cast<int *>(malloc(1024));
int cur = 0;
/*void *operator new(size_t sz)
{
    return pool + cur++;
}
void *operator new(size_t sz, const std::string &myarg)
{
    std::cout << myarg << std::endl;
    return pool + cur++;
}
void operator delete(void *ptr) noexcept
{
    std::cout << "call my delete, doing nothing\n";
}
*/
struct A
{
    A() { std::cout << "A ctor\n"; }
    ~A() { std::cout << "A dtor\n"; }
    void *operator new (size_t sz)
    {
        std::cout << "call A's new\n";
        return ::operator new(sz);
    }
    void operator delete(void * ptr)
    {
        std::cout << "call A's delete\n";
        ::operator delete(reinterpret_cast<A *>(ptr));
    }
};

int main()
{
    
    /*int *p = new int;
    *p = 1;
    std::cout << *p << std::endl;
    std::cout << p - pool << std::endl;
    delete p;

    int *p2 = new("hello") int;
    *p2 = 2;
    std::cout << *p2 << std::endl;
    std::cout << p2 - pool << std::endl;
    delete p2;*/

    A *pa = new A;
    delete pa;
}
