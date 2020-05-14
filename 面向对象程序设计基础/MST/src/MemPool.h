//
// Created by MashPlant on 18-5-9.
//

#ifndef MST_MEMPOOL_H
#define MST_MEMPOOL_H

#include <cstdlib>
#include <utility>

template<typename K, int BlockSize = 4096>
class MemPool
{
	union Slot
	{
		K data;
		Slot *next;
	};
	// freeSlot stores a link list of free memory from deallocate
	Slot *curBlock = nullptr, *freeSlot = nullptr;
	int pos = 1;

public:
	typedef K value_type;
	typedef value_type *pointer;
	typedef value_type &reference;
	typedef const value_type *const_pointer;
	typedef const value_type &const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	template<typename U>
	struct rebind
	{
		typedef MemPool<U, BlockSize> other;
	};

	pointer allocate(size_type = 1)
	{
		if (freeSlot != nullptr)
		{
			Slot *ret = freeSlot;
			freeSlot = freeSlot->next;
			return reinterpret_cast<pointer>(ret);
		}
		if (pos == BlockSize || curBlock == nullptr)
		{
			Slot *tmp = curBlock;
			curBlock = reinterpret_cast<Slot *>(malloc(BlockSize * sizeof(Slot)));
			curBlock->next = tmp;
			pos = 1;
		}
		return reinterpret_cast<pointer>(curBlock + pos++);
	}

	template<typename ...Args>
	void construct(pointer p, Args &&...args)
	{
		new(p) value_type(std::forward<Args>(args)...);
	}

	template<typename ...Args>
	pointer newElem(Args &&...args)
	{ return new(allocate()) value_type(std::forward<Args>(args)...); }

	void deallocate(pointer p, size_type = 1)
	{
		if (p)
		{
			Slot *tmp = reinterpret_cast<Slot *>(p);
			tmp->next = freeSlot;
			freeSlot = tmp;
		}
	}

	static void destroy(pointer p)
	{
		p->~value_type();
	}

	MemPool() noexcept
	{}

	template<typename K_, int BlockSize_>
	MemPool(const MemPool<K_, BlockSize_> &) noexcept
	{}

	~MemPool()
	{
		while (curBlock)
		{
			Slot *tmp = curBlock->next;
			free(curBlock);
			curBlock = tmp;
		}
	}
};

#endif //MST_MEMPOOL_H
