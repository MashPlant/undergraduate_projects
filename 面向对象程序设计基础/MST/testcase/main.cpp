#include <random>
#include <string>
#include <iostream>

#include "RandomDoubleTest.h"
#include "DistinctIntTest.h"
#include "GeneratorTest.h"
#include "FileInputTest.h"

int main(int argc, char **argv)
{
	int n = 20000;
	if (argc >= 2)
		try
		{
			n = std::stoi(argv[1]);
		}
		catch (const std::invalid_argument &)
		{
			std::cout << "the input n is invalid" << std::endl;
			return 1;
		}
	std::mt19937 mt;
	std::uniform_real_distribution<> dis(-1000.0, 1000.0);

	auto tests =
		{
			BaseTest::createTest<RandomDoubleTest>(n, 0.0, 10000.0),
			BaseTest::createTest<DistinctIntTest>(n, 0, 10000),
			BaseTest::createTest<GeneratorTest>(n, [&](Point2d &p) { return p.x = p.y = dis(mt); }),
			BaseTest::createTest<FileInputTest>("../testcase/in.txt"),
		};
	for (auto &test : tests)
	{
		test->solveAll();
		std::cout << (test->checkEqual() ? "\npass\n" : "\nfail\n") << std::endl;
	}
}
