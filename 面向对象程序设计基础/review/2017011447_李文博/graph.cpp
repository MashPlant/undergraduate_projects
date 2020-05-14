#include "graph.h"
#include <cmath>
#include <iostream>
using namespace std;

graph::graph(int point[n][2]) {
	for (int i = 0; i < n; i++) {
		mypoint[i].x = point[i][0];
		mypoint[i].y = point[i][1];
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			weight[i][j] = sqrt((mypoint[i].x - mypoint[j].x) * (mypoint[i].x - mypoint[j].x) + (mypoint[i].y - mypoint[j].y) * (mypoint[i].y - mypoint[j].y));
		}
		weight[i][i] = INF;
	}
	for (int i = 0; i < n; i++) {
		cout << i << ": (" << mypoint[i].x << "," << mypoint[i].y << ")\n";
	}
}

void graph::getweight(float weight[n][n]) {
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			weight[i][j] = this->weight[i][j];
}