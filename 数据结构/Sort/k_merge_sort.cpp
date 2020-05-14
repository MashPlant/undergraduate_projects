#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <climits>
#include <thread>

#define MULTI_THREAD 0
const int N = 1e7;
int a[N], aux[N];

const int K = 128;

// Note: in an older version I used a lambda called `cmp` to handle -1 and got WA
// because capture by value will COPY array elements rather than pointer
void k_merge_sort(int *first, int *last, int *aux)
{
//    puts("hi there");
    if (last - first <= 4096)
        return void(std::sort(first, last));
    int n = last - first, n_k = n / K;
    int *ptr_[K + 1], **ptr = ptr_ + 1, sentinel = INT_MAX;
    ptr[-1] = &sentinel;
#if MULTI_THREAD
    {
        std::thread ts[K];
        for (int i = 0; i < K; ++i)
        {
            ptr[i] = first + n_k * i;
            ts[i] = std::thread(k_merge_sort, ptr[i],
                                i == K - 1 ? last : (first + n_k * (i + 1)), aux + n_k * i);
        }
        for (int i = 0; i < K; ++i)
            ts[i].join();
    }
#else
    for (int i = 0; i < K; ++i)
        k_merge_sort(ptr[i] = first + n_k * i,
                     i == K - 1 ? last : (first + n_k * (i + 1)), aux + n_k * i);
#endif
    // [1, 2 * K - 1] indexed, 0 is useless
    int tree[2 * K]; // winner tree
    for (int i = 2 * K - 1; i >= K; --i)
        tree[i] = i - K;
    for (int i = K - 1; i >= 1; --i)
        tree[i] = *ptr[tree[i << 1]] < *ptr[tree[i << 1 | 1]] ? tree[i << 1] : tree[i << 1 | 1];
    int *aux_ptr = aux;
    while (tree[1] != -1)
    {
        int winner = tree[1];
        *aux_ptr++ = *ptr[winner];
        if (++ptr[winner] == (winner == K - 1 ? last : first + n_k * (winner + 1)))
            tree[winner + K] = -1;
        for (int i = winner + K; i > 1; i >>= 1)
            tree[i >> 1] = *ptr[tree[i]] < *ptr[tree[i ^ 1]] ? tree[i] : tree[i ^ 1];
    }
    memcpy(first, aux, sizeof(int) * (aux_ptr - aux));
}

#include <iostream>
#include <chrono>

using namespace std::chrono;
using namespace std;
int b[N];

int main()
{
    for (int i = 0; i < N; ++i)
        a[i] = b[i] = rand();
    {
        auto now = high_resolution_clock::now;
        auto beg = now();
        k_merge_sort(a, a + N, aux);
        std::cout << duration<double>(now() - beg).count() << std::endl;
    }
    {
        auto now = high_resolution_clock::now;
        auto beg = now();
        std::sort(b, b + N);
        std::cout << duration<double>(now() - beg).count() << std::endl;
    }
    cout << memcmp(a, b, sizeof a) << endl;
}