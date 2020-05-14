#include <bits/stdc++.h>
using namespace std;

struct Base
{
    string name;
    virtual Base *create() = 0;
    virtual ~Base() {}
};

struct A : Base
{
    A() { name = "A"; }
    string afoo() { return "A"; }
    Base *create() override
    {
        return new A();
    }
};
struct B : Base
{
    B() { name = "B"; }
    string bfoo() { return "B"; }
    Base *create() override
    {
        return new B();
    }
};

int main()
{
    map<string, Base *> typeMap;
    typeMap["A"] = new A;
    typeMap["B"] = new B;
    string in;
    cin >> in;
    Base *base = typeMap[in]->create();
    //remove_reference<decltype(*base)>::type x; CE,不可能获取到运行时的对象实体
    cout << base->name << endl;
    cout << typeid(*base).name() << endl;
    if (A *a = dynamic_cast<A *>(base))
        cout << a->afoo();
    if (B *b = dynamic_cast<B *>(base))
        cout << b->bfoo();
}