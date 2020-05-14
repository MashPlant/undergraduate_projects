#ifndef SORTER_H
#define SORTER_H
#include <functional>
#include <iterator>
#include <type_traits>
#include <cstring>

class Sorter
{
    template <typename Iter, typename Comp>
    static void fixHeap(Iter first, Comp comp, int where, int heapSize) //有且仅有where及其孩子违反堆性质，修复之
    {
        int nxt = (where << 1) + 1;
        auto value = std::move(first[where]);
        while (nxt < heapSize)
        {
            if (nxt + 1 < heapSize && comp(first[nxt], first[nxt + 1]))
                ++nxt;
            if (comp(value, first[nxt]))
            {
                first[where] = std::move(first[nxt]);
                where = nxt;
                nxt = (where << 1) + 1;
            }
            else
                break;
        }
        first[where] = std::move(value);
    }
    template <typename T, typename Comp>
    static T median(const T &a, const T &b, const T &c, Comp comp)
    {
        if (comp(a, b)) // a<b
        {
            if (comp(b, c)) //a<b<c
                return b;
            if (comp(a, c))
                return c;
            return a;
        }
        //a>=b
        if (comp(a, c)) //b<=a<c
            return a;
        if (comp(b, c))
            return c;
        return b;
    }
    template <typename Iter, typename T, typename Comp>
    static Iter partion(Iter first, Iter last, const T &pivot, Comp comp)
    {
        // Iter ret = first - 1;
        // int len = last - first;
        // for (int i = 0; i < len; ++i)
        //     if (comp(first[i],pivot))
        //         std::swap(first[i], *++ret);
        // return ret;
        while (true)
        {
            while (comp(*first, pivot))
                ++first; //找出第一个>=pivot的first
            --last;
            while (comp(pivot, *last))
                --last; //找出第一个(倒着数)<=pivot的last
            if (first >= last)
                return first;
            std::swap(*first, *last);
            ++first; //swap完了之后first指向的位置已经保证<=pivot,last同理
                     //但是对last的修改在下一轮循环中进行,可以避免特判last是不是尾后迭代器
        }
    }
    static int lg(int x)
    {
        int ret = 0;
        for (; x; x >>= 1, ++ret)
            ;
        return ret;
    }
    template <typename Iter, typename Comp>
    static void pushHeap(Iter first, Iter last, Comp comp) //[first,last-1)为合法堆,push的新值是last-1
    {
        //不用swap而是用赋值,稍微快一些
        auto value = std::move(*(last - 1));
        int hole = last - first - 1; //刚刚被move的下标
        int parent = (hole - 1) >> 1;
        while (hole && comp(*(first + parent), value)) //value一直在向上浮动
        {
            *(first + hole) = std::move(*(first + parent));
            hole = parent;
            parent = (hole - 1) >> 1;
        }
        *(first + hole) = std::move(value);
    }
    template <typename Iter>
    static void pushHeap(Iter first, Iter last)
    {
        pushHeap(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
    }
    template <typename Iter, typename Comp>
    static void introSort(Iter first, Iter last, Comp comp, int depth)
    {
        if (last - first <= 64)
            return;
        if (!depth)
        {
            heapSort(first, last, comp);
            return;
        }
        const auto &pivot = median(*first, *(first + (last - first) / 2), *(last - 1), comp);
        Iter div = partion(first, last, pivot, comp);
        introSort(first, div, comp, depth - 1);
        introSort(div + 1, last, comp, depth - 1);
    }

  public:
    template <typename Iter, typename Comp>
    static void sort(Iter first, Iter last, Comp comp)
    {
        if (last - first <= 1)
            return;
        introSort(first, last, comp, 2 * lg(last - first));
        insertSort(first, last, comp);
    }
    template <typename Iter>
    static void sort(Iter first, Iter last)
    {
        sort(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
    }
    template <typename Iter, typename Comp>
    static void heapSort(Iter first, Iter last, Comp comp)
    {
        int heapSize = last - first;
        for (int i = heapSize >> 1; i >= 0; --i)
            fixHeap(first, comp, i, heapSize);
        while (heapSize)
        {
            std::swap(first[0], first[--heapSize]);
            fixHeap(first, comp, 0, heapSize);
        }
    }
    template <typename Iter>
    static void heapSort(Iter first, Iter last)
    {
        heapSort(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
    }
    template <typename Iter, typename Comp>
    static void insertSort(Iter first, Iter last, Comp comp)
    {
        Iter cur = first;
        while (cur != last)
        {
            Iter tmp = cur++;
            auto value = std::move(*tmp);
            if (comp(value, *first)) //首先与第一个比较，如果比第一个还小，那么直接移动
            {
                while (tmp != first) //这里就不用判断大小了
                    *tmp = std::move(*(tmp - 1)), --tmp;
                *tmp = std::move(value);
                continue;
            }
            //这里就不用判断越界了
            while (comp(value, *(tmp - 1)))
                *tmp = std::move(*(tmp - 1)), --tmp;
            *tmp = std::move(value);
        }
    }
    template <typename Iter>
    static void insertSort(Iter first, Iter last)
    {
        insertSort(first, last, std::less<typename std::iterator_traits<Iter>::value_type>());
    }
    template <typename Integer>
    static void radixSort(Integer *first, Integer *last)
    {
        static_assert(std::is_integral<Integer>::value, "radix sort can only be used ong integers");
        const static int U = 65536;
        static int cnt[U];
        const static int loop = sizeof(Integer) / 2;
        const int size = last - first;
        auto mask = [](Integer x, int d) { return (x >> (16 * d)) & (U - 1); };
        Integer *aux = reinterpret_cast<Integer *>(malloc(size * sizeof(Integer)));
        for (int d = 0; d < loop; ++d)
        {
            memset(cnt, 0, sizeof(cnt));
            for (int i = 0; i < size; ++i)
                ++cnt[mask(first[i], d)];
            for (int i = 1; i < U;++i)
                cnt[i] += cnt[i - 1];
            for (int i = size - 1; i >= 0;--i)
                aux[--cnt[mask(first[i], d)]] = first[i];
            memcpy(first, aux, sizeof(Integer) * size);
        }
        free(aux);
    }
};

#endif