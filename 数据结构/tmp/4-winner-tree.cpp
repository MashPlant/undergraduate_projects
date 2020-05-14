#include "kth.h"
#include <cstdio>
#include <cstring>

template <typename T>
void swap(T &a, T &b)
{
	T c = a;
	a = b;
	b = c;
}

const int N = 500000 + 5;
const int K = 2000000 + 5;
int aux[N];

template <typename Comp>
void insert_sort(int *first, int *last, Comp comp)
{
	for (int *cur = first + 1, *tmp = cur; cur != last; tmp = ++cur)
	{
		int val = *tmp;
		if (comp(val, *first))
			while (tmp != first)
				*tmp = *(tmp - 1), --tmp;
		else
			while (comp(val, *(tmp - 1)))
				*tmp = *(tmp - 1), --tmp;
		*tmp = val;
	}
}

template <typename Comp>
void merge_sort(int *first, int *last, Comp comp)
{
	if (last - first <= 16)
		return insert_sort(first, last, comp);
	int *mid = first + (last - first) / 2;
	merge_sort(first, mid, comp);
	merge_sort(mid, last, comp);
	int *pos1 = first, *pos2 = mid, *pos = aux;
	while (pos1 != mid && pos2 != last)
	{
		int a = *pos1, b = *pos2;
		int cmp = comp(b, a);
		*pos++ = cmp ? b : a;
		pos1 += cmp ^ 1, pos2 += cmp;
	}
	memcpy(pos, pos1, sizeof(int) * (mid - pos1)), pos += mid - pos1;
	memcpy(first, aux, sizeof(int) * (pos - aux));
}

int n, s1[N], s2[N], s3[N];

template <typename T>
struct Heap // min heap
{
	int tree[N * 2];
	T data[N];

	void build()
	{
		for (int i = n + 1; i <= 2 * n; ++i)
			tree[i] = i - n;
		tree[2 * n + 1] = n;
		for (int i = 2 * n; i > 1; --i)
			tree[i >> 1] = tree[i] < tree[i ^ 1] ? tree[i] : tree[i ^ 1];
	}

	void update(int i, T val)
	{
		data[i] = val;
		for (i += n; i > 1; i >>= 1)
			tree[i >> 1] = data[tree[i]] < data[tree[i ^ 1]] ? tree[i] : tree[i ^ 1];
	}

	int top()
	{
		return tree[1];
	}
};

struct Data3
{
	int x, y, z, num;

	bool operator<(const Data3 &rhs) const
	{
		return compare(x, y, z, rhs.x, rhs.y, rhs.z);
	}
};

Heap<Data3> h;

namespace rem
{
struct Data2
{
	int y, z, num;

	bool operator<(const Data2 &rhs) const
	{
		// a sequence is over, set value to infinity
		if (num == n + 1)
			return false;
		if (rhs.num == n + 1)
			return true;
		return compare(1, y, z, 1, rhs.y, rhs.z);
	}
} cache[K];

Heap<Data2> h;

int count;

Data2 get(int k)
{
	if (k <= count)
		return cache[k];
	int top = h.top();
	Data2 cur = h.data[top];
	h.update(top, {cur.y, s3[cur.num + 1], cur.num + 1});
	return cache[count = k] = cur;
}

void init()
{
	for (int i = 1; i <= n; ++i)
		h.data[i] = {s2[i], s3[1], 1};
	h.build();
	get(1);
}
} // namespace rem

void get_kth(int n, int k, int *x, int *y, int *z)
{
	::n = n;
	for (int i = 1; i <= n; ++i)
		s1[i] = s2[i] = s3[i] = i;
	merge_sort(s1 + 1, s1 + n + 1, [](int l, int r) { return compare(l, 1, 1, r, 1, 1); });
	merge_sort(s2 + 1, s2 + n + 1, [](int l, int r) { return compare(1, l, 1, 1, r, 1); });
	merge_sort(s3 + 1, s3 + n + 1, [](int l, int r) { return compare(1, 1, l, 1, 1, r); });
	for (int i = 1; i <= n; ++i)
		h.data[i] = {s1[i], s2[1], s3[1], 1};
	h.build();
	rem::init();
	while (--k)
	{
		int top = h.top();
		Data3 cur = h.data[top];
		rem::Data2 nxt = rem::get(cur.num + 1);
		h.update(top, {cur.x, nxt.y, nxt.z, cur.num + 1});
	}
	int top = h.top();
	*x = h.data[top].x, *y = h.data[top].y, *z = h.data[top].z;
}