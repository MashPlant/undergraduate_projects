#include <bits/stdc++.h> 
using namespace std;

void func(int &&i)
{
    printf("%s\n","rvalue ref");
}
void func(const int &i)
{
    printf("%s\n", "const lvalue ref");
}

void func(int &i)
{
    printf("%s\n", "lvalue ref");
}

template<typename T>
void call(T &&t)
{
    func(std::forward<T>(t));
}
int main()
{
    call(1);
    int i;
    call(i);
    const int &ii = i;
    call(ii);
}
