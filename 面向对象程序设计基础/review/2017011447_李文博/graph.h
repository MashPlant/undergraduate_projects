#ifndef GRAPH_H
#define GRAPH_H

#include "head.h"

class graph {
public:
	graph(int point[n][2]);
	void getweight(float weight[n][n]);
private:
	struct point {
		int x;
		int y;
	} mypoint[n];
	float weight[n][n];//��¼Ȩ����
};

#endif // !GRAPH_H

