//
// Created by MashPlant on 18-5-15.
//

#ifndef MST_BASETEST_H
#define MST_BASETEST_H

#include <string_view>
#include <vector>
#include <chrono>
#include <iostream>

#include "../src/Point2d.h"
#include "../src/YaoMST.h"
#include "../src/PrimMST.h"
#include "../src/KruskalMST.h"
#include "../src/PairingHeap.h"
#include "../src/DelaunayMST.h"
#include "../src/FibHeap.h"

class BaseTest
{
	constexpr static auto &now = std::chrono::system_clock::now;

	struct Result
	{
		std::string_view name;
		double result;
		decltype((now() - now()).count()) time;

		Result(std::string_view name, double result, decltype(time) time)
			: name(name), result(result), time(time)
		{
		}
	};

	template <typename MSTSolver>
	void solve(const char *info);

  protected:
	int N;
	std::vector<Result> results;
	std::vector<Point2d> input;

  public:
	virtual void generate() = 0;

	BaseTest(int N) : N(N), input(N)
	{
	}

	void solveAll();

	bool checkEqual();

	virtual ~BaseTest() = default;

	template <typename Test, typename... Args>
	static std::unique_ptr<BaseTest>
	createTest(Args... args)
	{
		std::unique_ptr<BaseTest> ret(new Test(std::forward<Args>(args)...));
		ret->generate();
		return ret;
	}
};

#endif //MST_BASETEST_H
