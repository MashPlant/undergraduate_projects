//
// Created by MashPlant on 18-5-15.
//

#include "BaseTest.h"

template<typename MSTSolver>
void BaseTest::solve(const char *info)
{
	std::cout << info << std::endl;
	auto beg = now();
	MSTSolver solver(input);
	double ans = solver();
	auto time = std::chrono::duration<double>(now() - beg).count();
	std::cout << "use time : " << time << std::endl
	          << "result : " << ans << std::endl
	          << std::endl;
	results.emplace_back(info, ans, time);
}

void BaseTest::solveAll()
{
	solve<DelaunayMST>("delaunay triangulation mst method");
	solve<YaoMST>("yao mst method");
	solve<PrimMST<>>("naive prim method");
	solve<PrimMST<FibHeap>>("prim method with fibonacci heap\n"
	                        "works well on dense graph (O(E+VlgV))\n"
	                        "but still can't match delaunay because the geometry property is not used");
	solve<PrimMST<PairingHeap>>("prim method with pairing heap, as above");
	try
	{
		solve<PrimMST<BinaryHeap>>("prim method with binary heap\n"
		                           "perform worse on dense graph\n"
		                           "might cause MLE");
	}
	catch (const std::bad_alloc &)
	{
		std::cout << "prim with binary heap failed to calculate mst because of n^2 memory use" <<
		          std::endl;
	}
	try
	{
		solve<KruskalMST>("naive kruskal method\n"
		                  "might cause MLE");
	}
	catch (const std::bad_alloc &)
	{
		std::cout << "kruskal failed to calculate mst because of n^2 memory use" <<
		          std::endl;
	}
}

bool BaseTest::checkEqual()
{
	auto deq = [](double a, double b)
	{ return std::abs(a - b) < 1e-3; };
	for (int i = 1; i < results.size(); ++i)
		if (!deq(results[i].result, results[0].result))
			return false;
	return true;
}

