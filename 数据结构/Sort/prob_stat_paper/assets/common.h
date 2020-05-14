#include <cstring>
#include <cstdlib>
#include <utility>
#include <random>

int cmp_cnt = 0;

struct Int
{
    int val;
    bool operator<(Int rhs) const
    {
        ++cmp_cnt;
        return val < rhs.val;
    }
};

const int N = 2e5;
Int a[N];

Int *partition(Int *l, Int *r) // [l, r]
{
    Int x = *r, *i = l;
    for (Int *j = l; j < r; ++j)
        if (*j < x)
            std::swap(*i++, *j);
    std::swap(*i, *r);
    return i;
}

std::mt19937 mt(time(nullptr));
std::uniform_int_distribution<> dis;

std::pair<double, double> ave_s2(int *a, int n)
{
    double ave = 0.0;
    for (int i = 0; i < n; ++i)
        ave += a[i];
    ave /= n;
    double s2 = 0.0;
    for (int i = 0; i < n; ++i)
        s2 += (a[i] - ave) * (a[i] - ave);
    s2 /= n - 1;
    return {ave, s2};
}