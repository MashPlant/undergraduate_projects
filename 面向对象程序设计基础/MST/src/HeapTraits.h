//
// Created by MashPlant on 18-5-8.
//

#ifndef MST_HEAPTRAITS_H
#define MST_HEAPTRAITS_H

#include <type_traits>
#include <queue>
#include <functional>

namespace detail
{
	template<typename Heap, typename = decltype(&Heap::decKey)>
	std::true_type isDecreaseableHeap(int);

	template<typename Heap>
	std::false_type isDecreaseableHeap(double);

	template<typename Heap, typename = decltype(&Heap::merge)>
	std::true_type isMergeableHeap(int);

	template<typename Heap>
	std::false_type isMergeableHeap(double);
}

template<typename Heap>
constexpr inline bool IsDecreaseableHeapV = decltype(detail::isDecreaseableHeap<Heap>(0))::value;

template<typename Heap>
constexpr inline bool IsMergeableHeapV = decltype(detail::isMergeableHeap<Heap>(0))::value;

template<typename T>
using BinaryHeap = std::priority_queue<T, std::vector<T>, std::greater<T>>;

template<typename T>
using NoHeap = void;


#endif //MST_HEAPTRAITS_H
