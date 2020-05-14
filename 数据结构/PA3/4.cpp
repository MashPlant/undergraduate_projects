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
	if (last - first <= 8)
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

// [first, last - 1] is a min heap, except for first and its 2 children may violate heap rule
template <typename It>
void down(It first, It last)
{
	int n = last - first, fa = 0, son = 1;
	auto val = first[0];
	while (true)
	{
		if (son + 1 < n && first[son + 1] < first[son])
			++son;
		if (son < n && first[son] < val)
			first[fa] = first[son], fa = son, son = (fa << 1) + 1;
		else
			break;
	}
	first[fa] = val;
}

int n, s1[N], s2[N], s3[N];

struct Data3
{
	int x, y, z, num;

	bool operator<(const Data3 &rhs) const
	{
		return compare(x, y, z, rhs.x, rhs.y, rhs.z);
	}
} h[N];

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
} cache[K], h[N];

int count;

Data2 get(int k)
{
	if (k <= count)
		return cache[k];
	Data2 cur = h[1];
	h[1] = {cur.y, s3[cur.num + 1], cur.num + 1};
	down(h + 1, h + n + 1);
	return cache[count = k] = cur;
}

void init()
{
	for (int i = 1; i <= n; ++i)
		h[i] = {s2[i], s3[1], 1};
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
		h[i] = {s1[i], s2[1], s3[1], 1};
	rem::init();
	while (--k)
	{
		Data3 cur = h[1];
		rem::Data2 nxt = rem::get(cur.num + 1);
		h[1] = {cur.x, nxt.y, nxt.z, cur.num + 1};
		down(h + 1, h + n + 1);
	}
	*x = h[1].x, *y = h[1].y, *z = h[1].z;
}