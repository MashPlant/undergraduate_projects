#include <cstring>
#include <cstdlib>

const int N = 1e7;
using It = int *;
using T = int;
using R = long long;
T a[N], aux[N];

int insert_sort(It first, It last)
{
    int ans = 0;
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
        ans += cur - tmp;
    }
    return ans;
}

R merge_sort(It first, It last)
{
    if (last - first <= 64)
        return insert_sort(first, last);
    It mid = first + (last - first) / 2;
    R ans = merge_sort(first, mid) + merge_sort(mid, last);
    It pos1 = first, pos2 = mid, pos = aux;
    while (pos1 != mid && pos2 != last)
    {
        T a = *pos1, b = *pos2;
        int cmp = b < a;
        *pos++ = cmp ? b : a;
        ans += cmp * (mid - pos1);
        pos1 += cmp ^ 1, pos2 += cmp;
    }
    memcpy(pos, pos1, sizeof(T) * (mid - pos1)), pos += mid - pos1;
    memcpy(first, aux, sizeof(T) * (pos - aux));
    return ans;
}

#include <iostream>
#include <chrono>
#include <algorithm>
using namespace std;
using namespace chrono;

int main()
{
    for (auto &i : a)
        i = rand();
    auto now = high_resolution_clock::now;
    {
        auto beg = now();
        cout << merge_sort(a, a + N) << endl;
        cout << duration<double>(now() - beg).count() << endl;
    }
    cout << is_sorted(a, a + N) << endl;
}