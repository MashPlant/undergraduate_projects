//
// Created by mashplant on 18-9-5.
//

#ifndef WEEK2LAB21_WORKER_HPP
#define WEEK2LAB21_WORKER_HPP

#include <QThread>
#include <functional>

struct Worker : QThread
{
    std::function<void(void)> work;

    // well, perfect forwarding is not implemented
    // which saved a lot of work
    template<typename F, typename ...Args>
    Worker(F f, Args ...args) : QThread(nullptr), work([f, args...] { f(args...); })
    {}

protected:
    void run() override
    {
        work();
    }
};

#endif //WEEK2LAB21_WORKER_HPP
