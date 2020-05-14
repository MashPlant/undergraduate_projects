#include <iostream>
#include <typeinfo>
using namespace std;

template <auto T>
struct A 
{
    A() { cout << typeid(T).name() << endl; }
};

int main()
{
    //A<int> a1;
    A<1> a2;
    A<'a'> a3;
    A<1ll> a4;
}