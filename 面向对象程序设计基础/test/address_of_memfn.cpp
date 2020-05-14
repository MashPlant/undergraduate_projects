#include <bits/stdc++.h>
using namespace std;

struct A
{
    void f1() { cout << "f1\n"; }
    void f2() { cout << "f2\n"; }
    void f3() { cout << "f3\n"; }

  private:
    void f4() { cout << "f4\n"; }
};
int main()
{
    A a;
    auto fp1 = &A::f1;
    auto fp2 = &A::f2;
    auto fp3 = &A::f3;
    int p1 = (unsigned)*((unsigned char *)(void *)(fp1) + 15);
    int p2 = (unsigned)*((unsigned char *)(void *)(fp2) + 15);
    int p3 = (unsigned)*((unsigned char *)(void *)(fp3) + 15);
    cout << p1 << ' ' << p2 << ' ' << p3 << endl;
    for (int i = 0; i < sizeof fp1; ++i)
        cout << hex << (unsigned)*((unsigned char *)(void *)(fp1) + i) << ' ';
    cout << endl;
    for (int i = 0; i < sizeof fp2; ++i)
        cout << hex << (unsigned)*((unsigned char *)(void *)(fp2) + i) << ' ';
    cout << endl;
    for (int i = 0; i < sizeof fp3; ++i)
        cout << hex << (unsigned)*((unsigned char *)(void *)(fp3) + i) << ' ';
    cout<< endl;
    //(a.*fp3)();
}
