//
// Created by MashPlant on 18-5-9.
//

#ifndef MST_DELAUNAYMST_H
#define MST_DELAUNAYMST_H

#include <algorithm>
#include <memory>
#include "KruskalMST.h"

class DelaunayMST : public KruskalMST
{
	struct EListNode
	{
		int to = 0, prev = 0, next = 0;
	};
	std::unique_ptr<EListNode[]> e;

	struct Point3d
	{
		double x, y, z;

		Point3d operator-(const Point3d &rhs) const
		{ return {x - rhs.x, y - rhs.y, z - rhs.z}; }

		static double dot(const Point3d &lhs, const Point3d &rhs)
		{ return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; }

		static Point3d cross(const Point3d &a, const Point3d &b)
		{ return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
	};

	std::unique_ptr<int[]> last, stk;
	std::unique_ptr<Point2d[]> p;
	int eCnt = 1;

	struct IndexPoint2d : Point2d
	{
		int index;

		IndexPoint2d() = default;

		IndexPoint2d(const Point2d &p, int index) : Point2d(p), index(index)
		{}
	};

	template<typename Point>
	auto cross(const Point &a, const Point &b, const Point &c)
	{
		// for both 2d and 3d
		return Point::cross(b - a, c - a);
	}

	// judge whether segment (p[a],p[b]) intersect with with (p[c],p[d])
	bool isIntersect(int a, int b, int c, int d)
	{
		return cross(p[a], p[c], p[d]) * cross(p[b], p[c], p[d]) < 0 &&
		       cross(p[c], p[a], p[b]) * cross(p[d], p[a], p[b]) < 0;
	}

	// judge whether p[d] is in the circumcircle of p[a],p[b],p[c]
	bool isInCircle(int a, int b, int c, int d)
	{
		auto mapToHiDim = [](const Point2d &p2)
		{ return Point3d{p2.x, p2.y, p2.x * p2.x + p2.y * p2.y}; };
		Point3d pa = mapToHiDim(p[a]), pb = mapToHiDim(p[b]), pc = mapToHiDim(p[c]), pd = mapToHiDim(p[d]);
		if (cross(p[a], p[b], p[c]) < 0)
			std::swap(pb, pc);
		return Point3d::dot(cross(pa, pb, pc), (pd - pa)) < 0;
	}

	// add two edge into the linked list
	void add(int x, int y)
	{
		e[++eCnt] = {y, last[x]};
		e[last[x]].next = eCnt, last[x] = eCnt;
		e[++eCnt] = {x, last[y]};
		e[last[y]].next = eCnt, last[y] = eCnt;
	}

	// remove a node from linked list
	void del(int x)
	{
		e[e[x].next].prev = e[x].prev;
		e[e[x].prev].next = e[x].next;
		last[e[x ^ 1].to] == x ? last[e[x ^ 1].to] = e[x].prev : 1;
	}

	// delaunay triangulation
	void delaunay(int l, int r)
	{
		if (r - l <= 2)
		{
			for (int i = l; i < r; ++i)
				for (int j = i + 1; j <= r; ++j)
					add(i, j);
			return;
		}
		int i, mid = l + r >> 1, ld = 0, rd = 0, tot = 0;
		delaunay(l, mid), delaunay(mid + 1, r);
		// find the convex hull
		for (i = l; i <= r; stk[++tot] = i++)
			while (tot > 1 && cross(p[stk[tot - 1]], p[stk[tot]], p[i]) < 0)
				tot--;
		// find the lowest common tangent for convex hull
		for (i = 1; i < tot && !ld; ++i)
			if (stk[i] <= mid && mid < stk[i + 1])
				ld = stk[i], rd = stk[i + 1];
		for (; add(ld, rd), 1;)
		{	
			/*
然后就是连接这个点id，如果id原来在左边就连边(id,rd),删掉之前端点为ld，与(id,rd)有交的边，然后更新ld=id。id在右边同理。
			*/
			// check all the point connected to ld & rd
			// there must be one `id` which make circle (ld, rd, id) doesn't contain any point connected to ld & rd 
			// so, if the prev `id` is in the circle (ld, rd, id'), make id = id'
			int id = 0, op = 0;
			for (i = last[ld]; i; i = e[i].prev)
				if (cross(p[ld], p[rd], p[e[i].to]) > 0)
					if (!id || isInCircle(ld, rd, id, e[i].to))
						op = -1, id = e[i].to;
			for (i = last[rd]; i; i = e[i].prev)
				if (cross(p[rd], p[ld], p[e[i].to]) < 0)
					if (!id || isInCircle(ld, rd, id, e[i].to))
						op = 1, id = e[i].to;
			// then connect id and [ld or rd] in the triangulation
			// if id is originally connected to ld, then connect id with rd
			// and delete all edges containing ld and intersecting with edge (id, rd)
			// vice versa
			if (op == 0)
				break;
			if (op == -1)
			{
				for (i = last[ld]; i; i = e[i].prev)
					if (isIntersect(rd, id, ld, e[i].to))
						del(i), del(i ^ 1), i = e[i].next;
				ld = id;
			}
			else
			{
				for (i = last[rd]; i; i = e[i].prev)
					if (isIntersect(ld, id, rd, e[i].to))
						del(i), del(i ^ 1), i = e[i].next;
				rd = id;
			}
		}
	}

public:
	DelaunayMST(const std::vector<Point2d> &points)
			: last(new int[points.size() + 1]{}), stk(new int[points.size() + 1]{}),
			  p(new Point2d[points.size() + 1]{}), e(new EListNode[(points.size() + 1) * 32]{})
	{
		n = static_cast<int>(points.size());
		std::unique_ptr<IndexPoint2d[]> ori(new IndexPoint2d[points.size() + 1]{});
		for (int i = 1; i <= n; ++i)
			p[i] = points[i - 1], ori[i] = {points[i - 1], i};

		auto pointCmp = [](auto &lhs, auto &rhs)
		{ return lhs.x < rhs.x || lhs.x == rhs.x && lhs.y < rhs.y; };
		std::sort(p.get() + 1, p.get() + 1 + n, pointCmp);
		std::sort(ori.get() + 1, ori.get() + 1 + n, pointCmp);

		delaunay(1, n);

		for (int i = 1; i <= n; ++i)
			for (int j = last[i]; j; j = e[j].prev)
				addEdge(ori[i].index - 1, ori[e[j].to].index - 1, (p[i] - p[e[j].to]).length());
		std::sort(edges.begin(), edges.end());
	}
};

#endif //MST_DELAUNAYMST_H
