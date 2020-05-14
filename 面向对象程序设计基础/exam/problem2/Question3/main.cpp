#include <iostream>
#include <iterator>
#include <algorithm>
#include "lists.h"
#include "pred.h"
using namespace std;

int main()
{
	//For testing longest increasing subsequence
	const char* numArr[] = {"adventure", "hub", "earth", "diffract", "advantage","diffuse", "eye", "mantle", "huff",  "malice"};
	const char* numArr1[] = {"imitat", "play", "rare", "much","muck","bowsprit","inapt","diffuse","box", "raf"};
	//For testing longest non-decreasing subsequence
	const char* numArr2[] = {"foot", "much", "foot", "mantle", "earth", "huff", "earth", "faddish","hub", "malice"};
	const char* numArr3[] = {"brake", "play", "brake", "impact", "brake", "advantage", "however", "brake", "however", "play"};

	vector<const char *> numVec;
    numVec.reserve(10);
    numVec.assign(&numArr[0], &numArr[10]);
	copy(numVec.begin(), numVec.end(), ostream_iterator<const char *>(cout, " "));
	cout << endl;
	
	vector<int> posIndex;
	LISTS<const char *, PredI> lis;
	lis.solve(numVec, posIndex);
	for (int it : posIndex)
		cout << numVec[it] << ' ';
	cout << endl;
	
	vector<int> posIndex2;
	LISTS<const char *, PredND> lis2;
	lis2.solve(numVec, posIndex2);
	for (int it : posIndex2)
		cout << numVec[it] << ' ';
	cout << endl;

	return 0;
}