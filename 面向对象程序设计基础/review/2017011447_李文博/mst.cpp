#include "mst.h"
#include <cstring>
#include <iostream>
using namespace std;

mst::mst() {
	memset(marked, 0, sizeof(marked));
	marked[0] = true;
	edgenumber = 0;
}

void mst::computeMST(float weight[n][n]){
	while (edgenumber < n - 1) {//边数达到n-1则停止
		int min = INF;
		int id[2];//存放最短边两个端点的编号
		for (int i = 0; i < n; i++) {
			if (marked[i] == true) {//如果该点已被访问，则从该点遍历找最短边
				for (int j = 0; j < n; j++) {
					if (weight[i][j] < min && marked[j] == false) {//某一边权值较小且另一端点未被访问过
						min = weight[i][j];
						id[0] = i;
						id[1] = j;
					}
				}
			}
		}
		result[edgenumber][0] = id[0];
		result[edgenumber][1] = id[1];
		marked[id[1]] = true;//另一端点已被访问
		edgenumber++;
	}
	output();
}

void mst::output() {
	for (int i = 0; i < n - 1; i++) {
		cout << result[i][0] << "---" << result[i][1] << endl;
	}
}