//
// Created by MashPlant on 18-5-15.
//

#ifndef MST_GENERATOR_H
#define MST_GENERATOR_H


#include "BaseTest.h"

class GeneratorTest : public BaseTest
{
	std::function<void(Point2d &)> gen;

public:
	template<typename Func>
	GeneratorTest(int N, Func &&f) : BaseTest(N), gen(std::forward<Func>(f))
	{}

	void generate() override
	{
		for (int i = 0; i < N; ++i)
			gen(input[i]);
	}
};


#endif //MST_GENERATOR_H
