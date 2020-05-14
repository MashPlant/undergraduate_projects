//
// Created by MashPlant on 18-5-8.
//

#ifndef MST_KRUSKALMST_H
#define MST_KRUSKALMST_H

#include <vector>
#include <algorithm>
#include "UFS.h"
#include "Point2d.h"

class KruskalMST
{
	struct Edge
	{
		int from, to;
		double w;

		bool operator<(const Edge &rhs) const
		{
			return w < rhs.w;
		}
	};

protected:
	KruskalMST() = default;

	std::vector<Edge> edges;
	int n;

	void addEdge(int from, int to, double w)
	{ edges.push_back({from, to, w}); }

public:
	KruskalMST(const std::vector<Point2d> &points)
	{
		n = static_cast<int>(points.size());
		edges.resize(n * (n - 1) / 2);
		int pos = 0;
		for (int i = 0; i < n; ++i)
			for (int j = i + 1; j < n; ++j)
				edges[pos++] = {i, j, (points[i] - points[j]).length()};
		std::sort(edges.begin(), edges.end());
	}

	double operator()() const
	{
		double ans = 0.0;
		int need = n;
		UFS ufs(n);
		for (auto &edge : edges)
		{
			if (!ufs.connected(edge.from, edge.to))
			{
				ufs.connect(edge.from, edge.to);
				ans += edge.w;
				if (--need == 1)
					break;
			}
		}
		return ans;
	}
};

#endif //MST_KRUSKALMST_H
