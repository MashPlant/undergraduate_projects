//
// Created by MashPlant on 18-5-9.
//

#ifndef MST_PAIRINGHEAP_H
#define MST_PAIRINGHEAP_H

#include <utility>
#include <type_traits>
#include "MemPool.h"

template<typename K>
class PairingHeap
{
public:
	struct Node
	{
		K key;
		Node *porp = nullptr; // prev or parent
		Node *ch = nullptr;
		Node *next = nullptr;

		Node(const K &key) : key(key)
		{}

		const K &getKey() const
		{ return key; }
	};

private:
	MemPool<Node> alloc;
	Node *root = nullptr;

	static Node *merge(Node *dest, Node *sour)
	{
		if (!dest) return sour;
		if (!sour) return dest;
		if (sour->key < dest->key)
			std::swap(dest, sour);
		makeChildOf(sour, dest);
		return dest;
	}

	static void makeChildOf(Node *ch, Node *p)
	{
		ch->next = p->ch;
		if (p->ch)
			p->ch->porp = ch;
		ch->porp = p;
		p->ch = ch;
	}

	static Node *joinChild(Node *p)
	{
		Node *ret = p->ch;
		if (!ret)
			return nullptr;
		while (ret->next)
			ret = fwdJoin(ret, ret->next);
		while (ret->porp != p)
			ret = backJoin(ret->porp, ret);
		return ret;
	}

	static Node *fwdJoin(Node *cur, Node *nxt)
	{
		if (nxt->key < cur->key)
		{
			nxt->porp = cur->porp;
			makeChildOf(cur, nxt);
			return nxt->next ? nxt->next : nxt;
		}
		if (nxt->next)
		{
			nxt->next->porp = cur;
			cur->next = nxt->next;
			makeChildOf(nxt, cur);
			return cur->next;
		}
		cur->next = nullptr;
		makeChildOf(nxt, cur);
		return cur;
	}

	static Node *backJoin(Node *pre, Node *cur)
	{
		if (cur->key < pre->key)
		{
			cur->porp = pre->porp;
			makeChildOf(pre, cur);
			return cur;
		}
		pre->next = nullptr;
		makeChildOf(cur, pre);
		return pre;
	}

	void destroy(Node *x)
	{
		for (Node *it = x->ch; it; it = it->next)
			destroy(x);
		alloc.destroy(x);
	}

public:
	PairingHeap() = default;

	PairingHeap(const PairingHeap &) = delete;

	PairingHeap(PairingHeap &&) = delete;

	PairingHeap &operator=(const PairingHeap &) = delete;

	PairingHeap &operator=(PairingHeap &&) = delete;

	Node *push(const K &key)
	{
		Node *ret = alloc.newElem(key);
		root = merge(root, ret);
		return ret;
	}

	void merge(PairingHeap &rhs)
	{ root = merge(root, rhs.root); }

	const K &top() const
	{ return root->key; }

	bool empty() const
	{ return !root; }

	void pop()
	{
		Node *dead = root;
		root = joinChild(root);
		alloc.deallocate(dead);
		if constexpr (!std::is_trivially_destructible_v<Node>)
			alloc.destroy(dead);
	}

	void decKey(Node *x, const K &key)
	{
		x->key = key;
		if (x == root)
			return;
		if (x->next)
			x->next->porp = x->porp;
		if (x->porp->next == x) // porp is prev
			x->porp->next = x->next;
		else // porp is parent
			x->porp->ch = x->next;
		x->porp = nullptr;
		root = merge(root, x);
	}

	~PairingHeap()
	{
		if constexpr (!std::is_trivially_destructible_v<Node>)
			destroy(root);
	}
};

#endif //MST_PAIRINGHEAP_H
