//
// Created by mashplant on 18-9-9.
//

#ifndef WEEK2LAB3_WORKER_HPP
#define WEEK2LAB3_WORKER_HPP

#include <QThread>
#include <functional>

template<typename R>
struct Worker : QThread
{
public:
    std::function<R(void)> work;
    R r;

    template<typename F, typename ...Args>
    Worker(F f, Args &&...args):
            work(std::bind(f, std::forward<Args>(args)...))
    {}

    R get() const
    { return r; }

protected:
    void run() override
    {
        r = work();
    }
};

#endif //WEEK2LAB3_WORKER_HPP
