#include <stdio.h>
//g++ -L. -ldll -o test.exe test.cpp 
extern "C"
{
    __declspec(dllimport) void hello(const char *msg);
    __declspec(dllimport) void helloc(const char *msg);
}

int main(int argc, char *argv[])
{
    helloc("Fucking!");
    return 0;
}