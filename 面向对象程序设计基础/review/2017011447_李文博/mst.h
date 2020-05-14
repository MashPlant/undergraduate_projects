#ifndef MST_H
#define MST_H

#include "head.h"

class mst {
public:
	mst();
	void computeMST(float weight[n][n]);
	void output();
private:
	bool marked[n];//记录点是否被访问过
	int edgenumber;//记录已选取的边数
	int result[n - 1][2];//存储最终结果
};

#endif
