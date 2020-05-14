//
// Created by mashplant on 18-9-5.
//

#include <cstring>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include "Worker.hpp"

void sort(int *first, int *last, int thread_count)
{
    if (last - first <= 32)
        return void(std::__final_insertion_sort(first, last, __gnu_cxx::__ops::__iter_less_iter()));
    int *mid = std::__unguarded_partition_pivot(first, last, __gnu_cxx::__ops::__iter_less_iter());
    if (!thread_count)
        sort(first, mid, 0), sort(mid, last, 0);
    else
    {
        double left_portion = 1.0 * (mid - first) / (last - first);
        int left_count = left_portion * thread_count, right_count = thread_count - left_count;
        Worker s1(sort, first, mid, std::max(left_count - 1, 0)), s2(sort, mid, last, std::max(right_count - 1, 0));
        s1.start(), s2.start();
        s1.wait(), s2.wait();
    }
}

const int N = 1e6;
int a[N], a_save[N];

#define TIME_IT(op)                                                                    \
    ({                                                                                 \
        auto beg = std::chrono::system_clock::now();                                   \
        op;                                                                            \
        std::chrono::duration<double>(std::chrono::system_clock::now() - beg).count(); \
    })

int main()
{
    const int M = 5;
    for (auto &i : a_save)
        i = rand();
    for (int t = 0; t < 1000; ++t)
    {
        double all = 0.0;
        for (int _ = 0; _ < M; ++_)
        {
            memcpy(a, a_save, sizeof a);
            all += TIME_IT(sort(a, a + N, t));
        }
        std::cout << all / M << '\n';
    }
}
