#include <stdio.h>
//g++ -shared  -o dll.dll dll.cpp

void helloc(const char *msg)
{
    printf("Helloc %s\n", msg);
}