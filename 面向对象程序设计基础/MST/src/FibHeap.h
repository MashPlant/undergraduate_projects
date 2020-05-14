//
// Created by MashPlant on 18-5-5.
//

#ifndef MST_FIBHEAP_H
#define MST_FIBHEAP_H

#include <type_traits>
#include "MemPool.h"

template<typename K>
class FibHeap
{
public:
	// The list in stl can't quite suit the usage, if we pursue speed
	// so here we have a hand written list
	struct Node
	{
		K key;
		// fib heap part
		Node *p = nullptr;
		Node *head = nullptr, *tail = nullptr;
		int degree = 0;
		bool mark = false;
		// link list part
		Node *next = nullptr, *prev = nullptr;

		Node() = default;

		explicit Node(const K &key) : key(key)
		{}

		void clear()
		{
			degree = 0;
			head = tail = nullptr;
		}

		void erase(Node *x)
		{
			--degree;
			if (x->next)
				x->next->prev = x->prev;
			else // x has no next => x is tail
				tail = x->prev;

			if (x->prev)
				x->prev->next = x->next;
			else // x has no prev => x is head
				head = x->next;

			x->next = x->prev = nullptr;
		}

		void push(Node *x) // push front
		{
			++degree;
			if (head)
				head->prev = x;
			else // list is empty, so update tail
				tail = x;
			x->next = head;
			head = x;

			x->prev = nullptr;
		}

		void splice(Node *src)
		{
			if (src->degree == 0)
				return;
			degree += src->degree;
			src->degree = 0;

			tail->next = src->head;
			src->head->prev = tail;
			tail = src->tail;
		}

		const K &getKey() const
		{ return key; }
	};

private:
	MemPool<Node> alloc;
	Node *min;
	Node *root;

	void link(Node *x, Node *y) // remove y from root's children and add it to x's
	{
		y->mark = false;
		y->p = x;
		root->erase(y);
		x->push(y);
	}

	void popFix()
	{
		Node *aux[32] = {nullptr};
		for (Node *x = root->head; x;)
		{
			Node *next = x->next;
			while (Node *y = aux[x->degree])
			{
				aux[x->degree] = nullptr;
				if (y->key < x->key)
					link(y, x), x = y;
				else
					link(x, y);
			}
			aux[x->degree] = x;
			x = next;
		}
		root->clear();
		min = nullptr;
		for (int i = 0; i < 32; ++i)
			if (aux[i])
			{
				root->push(aux[i]);
				if (!min || aux[i]->key < min->key)
					min = aux[i];
			}
	}

	void cut(Node *x) // x is cut from it's p and put to root
	{
		x->p->erase(x);
		root->push(x);
		x->p = nullptr;
		x->mark = false;
	}

	void cascadingCut(Node *y)
	{
		while (y->p)
		{
			if (!y->mark)
			{
				y->mark = true;
				break;
			}
			Node *p = y->p;
			cut(y);
			y = p;
		}
	}

	void destroy(Node *x)
	{
		// this function is only called when ~FibHeap called
		// so there's no need to call deallocate
		// because the whole MemPool is going to destruct
		for (Node *it = x->head; it; it = it->next)
			destroy(it);
		alloc.destroy(x);
	}

public:
	FibHeap() : min(nullptr), root(alloc.newElem())
	{}

	// to avoid some dirty work...
	FibHeap(const FibHeap &) = delete;

	FibHeap(FibHeap &&) = delete;

	FibHeap &operator=(const FibHeap &) = delete;

	FibHeap &operator=(FibHeap &&) = delete;

	Node *push(const K &key)
	{
		Node *x = alloc.newElem(key);
		root->push(x);
		if (!min || key < min->key)
			min = x;
		return x;
	}

	const K &top() const
	{ return min->key; }

	void pop()
	{
		for (Node *x = min->head; x; x = x->next)
			x->p = nullptr;
		root->splice(min);
		root->erase(min);
		alloc.deallocate(min);
		if constexpr (!std::is_trivially_destructible_v<Node>)
			alloc.destroy(min);
		popFix();
	}

	bool empty()
	{ return min == nullptr; }

	void merge(FibHeap &src)
	{
		root->splice(src.root);
		if (!min || (src.min && src.min->key < min->key))
			min = src.min;
		src.min = nullptr;
	}

	void decKey(Node *x, const K &key)
	{
		x->key = key;
		Node *y = x->p;
		if (y && x->key < y->key)
			cut(x), cascadingCut(y);
		if (x->key < min->key)
			min = x;
	}

	~FibHeap()
	{
		if constexpr (!std::is_trivially_destructible_v<Node>)
			destroy(root);
	}
};

#endif //MST_FIBHEAP_H
