//
// Created by MashPlant on 18-5-6.
//

#ifndef MST_YAOMST_H
#define MST_YAOMST_H

#include <vector>
#include <algorithm>
#include <cmath>
#include "UFS.h"
#include "KDTree.h"
#include "Point2d.h"
#include "Mat22.h"
#include "KruskalMST.h"

class YaoMST : public KruskalMST
{
	struct PointPair
	{
		Point2d oPoint;
		Point2d nPoint;
		int index;
	};

	struct NewAt
	{
		double operator()(const PointPair &pointPair, int pos) const
		{
			return pointPair.nPoint[pos];
		}
	};

	struct OldAt
	{
		double operator()(const PointPair &pointPair, int pos) const
		{
			return pointPair.oPoint[pos];
		}
	};

	struct Cell
	{
		KDTree<PointPair, 2, OldAt> oBlock;
		KDTree<PointPair, 2, NewAt> nBlock;
	};

public:
	explicit YaoMST(const std::vector<Point2d> &points, double sFactor = 0.13)
	{
		n = static_cast<int>(points.size());
		const double s = std::pow(n, sFactor);
		const int step1 = static_cast<int>(n / s);
		const int step2 = static_cast<int>(step1 / s);
		const double sq3 = std::sqrt(3);
		const Mat22 trans[3] = {{sq3,  -1, 0,   2},
		                        {-sq3, 1,  sq3, 1},
		                        {-sq3, -1, 0,   2}};
		// two copy for points
		// oldTree for KDTree of old coordinate
		// newTree for KDTree of new coordinate
		// KDTree is designed to not manage memory, but use provided memory
		// to reduce memory fragmentation
		std::vector<PointPair> oldTree(points.size()), newTree(points.size());
		std::vector<int> oldSplit(points.size()), newSplit(points.size());
		for (auto &tran : trans)
		{
			for (int i = 0; i < n; ++i)
			{
				oldTree[i].oPoint = points[i];
				oldTree[i].nPoint = tran * points[i];
				oldTree[i].index = i;
			}
			std::sort(oldTree.begin(), oldTree.end(), [](const PointPair &lhs, const PointPair &rhs)
			{ return lhs.nPoint.x < rhs.nPoint.x; });
			std::vector<Cell> cells;
			for (int i = 0; i < n; i += step1)
			{
				const int up = std::min(i + step1, n);
				std::sort(oldTree.begin() + i, oldTree.begin() + up, [](const PointPair &lhs, const PointPair &rhs)
				{ return lhs.nPoint.y > rhs.nPoint.y; });
			}

			newTree = oldTree; // vector's operator= has been optimized, this will not allocate memory
			for (int i = 0; i < n; i += step1)
			{
				const int up = std::min(i + step1, n);
				for (int j = i; j < up; j += step2)
					cells.push_back(
							{{oldTree.data() + j, oldTree.data() + std::min(j + step2, up), oldSplit.data() + j},
							 {newTree.data() + j, newTree.data() + std::min(j + step2, up), newSplit.data() + j}});
			}
			for (auto &pointPair : oldTree)
			{
				double dis2 = std::numeric_limits<double>::max();
				int to = -1;
				auto checkMin = [&](int index, double newDis2)
				{
					if (newDis2 < dis2)
						to = index, dis2 = newDis2;
				};
				for (auto &cell : cells)
				{
					if (cell.nBlock.allGreater(pointPair))
					{
						auto result = cell.oBlock.nearest(pointPair);
						checkMin(result.point.index, result.dis2);
					}
					else if (cell.nBlock.anyGreater(pointPair))
					{
						cell.nBlock.forEach([&](const PointPair &pr)
						                    {
							                    if (pr.index != pointPair.index &&
							                        pr.nPoint.x >= pointPair.nPoint.x &&
							                        pr.nPoint.y >= pointPair.nPoint.y)
								                    checkMin(pr.index, Point2d::dis2(pr.oPoint, pointPair.oPoint));
						                    });
					}
				}
				if (to != -1)
					addEdge(pointPair.index, to, std::sqrt(dis2));
			}
		}
		std::sort(edges.begin(), edges.end());
	}
};

#endif //MST_YAOMST_H
