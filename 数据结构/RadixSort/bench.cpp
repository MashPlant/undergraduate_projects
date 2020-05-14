#include "f4_sort.h"
#include "radix_sort.h"
#include "u4_sort.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
using namespace std::chrono;

const int N = 2e7;
float a[N], b[N];
int main() {
  std::mt19937 mt;
  std::uniform_real_distribution<float> dis(-100.0f, 100.0f);
  for (int i = 0; i < N; ++i)
    a[i] = b[i] = dis(mt);
  auto now = high_resolution_clock::now;
  {
    auto beg = now();
    f4_sort(a, N);
    auto cost = duration<double>(now() - beg).count();
    assert(std::is_sorted(a, a + N));
    printf("%.3lf\n", cost);
  }
  {
    auto beg = now();
    radix_sort_neg(b, N);
    auto cost = duration<double>(now() - beg).count();
    assert(std::is_sorted(b, b + N));
    printf("%.3lf\n", cost);
  }
}

// void u4() {
//   auto now = high_resolution_clock::now;
//   for (int i = 0; i < N; ++i)
//     a[i] = b[i] = rand();
//   {
//     auto beg = now();
//     u4_sort(a, N);
//     auto cost = duration<double>(now() - beg).count();
//     assert(std::is_sorted(a, a + N));
//     printf("%.3lf\n", cost);
//   }
//   {
//     auto beg = now();
//     radix_sort(b, N);
//     auto cost = duration<double>(now() - beg).count();
//     assert(std::is_sorted(b, b + N));
//     printf("%.3lf\n", cost);
//   }
// }
