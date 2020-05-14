#include <bits/stdc++.h> 
using namespace std;

struct B 
{
    int x=42;
};
struct A 
{
    //B b;
    //B *operator->(){return &b;};
    int *b=new int(100);
    int *operator->{return b};
};
int main()
{
    A a;
    cout<<a->b;
    //cout<<a->x<<endl;
    //cout<<(a.operator->())->x;
}
