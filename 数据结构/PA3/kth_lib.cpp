#include "kth.h"
#include <stdlib.h>
#include <stdio.h>

static int *a;
static int *b;
static int *c;
static int n;

int compare(int x, int y, int z, int u, int v, int w) {
	if (x < 1 || x > n || y < 1 || y > n || z < 1 || z > n) {
		puts("Wrong Answer --- invalid parameters");
		exit(0);
	}
	
	if (u < 1 || u > n || v < 1 || v > n || w < 1 || w > n) {
		puts("Wrong Answer --- invalid parameters");
		exit(0);
	}
	
	return a[x] + b[y] + c[z] < a[u] + b[v] + c[w];
}

int main() {
	int n, k;
	scanf("%d%d", &n, &k);
	::n = n;
	
	a = new int[n + 1];
	b = new int[n + 1];
	c = new int[n + 1];
	
	for (int i = 1; i <= n; i++) {
		scanf("%d", a + i);
	}
	
	for (int i = 1; i <= n; i++) {
		scanf("%d", b + i);
	}
	
	for (int i = 1; i <= n; i++) {
		scanf("%d", c + i);
	}
	
	int x = -1, y = -1, z = -1;
	get_kth(n, k, &x, &y, &z);
	
	if (x < 1 || x > n || y < 1 || y > n || z < 1 || z > n) {
		puts("Wrong Answer --- invalid answer");
		return 0;
	}
	
	printf("%d\n", a[x] + b[y] + c[z]);
}
