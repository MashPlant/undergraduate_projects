#include <any>
#include <iostream>
using namespace std;
struct A
{
    int x = -1;
    A() { cout << "A" << endl; }
};
int main()
{
    auto a1 = any(A());
    auto a2 = a1;
    auto &ref = any_cast<A &>(a2);
    ref.x = 1;
    cout << any_cast<A &>(a1).x << endl;
    cout << any_cast<A &>(a2).x << endl;
    //any_cast<A &>(a2).x = 1;
    //cout << (a1 == a2);
    //cout << any_cast<A &>(a1).x;
}
