#include <cstdio>
#include <utility>
#include <cstdlib>
int *partition(int *l, int *r) // [l, r]
{
    int x = *r, *i = l;
    for (int *j = l; j < r; ++j)
        if (*j < x)
            std::swap(*i++, *j);
    std::swap(*i, *r);
    return i;
}
void qsort(int *l, int *r) // [l, r]
{
    if (l < r)
    {
        int *pivot = partition(l, r);
        qsort(l, pivot - 1);
        qsort(pivot + 1, r);
    }
}
int main()
{
    int a[100];
    for (int i = 0; i < 100; ++i)
        a[i] = rand() % 10000;
    qsort(a, a + 99);
    for (int i = 0; i < 100; ++i)
        printf("%d\n", a[i]);
}