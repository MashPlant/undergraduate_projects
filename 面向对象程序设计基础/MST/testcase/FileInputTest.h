//
// Created by MashPlant on 18-5-17.
//

#ifndef MST_FILEINPUTTEST_H
#define MST_FILEINPUTTEST_H

#include <fstream>
#include "BaseTest.h"

class FileInputTest : public BaseTest
{
	std::ifstream fin;

	static int getCount(const std::string &file)
	{
		std::ifstream fin(file);
		int N;
		fin >> N;
		return N;
	}

public:
	FileInputTest(const std::string &file) : BaseTest(getCount(file)), fin(file)
	{}

	void generate() override
	{
		fin >> N;
		for (int i = 0; i < N; ++i)
		{
			double x, y;
			fin >> x >> y;
			input[i].x = x, input[i].y = y;
		}
	}
};

#endif //MST_FILEINPUTTEST_H
