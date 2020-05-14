#include <bits/stdc++.h> 
using namespace std;
const int ci = 10;
/*
const_cast只能cast掉那些"自以为是"的const 指针/引用
真正的const是不能修改的
*/
int main()
{
    //int *pi = const_cast<int *>(&ci);
    //*pi = 5; //boom

    int i = 10;
    const int *pi = &i;
    *const_cast<int *>(pi)=5;
    cout << i << endl; //OK, i = 5

    const int &ri = i;
    const_cast<int &>(ri) = 7;
    cout << i; //OK, i = 7
}
