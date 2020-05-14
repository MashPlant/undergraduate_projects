#include <random>
#include <iostream>
#include "Timer.h"
#include "DpLIS.h"
#include "BfLIS.h"

const int N = 1000;
int main()
{
    std::mt19937 mt;
    std::uniform_int_distribution<> dis(0, N);

    std::vector<int> input(N);
    for (int &i : input)
        i = dis(mt);

    Timer t;

    t.setStrategy(new DpLIS);
    double t1 = t.getRuntime(input);
    t.setStrategy(new BfLIS);
    double t2 = t.getRuntime(input);
    std::cout << "Runtime_bf = " << t2 << '\n'
              << "Runtime_dp = " << t1 << '\n'
              << "improvement(Runtime_bf/Runtime_dp) = " << t2 / t1 << std::endl;
}