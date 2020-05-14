#include <cstring>
#include <algorithm>
#include <iostream>
#include <chrono>
const int N = 1e8, T = 1;

unsigned randu32()
{
    static unsigned seed = 19260817u;
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

void radix_sort1(int *a, int n)
{
    const int U = 65536;
    int cnt[U];
    int *aux = new int[n];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[a[i] & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        aux[--cnt[a[i] & U - 1]] = a[i];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[aux[i] >> 16];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        a[--cnt[aux[i] >> 16]] = aux[i];
    delete[] aux;
}

void radix_sort2(int *a, int n)
{
    const int U = 256;
    int cnt[U];
    int *aux = new int[n];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[a[i] & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        aux[--cnt[a[i] & U - 1]] = a[i];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[aux[i] >> 8 & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        a[--cnt[aux[i] >> 8 & U - 1]] = aux[i];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[a[i] >> 16 & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        aux[--cnt[a[i] >> 16 & U - 1]] = a[i];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[aux[i] >> 24];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        a[--cnt[aux[i] >> 24]] = aux[i];
    delete[] aux;
}

void radix_sort3(int *a, int n)
{
    const int U = 2048;
    int cnt[U];
    int *aux = new int[n];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[a[i] & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        aux[--cnt[a[i] & U - 1]] = a[i];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[aux[i] >> 11 & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        a[--cnt[aux[i] >> 11 & U - 1]] = aux[i];
    memset(cnt, 0, sizeof cnt);
    for (int i = 0; i < n; ++i)
        ++cnt[a[i] >> 22 & U - 1];
    for (int i = 1; i < U; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = n - 1; i >= 0; --i)
        aux[--cnt[a[i] >> 22 & U - 1]] = a[i];
    memcpy(a, aux, sizeof(int) * n);
    delete[] aux;
}

int a[N], b[N];

int main()
{
    using namespace std::chrono;
    for (int i = 0; i < N; ++i)
        b[i] = randu32() & 0x7fffffff;
    auto now = high_resolution_clock::now;
    for (int n = N / 10; n <= N; n += N / 10)
    {
        std::cout << "n=" << n << '\n';
        {
            memcpy(a, b, n * sizeof(int));
            auto beg = now();
            radix_sort1(a, n);
            std::cout << "radix sort(base=65536): "
                      << duration<double>(now() - beg).count() << std::endl;
        }
        {
            memcpy(a, b, n * sizeof(int));
            auto beg = now();

            radix_sort2(a, n);
            std::cout << "radix sort(base=256): "
                      << duration<double>(now() - beg).count() << std::endl;
        }
    }
}