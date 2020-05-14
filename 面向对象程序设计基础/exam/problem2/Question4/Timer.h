#ifndef TIMER_H
#define TIMER_H

#include "AbstractLIS.h"
#include <memory>
#include <chrono>
#include <iostream>

class Timer
{
    std::unique_ptr<AbstractLIS> strategy;
    using ClockType = std::chrono::duration<double>;

  public:
    void setStrategy(AbstractLIS *strategy)
    {
        this->strategy.reset(strategy);
    }

    double getRuntime(const std::vector<int> &input)
    {
        auto beg = std::chrono::system_clock::now();
        std::vector<int> output;
        strategy->solve(input, output);
        return ClockType(std::chrono::system_clock::now() - beg).count();
    }
};

#endif // TIMER_H