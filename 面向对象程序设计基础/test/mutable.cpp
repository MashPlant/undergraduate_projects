#include <iostream>
using namespace std;

struct A
{
    mutable int x = 0;
    int y = 0;
    void chx() const { ++x; }
    //void chy() const { ++y; }
};
int main()
{
    const A a;
    const_cast<A *>(&a)->y = 1; //UB,但正常运行
    cout << a.y << endl;

    const int x = 0;
    *const_cast<int *>(&x) = 1;
    cout << x << endl;
}
