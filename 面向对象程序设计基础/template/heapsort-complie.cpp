#include <array>
#include <functional>
#include <iostream>
#include <utility>

//c++17 only
template <class RandomAccessIter, class Comp = std::less<>>
constexpr void HeapSort(
    RandomAccessIter Begin,
    RandomAccessIter End,
    Comp IsLess = Comp{})
{
    // make_heap
    auto Count = End - Begin;
    using diff_t = decltype(Count);
    for (diff_t i0 = Count / 2; i0 > 0;)
    {
        --i0;
        diff_t i = i0;
        diff_t j = i * 2 + 1;
        auto Temp(std::move(Begin[i]));
        do
        {
            if (j + 1 < Count && IsLess(Begin[j], Begin[j + 1]))
                ++j;
            if (IsLess(Temp, Begin[j]))
                Begin[i] = std::move(Begin[j]);
            else
                break;
            i = j;
            j = i * 2 + 1;
        } while (j < Count);
        Begin[i] = std::move(Temp);
    }
    // sort_heap
    while (true)
    {
        --End;
        auto Temp(std::move(*Begin));
        *Begin = std::move(*End);
        *End = std::move(Temp);
        --Count;
        if (Count <= 1)
            break;
        diff_t i = 0;
        diff_t j = i * 2 + 1;
        Temp = std::move(Begin[i]);
        do
        {
            if (j + 1 < Count && IsLess(Begin[j], Begin[j + 1]))
                ++j;
            if (IsLess(Temp, Begin[j]))
                Begin[i] = std::move(Begin[j]);
            else
                break;
            i = j;
            j = i * 2 + 1;
        } while (j < Count);
        Begin[i] = std::move(Temp);
    }
};

template <class Container>
constexpr Container HeapSortCopy(const Container &src)
{
    auto result = src;
    HeapSort(std::begin(result), std::end(result));
    return result;
}

template <class T, T... Items>
struct MPSequence
{
};

template <class>
struct MPArrayMaker;

template <class T, T... Items>
struct MPArrayMaker<MPSequence<T, Items...>>
{
    static constexpr std::array<T, sizeof...(Items)> result{{Items...}};
};

template <class>
struct MPHeapSort;

template <class T, T... Items>
struct MPHeapSort<MPSequence<T, Items...>>
{
    static constexpr auto SortedSeq =
        HeapSortCopy(MPArrayMaker<MPSequence<T, Items...>>::result);

    template <class>
    struct IndexedType;
    template <std::size_t... Indexes>
    struct IndexedType<std::index_sequence<Indexes...>>
    {
        using type = MPSequence<T, SortedSeq[Indexes]...>;
    };

    using type =
        typename IndexedType<std::make_index_sequence<sizeof...(Items)>>::type;
};

template <class MPSeq>
using MPHeapSort_T = typename MPHeapSort<MPSeq>::type;

template <class MPSeq>
void PrintMPSequence(char delim)
{
    for (const auto &i : MPArrayMaker<MPSeq>::result)
        std::cout << i << delim;
}

int main()
{
    using seq_type = MPSequence<int, 98, 765, 43, 210, -12, 34, 567>;
    using sorted_seq_type = MPHeapSort_T<seq_type>;
    PrintMPSequence<sorted_seq_type>(' ');
    std::cout << '\n';
}