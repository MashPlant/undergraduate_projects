#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cstring>
int cmp(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}
const int N = 1e7;

int a[N], b[N];

unsigned randu32()
{
    static unsigned seed = 19260817u;
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

int main()
{
    using namespace std::chrono;
    for (int i = 0; i < N; ++i)
        b[i] = N - i;
    // b[i] = 1;
    // b[i] = i;
    // b[i] = randu32() & 0x7fffffff;
    auto now = high_resolution_clock::now;
    for (int n = N; n <= N; n += N / 10)
    {
        std::cout << "n=" << n << '\n';
        {
            memcpy(a, b, n * sizeof(int));
            auto beg = now();
            std::sort(a, a + n);
            std::cout << "std::sort: "
                      << duration<double>(now() - beg).count() << ' ' << std::is_sorted(a, a + n) << std::endl;
        }
        {
            memcpy(a, b, n * sizeof(int));
            auto beg = now();
            qsort(a, n, sizeof(int), cmp);
            std::cout << "qsort: "
                      << duration<double>(now() - beg).count() << ' ' << std::is_sorted(a, a + n) << std::endl;
        }
    }
}