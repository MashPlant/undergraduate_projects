//
// Created by MashPlant on 18-5-7.
//

#ifndef MST_PRIMMST_H
#define MST_PRIMMST_H

#include <vector>
#include <memory>
#include <algorithm>
#include "Point2d.h"
#include "HeapTraits.h"
#include "FibHeap.h"

template<template<typename> typename HeapType = NoHeap,
		bool = IsDecreaseableHeapV<HeapType<int>>>
class PrimMST;

template<template<typename> typename NormalHeap>
class PrimMST<NormalHeap, false>
{
	std::vector<Point2d> pts;
	using Heap = NormalHeap<std::pair<double, int>>;

public:
	PrimMST(const std::vector<Point2d> &points) : pts(points)
	{}

	double operator()()
	{
		const int n = static_cast<int>(pts.size());
		Heap heap;
		std::unique_ptr<bool[]> vis(new bool[n]{});
		std::unique_ptr<double[]> dis(new double[n]{});
		std::fill(dis.get(), dis.get() + n, std::numeric_limits<double>::max());
		double ans = 0;
		dis[0] = 0.0, heap.push({0.0, 0});
		while (!heap.empty())
		{
			auto[cur, id] = heap.top();
			heap.pop();
			double chkMin;
			if (vis[id])
				continue;
			vis[id] = true;
			ans += cur;
			for (int i = 0; i < n; ++i)
				if (!vis[i] && (chkMin = (pts[id] - pts[i]).length()) < dis[i])
				{
					dis[i] = chkMin;
					heap.push({dis[i], i});
				}
		}
		return ans;
	}
};

template<template<typename> typename DecreaseableHeap>
class PrimMST<DecreaseableHeap, true>
{
	std::vector<Point2d> pts;
	using Heap = DecreaseableHeap<std::pair<double, int>>;
	using Node = typename Heap::Node;
public:
	PrimMST(const std::vector<Point2d> &points) : pts(points)
	{}

	double operator()()
	{
		const int n = static_cast<int>(pts.size());
		Heap heap;
		std::unique_ptr<bool[]> vis(new bool[n]{});
		std::unique_ptr<Node *[]> nodes(new Node *[n]{});
		nodes[0] = heap.push({0.0, 0});
		for (int i = 1; i < n; ++i)
			nodes[i] = heap.push({std::numeric_limits<double>::max(), i});
		double ans = 0, chkMin;
		while (!heap.empty())
		{
			auto[cur, id] = heap.top();
			heap.pop();
			vis[id] = true;
			ans += cur;
			for (int i = 0; i < n; ++i)
				if (!vis[i] && (chkMin = (pts[id] - pts[i]).length()) < nodes[i]->getKey().first)
					heap.decKey(nodes[i], {chkMin, i});
		}
		return ans;
	}
};


template<>
class PrimMST<NoHeap>
{
	std::vector<Point2d> pts;

public:
	PrimMST(const std::vector<Point2d> &points) : pts(points)
	{}

	double operator()()
	{
		const int n = static_cast<int>(pts.size());
		std::unique_ptr<bool[]> vis(new bool[n]{});
		std::unique_ptr<double[]> dis(new double[n]{});
		std::fill(dis.get(), dis.get() + n, std::numeric_limits<double>::max());
		double ans = 0;
		dis[0] = 0.0;
		for (int _ = 0; _ < n; ++_)
		{
			double mn = std::numeric_limits<double>::max();
			int id = -1;
			for (int i = 0; i < n; ++i)
				if (!vis[i] && mn > dis[i])
					mn = dis[i], id = i;
			ans += mn;
			vis[id] = true;
			for (int i = 0; i < n; ++i)
				if (!vis[i])
					dis[i] = std::min(dis[i], (pts[id] - pts[i]).length());
		}
		return ans;
	}
};

#endif //MST_PRIMMST_H
