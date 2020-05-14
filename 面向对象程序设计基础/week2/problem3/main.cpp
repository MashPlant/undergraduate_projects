#include "Sorter.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>

const int maxn = 2e7;
int input[maxn];
int main()
{
    std::mt19937 mt(time(nullptr));
    std::uniform_int_distribution<> gen(0, INT_MAX);
    clock_t beg, tm[4];

    for (int i = 0; i < maxn; ++i)
        input[i] = gen(mt);
    beg = clock();
    std::sort(input, input + maxn);
    std::cout << "std::sort : " << (tm[0] = clock() - beg) << std::endl;
    std::cout << std::boolalpha << "correct? " << std::is_sorted(input, input + maxn) << std::endl;

    for (int i = 0; i < maxn; ++i)
        input[i] = gen(mt);
    beg = clock();
    Sorter::sort(input, input + maxn);
    std::cout << "my implement of std::sort : " << (tm[1] = clock() - beg) << std::endl;
    std::cout << std::boolalpha << "correct? " << std::is_sorted(input, input + maxn) << std::endl;

    for (int i = 0; i < maxn; ++i)
        input[i] = gen(mt);
    beg = clock();
    Sorter::radixSort(input, input + maxn);
    std::cout << "radix sort : " << (tm[2] = clock() - beg) << std::endl;
    std::cout << std::boolalpha << "correct? " << std::is_sorted(input, input + maxn) << std::endl;

    for (int i = 0; i < maxn; ++i)
        input[i] = gen(mt);
    beg = clock();
    Sorter::heapSort(input, input + maxn);
    std::cout << "heap sort : " << (tm[3] = clock() - beg) << std::endl;
    std::cout << std::boolalpha << "correct? " << std::is_sorted(input, input + maxn) << std::endl;

    std::cout << "improvment(std::sort as baseline)" << std::endl;
    std::cout << "my implement of std::sort : " << 1.0 * (tm[0] - tm[1]) / tm[0] << std::endl;
    std::cout << "radix sort : " << 1.0 * (tm[0] - tm[2]) / tm[0] << std::endl;
    std::cout << "heap sort : " << 1.0 * (tm[0] - tm[3]) / tm[0] << " (slower)" << std::endl;
}