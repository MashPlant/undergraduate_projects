//
// Created by MashPlant on 18-5-15.
//

#ifndef MST_DISTINCTINT_H
#define MST_DISTINCTINT_H

#include <set>
#include <random>
#include "BaseTest.h"

class DistinctIntTest : public BaseTest
{
	int lb, ub;
public:
	DistinctIntTest(int N, int lb, int ub) : BaseTest(N), lb(lb), ub(ub)
	{}

	void generate() override
	{
		std::set<std::pair<int, int>> record;
		std::mt19937 mt;
		std::uniform_int_distribution<> dis(lb, ub);
		while (record.size() != N)
			record.insert({dis(mt), dis(mt)});
		auto it = record.begin();
		for (int i = 0; i < N; ++i, ++it)
			input[i].x = it->first, input[i].y = it->second;
	}
};

#endif //MST_DISTINCTINT_H
