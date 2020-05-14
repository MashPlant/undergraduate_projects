#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "temperature.h"

template<typename T>
void swap(T &a, T &b)
{
    T c = a;
    a = b;
    b = c;
}

template<typename T>
T min(T a, T b)
{ return a < b ? a : b; }

template<typename T>
T max(T a, T b)
{ return a < b ? b : a; }

template<typename T>
T _median(const T &a, const T &b, const T &c)
{
    return a < b ? b < c ? b : a < c ? c : a : c < b ? b : a < c ? a : c;
}

template<typename It, typename T>
It partition(It first, It last, const T &pivot)
{
    while (true)
    {
        while (*first < pivot)
            ++first;
        --last;
        while (pivot < *last)
            --last;
        if (first >= last)
            return first;
        swap(*first++, *last);
    }
}

template<typename It>
void _insert_sort(It first, It last)
{
    for (It cur = first + 1, tmp = cur; cur != last; tmp = ++cur)
    {
        auto sum = *tmp;
        if (sum < *first)
            while (tmp != first)
                *tmp = *(tmp - 1), --tmp;
        else
            while (sum < *(tmp - 1))
                *tmp = *(tmp - 1), --tmp;
        *tmp = sum;
    }
}

template<typename It>
void nth_element(It first, It nth, It last)
{
    while (last - first >= 32)
    {
        It mid = partition(first, last, _median(*first, *(first + (last - first) / 2), *(last - 1)));
        if (mid <= nth)
            first = mid;
        else
            last = mid;
    }
    _insert_sort(first, last);
}

const int N = 50000 + 5;
using i64 = long long;

struct P
{
    static int d;
    int x, y, temp;

    bool operator<(const P &rhs) const
    { return (&x)[d] < (&rhs.x)[d]; }
} ps[N];

struct Node
{
    P split;
    int x1, y1, x2, y2;
    int cnt;
    i64 sum;
    Node *l, *r;
};
Node *nil = new Node{};
int P::d = 0;

Node *build(int l, int r, int d)
{
    if (l == r)
        return nil;
    int mid = (l + r) >> 1;
    P::d = d;
    nth_element(ps + l, ps + mid, ps + r);
    int x1 = ps[l].x, y1 = ps[l].y, x2 = ps[l].x, y2 = ps[l].y;
    for (int i = l + 1; i < r; ++i)
    {
        x1 = min(x1, ps[i].x);
        y1 = min(y1, ps[i].y);
        x2 = max(x2, ps[i].x);
        y2 = max(y2, ps[i].y);
    }
    Node *ret = new Node{ps[mid], x1, y1, x2, y2};
    ret->l = build(l, mid, d ^ 1);
    ret->r = build(mid + 1, r, d ^ 1);
    ret->sum = ret->l->sum + ret->r->sum + ret->split.temp;
    ret->cnt = ret->l->cnt + ret->r->cnt + 1;
    return ret;
}

int x1, y1, x2, y2;
int cnt;
i64 sum;

void query(Node *x, int d)
{
    if (x == nil)
        return;
    if (x1 <= x->x1 && x->x2 <= x2 && y1 <= x->y1 && x->y2 <= y2)
        return sum += x->sum, void(cnt += x->cnt);
    if (x1 <= x->split.x && x->split.x <= x2 && y1 <= x->split.y && x->split.y <= y2)
        sum += x->split.temp, ++cnt;
    if (d == 0)
    {
        if (x1 <= x->split.x)
            query(x->l, 1);
        if (x->split.x <= x2)
            query(x->r, 1);
    }
    else
    {
        if (y1 <= x->split.y)
            query(x->l, 0);
        if (x->split.y <= y2)
            query(x->r, 0);
    }
}

int main()
{
    int n = GetNumOfStation();
    for (int i = 0; i < n; ++i)
        GetStationInfo(i, &ps[i].x, &ps[i].y, &ps[i].temp);
    Node *root = build(0, n, 0);
    while (GetQuery(&x1, &y1, &x2, &y2))
    {
        sum = 0, cnt = 0;
        query(root, 0);
        Response(cnt ? sum / cnt : 0);
    }
}