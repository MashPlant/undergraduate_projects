#include <bits/stdc++.h>
using namespace std;

struct B
{
    void foo() { cout << "B foo()\n"; }
    void foo(int) { cout << "B foo(int)\n"; }
};
struct A : public B
{
  private:
    using B::foo;
    //using B::bar; //CE 

  public:
    void foo() { cout << "A foo()\n"; }
};
template<typename Base>
struct D : public Base
{
    private:
      //using Base::bar; //CE
};
int main()
{
    
}
