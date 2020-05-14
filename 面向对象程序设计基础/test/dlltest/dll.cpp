#include <stdio.h>
//g++ -shared  -o dll.dll dll.cpp 
extern "C"
{
__declspec(dllexport) void hello(const char *msg);
}

void hello(const char *msg)
{
    printf("Hello %s\n", msg);
}