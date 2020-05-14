template<typename T>
void swap(T &a, T &b)
{
    T c = a;
    a = b;
    b = c;
}

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