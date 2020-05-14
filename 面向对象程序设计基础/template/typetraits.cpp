#include <bits/stdc++.h>
using namespace std;
template <typename Iter>
void destruct(Iter where)
{
    typedef typename std::iterator_traits<Iter>::value_type value_type;
    //std::cout << typeid(value_type).name() << '\n';
    //std::cout << typeid(Iter).name();
    where->~value_type();
}
struct SimpleIterator
{
    typedef std::random_access_iterator_tag iterator_category;
    typedef int value_type;
    typedef ptrdiff_t difference_type;
    typedef int *pointer;
    typedef int &reference;
    int *pos;
    int &operator*() { return *pos; }
    int *operator->() { return &(operator*()); }
};
class ClassName
{
  public:
    int foo(int) { return 42; }
};

template <typename K,size_t N>
struct Array  //注意,合成的拷贝构造函数是默认复制数组元素的
{
    K arr[N];
};
struct Data 
{
    Array<char, 20> arr;
    int x;
    double d;
};

struct TestFunc 
{
    void foo(){}
    //void bar(){}
};
template <typename K>
void test(K k)
{
    using Foo = decltype(k.foo());
    //using Bar = decltype(k.bar()); //CE
}
int main()
{
    TestFunc tf;
    test(tf);
    /*cout << boolalpha;
    cout << is_nothrow_copy_assignable<string>::value << endl;
    cout << is_nothrow_copy_assignable<vector<int>>::value << endl;
    cout << is_nothrow_copy_assignable<int>::value << endl;
    cout << is_nothrow_copy_assignable<int[10]>::value << endl; //false
    cout << is_nothrow_copy_assignable<Array<int,10>>::value << endl; //true
    cout << is_nothrow_copy_assignable<Data>::value << endl; //true
    */
    /*int a = 0;
    int *p = &a;
    using Int = int;
    p->~Int(); //看来只是不能用int这个词而已
    //destruct(p);
    //typedef void func(SimpleIterator *);
    int (ClassName::*pmf)(int) = &ClassName::foo;
    auto pderef = &SimpleIterator::operator*;*/
}
