#ifndef MST_H
#define MST_H

#include "head.h"

class mst {
public:
	mst();
	void computeMST(float weight[n][n]);
	void output();
private:
	bool marked[n];//��¼���Ƿ񱻷��ʹ�
	int edgenumber;//��¼��ѡȡ�ı���
	int result[n - 1][2];//�洢���ս��
};

#endif
