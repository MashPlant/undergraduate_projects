#include <bits/stdc++.h> 
using namespace std;

struct F
{
    int operator()() const 
    {
        return 42;
    }
};
int main()
{
    cout<<is_invocable<F>::value;
}
