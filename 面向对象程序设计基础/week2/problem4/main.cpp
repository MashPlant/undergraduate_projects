#include "Brute.h"
#include "KMP.h"
#include <ctime>
#include <iostream>
#include <random>
#include <string>
std::string pat(1e4, '\0');
std::string str;
int main()
{
    srand(time(nullptr));
    for (int i = 0; i < pat.size(); ++i)
        pat[i] = (rand() % 2) + '0';
    for (int i = 0; i < 1e5; ++i)
        if (rand() % 10)
            str += (rand() % 2) + '0';
        else
            str += pat;

    clock_t beg = clock(), tm[3];
    KMP kmp(pat);
    auto v1 = kmp.findIn(str);
    std::cout << "KMP : " << (tm[0] = clock() - beg) << std::endl;

    beg = clock();
    Brute bru(pat);
    auto v2 = bru.findIn(str);
    std::cout << "Brute : " << (tm[1] = clock() - beg) << std::endl;

    beg = clock();
    auto v3 = stlfind(str, pat);
    std::cout << "stl find : " << (tm[2] = clock() - beg) << std::endl;
    std::cout << "correct? " << std::boolalpha << (v1 == v2 && v2 == v3) << std::endl;

    std::cout << "improvement(stl find as base line)" << std::endl;
    std::cout << "KMP : " << 1.0 * (tm[2] - tm[0]) / tm[2] << std::endl;
    std::cout << "Brute : " << 1.0 * (tm[2] - tm[1]) / tm[2] << std::endl;
}