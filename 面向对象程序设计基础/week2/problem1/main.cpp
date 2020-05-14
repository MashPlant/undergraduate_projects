#include "Func.h"
#include <ctime>
#include <iostream>
#include <random>
#include <vector>
template <typename Func, typename... Args>
clock_t getRunTime(Func f, Args &&... args)
{
    clock_t beg = clock();
    for (int i = 0; i < 10000; ++i)
        for (int j = 0; j < 10000; ++j)
            f(std::forward<Args>(args)...);
    return clock() - beg;
}
std::mt19937 mt(time(nullptr));
std::uniform_real_distribution<> gen(1.0, 2.0);
int main()
{
    std::vector<double> a(10);
    for (int i = 0; i < a.size(); ++i)
        a[i] = gen(mt);
    double x = gen(mt);
    clock_t tm[4];
    std::cout << "polynomial time" << std::endl;
    std::cout << "QJS : " << (tm[0] = getRunTime(poly, a, x)) << std::endl;
    std::cout << "Brute : " << (tm[1] = getRunTime(polyBrute, a, x)) << std::endl;
    std::cout << "posynomial time" << std::endl;
    std::cout << "QJS : " << (tm[2] = getRunTime(posy, a, x)) << std::endl;
    std::cout << "Brute : " << (tm[3] = getRunTime(posyBrute, a, x)) << std::endl;
    std::cout << "polynomial time saved " << 1.0 * (tm[1] - tm[0]) / tm[1] << "%" << std::endl;
    std::cout << "posynomial time saved " << 1.0 * (tm[3] - tm[2]) / tm[3] << "%" << std::endl;
}