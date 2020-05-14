#include <iostream>
#include <typeinfo>
#include <map>
#include <string>
using namespace std;
struct Base 
{
    virtual void func(){}
};
struct Derive1 : Base 
{
};
struct Derive2 : Base
{
};
void casttest()
{
    Base *pd1 = new Derive1;
    Base *pb = new Base;
    if (dynamic_cast<Base *>(pd1))
        cout << "pd1 can be cast to Base*" << endl;
    if (dynamic_cast<Base *>(pb))
        cout << "pb can be cast to Base*" << endl;
    Derive2 *pd2 = new Derive2;
    //if (dynamic_cast<Derive1*>(pd2)) 编译错误 pd is not polymorphic
}
int main()
{
    map<int, string> typeToStr;
    typeToStr[typeid(Base).hash_code()] = "Base";
    typeToStr[typeid(Derive1).hash_code()] = "Derive1";
    typeToStr[typeid(Derive2).hash_code()] = "Derive2";
    Base *ptr;
    //if (getchar()=='1')
    //    ptr=new Derive1;
    //else 
    ptr=new Derive2;
    cout << typeToStr[typeid(*ptr).hash_code()];
    /*
        当Base中定义了virtual void func(){}时输出Derive2
        没有定义虚函数时输出Base
    */
}