#include <iostream>
#include <string>
using namespace std;
using namespace string_view_literals;

string_view f()
{
    return "123456"sv;
}
//It is the programmer's responsibility to ensure that
//std::string_view does not outlive the pointed-to character array:
std::string_view good("a string literal");   // OK: "good" points to a static array
std::string_view bad("a temporary string"s); // "bad" holds a dangling pointer

int main()
{
    cout << good << endl;
    cout << bad << endl; //乱码 or RE
}