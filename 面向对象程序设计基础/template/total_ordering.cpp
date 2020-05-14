#include <iostream>
using namespace std;

template <typename K>
struct TotalOrdering
{
    friend bool operator!=(const K &lhs, const K &rhs) { return !(lhs == rhs); }
    friend bool operator>(const K &lhs, const K &rhs) { return !(lhs == rhs) && !(lhs < rhs); }
};

struct A : TotalOrdering<A>
{
    int a = 0;
    A(int a = 0) : a(a) {}
    bool operator==(const A &rhs) const { return a == rhs.a; }
    bool operator<(const A &rhs) const { return a < rhs.a; }
};
//TotalOrdering<A> dummy; 看起来似乎没用?并没有实例化TotalOrdering中的函数
int main()
{
    A a1 = 1, a2 = 2;
    cout << boolalpha << (a1 == a2) << ' ' << (a1 < a2) << ' ' << (a1 != a2) << ' ' << (a1 > a2);
}