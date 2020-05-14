#include <iostream>
#include <string>
using namespace std;
inline string operator""_x(long double u)
{
    return to_string(u) + "x";
}
inline string operator""_x(unsigned long long u)
{
    return to_string(u) + "x";
}

string operator""_s (const char *str,size_t len)
{
    return string(str);
}
int main()
{
    cout << "123456789"_s.find_first_not_of("1");
}