#include <iostream>
#include <iterator>
#include <algorithm>
#include "lis.h"
using namespace std;

int main()
{
	int numArr[] = {89, 85, 78, 85, 82, 101, 81, 82, 90, 77};
	int numArr2[] = {76, 89, 84, 87, 84, 87, 87, 79, 84, 85};
	vector<int> numVec;
	numVec.reserve(10);
	numVec.assign(&numArr[0], &numArr[10]);
	copy(numVec.begin(), numVec.end(), ostream_iterator<int>(cout, " "));
	cout << endl;

	vector<int> posIndex;
	LIS lis;
	lis.solve(numVec, posIndex);
	for (int it : posIndex)
		cout << numVec[it] << ' ';
	cout << endl;

	return 0;
}
