#include "mst.h"
#include "graph.h"

#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

int main() {
	srand((unsigned)time(NULL));
	int point[n][2];
	for (int i = 0; i < n; i++) {
		point[i][0] = rand() % 10001;
		point[i][1] = rand() % 10001;
		for (int j = 0; j < i; j++) {
			if (point[i][0] == point[j][0] && point[i][1] == point[j][1]) {
				i--;
				break;//·ÀÖ¹µãÖØºÏ
			}
		}
	}
	graph g(point);
	float weight[n][n];
	g.getweight(weight);
	mst compute;
	compute.computeMST(weight);
}