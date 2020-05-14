#include <iostream>
using namespace std;

struct A
{
    int x;
    A(int x) : x(x){}
};
int main()
{
    A *p = new auto(A(1));
    //A *p = new auto(1); CE,new auto(1)被推断为int *
    //总而言之:new auto语法毫无用处
}