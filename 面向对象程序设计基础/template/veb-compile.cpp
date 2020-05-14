#include <bits/stdc++.h> 
using namespace std;

//假如我们发明了一种数据结构叫阶乘树
template <int N>
struct Node 
{
    Node<N - 1> ch[N];
};

template<>
struct Node<1>
{
    int x;
};
clock_t beg = clock();
Node<11> root;

template<int N>
void foo(Node<N> &x)
{
    for (int i = 0; i < N;++i)
        foo(x.ch[i]);
}
void foo(Node<1> &x)
{
    //cout << "x is a leaf,value is " << x.x << endl;
    x.x = 1;
}
int main()
{
    cout << clock() - beg << endl;
    beg = clock();
    foo(root); //如果不使用root,貌似就不会给它分配内存
    cout << clock() - beg << endl;
    getchar();
}
