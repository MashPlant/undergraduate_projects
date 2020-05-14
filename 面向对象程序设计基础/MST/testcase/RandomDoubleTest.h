//
// Created by MashPlant on 18-5-15.
//

#ifndef MST_RANDOMDOUBLE_H
#define MST_RANDOMDOUBLE_H

#include <random>
#include "BaseTest.h"

class RandomDoubleTest : public BaseTest
{
	double lb, ub;
public:
	RandomDoubleTest(int N, double lb, double ub) : BaseTest(N), lb(lb), ub(ub)
	{}

	void generate() override
	{
		std::mt19937 mt;
		std::uniform_real_distribution<> dis(lb, ub);
		for (int i = 0; i < N; ++i)
			input[i].x = dis(mt), input[i].y = dis(mt);
	}
};

#endif //MST_RANDOMDOUBLE_H
