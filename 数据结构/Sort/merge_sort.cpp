#include <cstring>
#include <cstdlib>

using It = int *;
using T = int;

const int N = 1e7;
int a[N], aux[N], b[N];

void insert_sort(It first, It last)
{
    for (It cur = first + 1, tmp = cur; cur != last; tmp = ++cur)
    {
        T val = *tmp;
        if (val < *first)
            while (tmp != first)
                *tmp = *(tmp - 1), --tmp;
        else
            while (val < *(tmp - 1))
                *tmp = *(tmp - 1), --tmp;
        *tmp = val;
    }
}

void merge_sort(It first, It last)
{
    if (last - first <= 64)
        return insert_sort(first, last);
    It mid = first + (last - first) / 2;
    merge_sort(first, mid);
    merge_sort(mid, last);
    It pos1 = first, pos2 = mid, pos = aux;
    while (pos1 != mid && pos2 != last)
    {
        T a = *pos1, b = *pos2;
        int cmp = b < a;
        *pos++ = cmp ? b : a;
        pos1 += cmp ^ 1, pos2 += cmp;
    }
    memcpy(pos, pos1, sizeof(T) * (mid - pos1)), pos += mid - pos1;
    memcpy(first, aux, sizeof(T) * (pos - aux));
}

#include <chrono>
#include <iostream>
#include <algorithm>

int main()
{
    for (int i = 0; i < N; ++i)
        a[i] = b[i] = rand();
    using namespace std::chrono;
    {
        auto now = high_resolution_clock::now;
        auto beg = now();
        merge_sort(a, a + N);
        std::cout << duration<double>(now() - beg).count() << std::endl;
    }
    {
        auto now = high_resolution_clock::now;
        auto beg = now();
        std::sort(b, b + N);
        std::cout << duration<double>(now() - beg).count() << std::endl;
    }
    std::cout << memcmp(a, b, sizeof a) << std::endl;
}
