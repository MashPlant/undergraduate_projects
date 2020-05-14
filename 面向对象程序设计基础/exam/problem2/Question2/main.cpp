#include <iostream>
#include <iterator>
#include <algorithm>
#include "list.h"
#include "student.h"
using namespace std;

int main()
{
	Student numArr[] = {Student(80, 81), Student(83, 79), Student(82, 80), Student(82, 85), Student(82, 75)};
	Student numArr2[] = {Student(76, 89), Student(89, 85), Student(84, 78), Student(87, 85), Student(84, 82), Student(87, 76), Student(87, 81), Student(79, 82), Student(84, 90), Student(85, 87)};

	vector<Student> numVec;
	numVec.reserve(10);
	numVec.assign(&numArr[0], &numArr[10]);
	copy(numVec.begin(), numVec.end(), ostream_iterator<Student>(cout, " "));
	cout << endl;

	vector<int> posIndex;
	LIST<Student> lis;
	lis.solve(numVec, posIndex);
	for (int it : posIndex)
		cout << numVec[it] << ' ';
	cout << endl;

	return 0;
}
