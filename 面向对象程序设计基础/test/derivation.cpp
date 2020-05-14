#include <bits/stdc++.h>
using namespace std;

//测试子类的类型定义能否影响基类
struct Base
{
    typedef int T;
    T getValue() { return 0; }
    void func() { cout << "Base\n"; }
    virtual void vfunc() { cout << "vBase\n"; }
};
struct Der : public Base
{
    typedef long long T;
    void func() { cout << "Der\n"; }
    virtual void vfunc() { cout << "vDer\n"; }
};
//-----------------------------------------------
//测试三种访问权限
//注意,三种继承方式只影响派生类的用户的访问权限
//public,private,protected分别表示把基类的public和protected作为自己的xxx
class TestAccess
{
  protected:
    int pro = 1;

  private:
    int pri = 2;

  public:
    int pub = 3;
};
class DerTestAccess : public TestAccess
{
    void f()
    {
        cout << pub;
        cout << pro;
        //cout << pri; CE
    }
};
void foo(DerTestAccess d)
{
    cout << d.pub;
    //cout << d.pro; CE
    //cout << d.pri; CE
}
//--------------------------------------------------
//测试嵌套类的初始化情况
class Outer
{
  protected:
    class Nested
    {
        int a = printf("a inited\n");
    };
};

class DerOuter : public Outer
{
    Nested *n = new Nested; //我感觉在VS上这样写没有触发a的赋值，但是这里没有问题
};
//---------------------------------------
//测试初始化顺序是否有影响
struct BaseSource
{
    struct Nested 
    {
        BaseSource *sour;
        Nested(BaseSource *sour):sour(sour){}
    };
    Nested *n;
    virtual void foo() = 0;
    BaseSource() { cout << "base inited\n"; }
};

struct DerSource :BaseSource
{
    int *dptr;
    void foo(){}
    DerSource():BaseSource()
    {
        
        dptr = new int[10];
        n = new Nested(this);
        //bptr = new int[10];
    }
};
int main()
{
    DerSource d;
    //DerOuter d;
}
