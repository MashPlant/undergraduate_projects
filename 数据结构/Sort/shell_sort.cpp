void insert_sort(int *first, int *last, int step)
{
    for (int *cur = first + step, *tmp = cur; cur < last; tmp = (cur += step))
    {
        int val = *tmp;
        if (val < *first)
            while (tmp != first)
                *tmp = *(tmp - step), tmp -= step;
        else
            while (val < *(tmp - step))
                *tmp = *(tmp - step), tmp -= step;
        *tmp = val;
    }
}
void _3_smooth_seq(int *dst, int n)
{
    int i2 = 0, i3 = 0;
    dst[0] = 1;
    for (int i = 1; i < n; ++i)
    {
        int n2 = 2 * dst[i2], n3 = 3 * dst[i3];
        dst[i] = n2 < n3 ? n2 : n3;
        i2 += n2 <= n3;
        i3 += n2 >= n3;
    }
}

void shell_sort(int *first, int *last)
{
    int step = last - first;
    do
    {
        step = step / 3 + 1;
        for (int i = 0; i < step; ++i)
            insert_sort(first + i, last, step);
    } while (step != 1);
    // const int S = 200;
    // int step[S];
    // _3_smooth_seq(step, S);
    // for (int i = S - 1; i >= 0; --i)
    //     for (int j = 0; j < step[i]; ++j)
    //         insert_sort(first + j, last, step[i]);
}

#include <cstdio>
#include <algorithm>
#include <iostream>
#include <chrono>

using namespace std::chrono;
using namespace std;
auto now = high_resolution_clock::now;
const int N = 1e7;
int a[N];

void unfinished_qsort(int *first, int *last)
{
    if (last - first <= 32)
        return;
    int *mid = std::__unguarded_partition_pivot(first, last, __gnu_cxx::__ops::__iter_less_iter());
    unfinished_qsort(first, mid);
    unfinished_qsort(mid, last);
}
int main()
{
    for (auto &i : a)
        i = rand();
    // auto beg = now();
    // unfinished_qsort(a, a + N);
    {
        auto beg = now();
        shell_sort(a, a + N);
        // std::__final_insertion_sort(a, a + N, __gnu_cxx::__ops::__iter_less_iter());
        std::cout << duration<double>(now() - beg).count() << std::endl;
    }
    // std::cout << duration<double>(now() - beg).count() << std::endl;
    cout << is_sorted(a, a + N);
}