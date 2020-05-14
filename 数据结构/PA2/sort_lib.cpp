#include "sort.h"

#include <stdio.h>
#include <stdlib.h>
// #define NDEBUG
#include <assert.h>

#define debug(...) fprintf(stderr, __VA_ARGS__)
// #define debug(...)

template <class T>
inline void swap(T &a, T &b)
{
	T c = a;
	a = b;
	b = c;
}

static int n;
static int limit;

static int *A;
static int cmp_cnt = 0;

void compare(int a, int b, int c, int *max, int *min)
{
	if (++cmp_cnt > limit)
	{
		printf("Wrong Answer --- exceeded limit\n");
		exit(0);
	}

	if (a < 0 || a >= n || b < 0 || b >= n || c < 0 || c >= n)
	{
		printf("Wrong Answer --- invalid parameters\n");
		exit(0);
	}

	if (A[a] > A[b])
		swap(a, b);
	*max = A[b] > A[c] ? b : c;
	*min = A[a] < A[c] ? a : c;
}

int main()
{
	assert(scanf("%d%d", &n, &limit) == 2);
	assert(n > 0);
	assert(limit > 0);
	printf("%d %d\n", n, limit);
	A = new int[n];
	for (int i = 0; i < n; i++)
	{
		scanf("%d", A + i);
	}

	int *res = new int[n];
	for (int i = 0; i < n; i++)
	{
		res[i] = i;
	}

	sort(n, limit, res);
	for (int i = 0; i < n; i++)
	{
		if (res[i] < 0 || res[i] >= n)
		{
			printf("Wrong Answer --- res[%d] = %d not in [0, n)\n", i, res[i]);
			exit(0);
		}
		if (A[res[i]] != i)
		{
			printf("Wrong Answer --- not sorted\n");
			exit(0);
		}
	}

	puts("Accepted");
	debug("cmp_cnt = %d\n", cmp_cnt);
}
