#include <iostream>
using namespace std;
void f(int &x)
{
    cout << "call by ref\n";
}
void f(int &&x)
{
    cout << "call by r-ref\n";
}
int main()
{
    f(1);
    int x = 1;
    int &&y = move(x);
    f(y);
    f(move(y));
}