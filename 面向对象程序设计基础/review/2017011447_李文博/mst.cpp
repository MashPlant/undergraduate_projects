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
	while (edgenumber < n - 1) {//�����ﵽn-1��ֹͣ
		int min = INF;
		int id[2];//�����̱������˵�ı��
		for (int i = 0; i < n; i++) {
			if (marked[i] == true) {//����õ��ѱ����ʣ���Ӹõ��������̱�
				for (int j = 0; j < n; j++) {
					if (weight[i][j] < min && marked[j] == false) {//ĳһ��Ȩֵ��С����һ�˵�δ�����ʹ�
						min = weight[i][j];
						id[0] = i;
						id[1] = j;
					}
				}
			}
		}
		result[edgenumber][0] = id[0];
		result[edgenumber][1] = id[1];
		marked[id[1]] = true;//��һ�˵��ѱ�����
		edgenumber++;
	}
	output();
}

void mst::output() {
	for (int i = 0; i < n - 1; i++) {
		cout << result[i][0] << "---" << result[i][1] << endl;
	}
}