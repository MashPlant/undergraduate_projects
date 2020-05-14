#include <iostream>
#include <string>
using std::cout;
using std::string;

using u64 = unsigned long long;
const u64 P = 19260817;
string flip(string s)
{
    for (int i = 0; i < s.size(); ++i)
        s[i] = '0' + '1' - s[i];
    return s;
}
u64 hash(const string &s)
{
    u64 h = 0;
    for (int i = 0; i < s.size(); ++i)
        h = h * P + s[i];
    return h;
}
u64 inv_hash(const string &s)
{
    u64 h = 0;
    for (int i = s.size() - 1; i >= 0; --i)
        h = h * P + s[i];
    return h;
}
int main()
{
    string s = "0";
    for (int i = 0; i < 12; ++i)
        s = s + flip(s);
    string flip = ::flip(s);
    cout << (s == flip) << '\n';
    cout << hash(s) << '\n';
    cout << hash(flip) << '\n';
    cout << inv_hash(s) << '\n';
    cout << inv_hash(flip) << '\n';
    return 0;
}