#include <cstring>
namespace ds
{

template <typename It>
It next(It it) { return ++it; }

template <typename It>
It prev(It it) { return --it; }

template <typename T>
const T &min(const T &lhs, const T &rhs) { return lhs < rhs ? lhs : rhs; }

template <typename T>
const T &max(const T &lhs, const T &rhs) { return lhs < rhs ? rhs : lhs; }

// int min(int lhs, int rhs) { return rhs ^ ((lhs ^ rhs) & -(lhs < rhs)); }

// int max(int lhs, int rhs) { return lhs ^ ((lhs ^ rhs) & -(lhs < rhs)); }

// // prerequisite: -2147483648 <= lhs - rhs <= 2147483647
// int _min(int lhs, int rhs) { return rhs + ((lhs - rhs) & ((lhs - rhs) >> 31)); }

// int _max(int lhs, int rhs) { return lhs - ((lhs - rhs) & ((lhs - rhs) >> 31)); }

template <typename T>
void swap(T &lhs, T &rhs)
{
    // who cares whether T is nothrow_moveable?
#if __cplusplus >= 201103L
    T tmp = static_cast<T &&>(lhs);
    lhs = static_cast<T &&>(rhs);
    rhs = static_cast<T &&>(tmp);
#else
    T tmp = lhs;
    lhs = rhs;
    rhs = tmp;
#endif
}

template <typename T>
T _median(const T &a, const T &b, const T &c)
{
    return a < b ? b < c ? b : a < c ? c : a : c < b ? b : a < c ? a : c;
}

template <typename T>
T _median(const T &a, const T &b, const T &c, const T &d, const T &e)
{
    return b < a ? d < c ? b < d ? a < e ? a < d ? e < d ? e : d
                                                 : c < a ? c : a
                                         : e < d ? a < d ? a : d
                                                 : c < e ? c : e
                                 : c < e ? b < c ? a < c ? a : c
                                                 : e < b ? e : b
                                         : b < e ? a < e ? a : e
                                                 : c < b ? c : b
                         : b < c ? a < e ? a < c ? e < c ? e : c
                                                 : d < a ? d : a
                                         : e < c ? a < c ? a : c
                                                 : d < e ? d : e
                                 : d < e ? b < d ? a < d ? a : d
                                                 : e < b ? e : b
                                         : b < e ? a < e ? a : e
                                                 : d < b ? d : b
                 : d < c ? a < d ? b < e ? b < d ? e < d ? e : d
                                                 : c < b ? c : b
                                         : e < d ? b < d ? b : d
                                                 : c < e ? c : e
                                 : c < e ? a < c ? b < c ? b : c
                                                 : e < a ? e : a
                                         : a < e ? b < e ? b : e
                                                 : c < a ? c : a
                         : a < c ? b < e ? b < c ? e < c ? e : c
                                                 : d < b ? d : b
                                         : e < c ? b < c ? b : c
                                                 : d < e ? d : e
                                 : d < e ? a < d ? b < d ? b : d
                                                 : e < a ? e : a
                                         : a < e ? b < e ? b : e
                                                 : d < a ? d : a;
}

template <typename It, typename T>
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
        ds::swap(*first++, *last);
    }
}

template <typename It>
void _insert_sort(It first, It last)
{
    for (It cur = ds::next(first), tmp = cur; cur != last; tmp = ++cur)
    {
        auto val = *tmp;
        if (val < *first)
            while (tmp != first)
                *tmp = *ds::prev(tmp), --tmp;
        else
            while (val < *ds::prev(tmp))
                *tmp = *ds::prev(tmp), --tmp;
        *tmp = val;
    }
}

template <typename It>
void _intro_sort(It first, It last)
{
    if (last - first <= 32)
        return;
    It mid = partition(first, last, _median(*first, *(first + (last - first) / 2), *(last - 1)));
    _intro_sort(first, mid);
    _intro_sort(mid, last); // it should be mid + 1 here, but somehow using mid is faster
}

template <typename It>
void sort(It first, It last)
{
    _intro_sort(first, last);
    _insert_sort(first, last);
}

template <typename It, typename T>
It lower_bound(It first, It last, const T &val)
{
    --last;
    while (first <= last)
    {
        It mid = first + (last - first) / 2;
        if (*mid < val)
            first = mid + 1;
        else
            last = mid - 1;
    }
    return first;
}

template <typename It>
int unique(It first, It last)
{
    It uni = first, copy_first = first;
    while (first != last)
        *first == *uni ? ++first : *++uni = *first++;
    return uni + 1 - copy_first;
}

template <typename Int, typename Check>
Int bsearch_ans(Int first, Int last, Check check)
{
    while (first <= last)
    {
        Int mid = first + last >> 1;
        if (check(mid))
            last = mid - 1;
        else
            first = mid + 1;
    }
    return first;
}

// [first, last - 2] is a max heap
// last[-1] is newly added by 'push_back' etc.
// first == last => UB, will access first[0]
template <typename It>
void push_heap(It first, It last)
{
    int n = last - first--, fa = n >> 1;
    auto val = first[n];
    while (fa && first[fa] < val)
    {
        first[n] = first[fa];
        n = fa, fa = n >> 1;
    }
    first[n] = val;
}

// [first, last - 1] is a max heap
// after pop, original top will be moved to last - 1, can be removed by `pop_back` etc.
// and [first, last - 2] will be a max heap
// first == last => UB, will access first[0] & first[-1]
template <typename It>
void pop_heap(It first, It last)
{
    int n = last - first, fa = 0, son = 1;
    ds::swap(first[0], first[--n]);
    auto val = first[0];
    while (true)
    {
        if (son + 1 < n && first[son] < first[son + 1])
            ++son;
        if (son < n && val < first[son])
            first[fa] = first[son], fa = son, son = (fa << 1) + 1;
        else
            break;
    }
    first[fa] = val;
}

template <typename It>
void make_heap(It first, It last)
{
    int n = last - first;
    for (int i = n >> 1; i >= 0; --i)
    {
        int cur = i, son = (cur << 1) + 1;
        auto val = first[cur];
        while (true)
        {
            if (son + 1 < n && first[son] < first[son + 1])
                ++son;
            if (son < n && val < first[son])
                first[cur] = first[son], cur = son, son = (cur << 1) + 1;
            else
                break;
        }
        first[cur] = val;
    }
}

// first is the 1st element
// nth will be put at first[n - 1]
template <typename It>
void nth_element(It first, It last, int n)
{
    while (true)
    {
        It mid = partition(first, last, _median(*first, *(first + (last - first) / 2), *(last - 1)));
        int cur = mid - first + 1;
        if (cur > n)
            last = mid;
        else if (cur < n)
            first = mid + 1, n -= cur;
        else
            return;
    }
}

template <typename It>
void reverse(It first, It last)
{
    while ((first != last) && (first != --last))
        ds::swap(*first++, *last);
}

template <typename It>
bool next_permutation(It first, It last)
{
    It bound = last;
    while (--last != first)
        if (*ds::prev(last) < *last)
        {
            It first_dec = ds::prev(last);
            It last_gt = bound;
            while (!(*first_dec < *--last_gt))
                ;
            ds::swap(*first_dec, *last_gt);
            ds::reverse(last, bound);
            return true;
        }
    ds::reverse(first, bound);
    return false;
}

unsigned randu32()
{
    static unsigned seed = 19260817u;
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

int randi32() { return randu32() & 0x7fffffff; }

int randi32(int l, int r) { return l + randu32() % (r - l + 1); }

int randi32(int r) { return randu32() % (r + 1); }
} // namespace ds