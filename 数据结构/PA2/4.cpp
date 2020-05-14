#include "sort.h"
#include <cstring>
#pragma GCC optimize("O3")

int aux[1000005];

template <typename T>
void swap(T &a, T &b)
{
	T c = a;
	a = b;
	b = c;
}

void merge_sort(int *l, int *r)
{
	int n = r - l, min, max;
	switch (n)
	{
	case 0:
	case 1:
		break;
	case 2:
	{
		compare(l[0], l[1], l[1], &max, &min);
		l[0] = min, l[1] = max;
	}
	break;
	case 3:
	{
		compare(l[0], l[1], l[2], &max, &min);
		if (l[0] != min)
			swap(l[0], l[1] == min ? l[1] : l[2]);
		if (l[2] != max)
			swap(l[1], l[2]);
	}
	break;
	default:
	{
		int n4 = n / 4, not_min;
		int *pos[4] = {l, l + n4, l + 2 * n4, l + 3 * n4};
		int *end[4] = {l + n4, l + 2 * n4, l + 3 * n4, l + n};
		int *dst = aux;
		for (int i = 0; i < 4; ++i)
			merge_sort(pos[i], end[i]);
		compare(*pos[0], *pos[1], *pos[2], &max, &min);
		not_min = max == *pos[0] ? 0 : max == *pos[1] ? 1 : 2;
		while (pos[0] != end[0] && pos[1] != end[1] && pos[2] != end[2] && pos[3] != end[3])
		{
			swap(pos[not_min], pos[3]), swap(end[not_min], end[3]);
			compare(*pos[0], *pos[1], *pos[2], &max, &min);
			not_min = max == *pos[0] ? 0 : max == *pos[1] ? 1 : 2;
			*dst++ = *pos[min == *pos[0] ? 0 : min == *pos[1] ? 1 : 2]++;
		}
		if (pos[0] == end[0]) swap(pos[0], pos[3]), swap(end[0], end[3]);
		else if (pos[1] == end[1]) swap(pos[1], pos[3]), swap(end[1], end[3]);
		else if (pos[2] == end[2]) swap(pos[2], pos[3]), swap(end[2], end[3]);
		while (pos[0] != end[0] && pos[1] != end[1] && pos[2] != end[2])
		{
			compare(*pos[0], *pos[1], *pos[2], &max, &min);
			*dst++ = *pos[min == *pos[0] ? 0 : min == *pos[1] ? 1 : 2]++;
		}
		if (pos[0] == end[0]) swap(pos[0], pos[2]), swap(end[0], end[2]);
		else if (pos[1] == end[1]) swap(pos[1], pos[2]), swap(end[1], end[2]);
		while (pos[0] != end[0] && pos[1] != end[1])
		{
			compare(*pos[0], *pos[1], *pos[1], &max, &min);
			*dst++ = *pos[min == *pos[0] ? 0 : 1]++;
		}
		if (pos[0] == end[0]) swap(pos[0], pos[1]), swap(end[0], end[1]);
		memcpy(dst, pos[0], sizeof(int) * (end[0] - pos[0])), dst += end[0] - pos[0];
		memcpy(l, aux, sizeof(int) * (dst - aux));
	}
	break;
	}
}

void sort(int n, int limit, int *a)
{
	merge_sort(a, a + n);
}