//
// Created by MashPlant on 18-5-5.
//

#ifndef MST_KDTREE_H
#define MST_KDTREE_H

#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

template <typename Point>
struct DefaultCoordinate
{
	double operator()(const Point &point, int pos) const
	{
		return point[pos];
	}
};

template <typename Point, int K,
		  typename Coordinate = DefaultCoordinate<Point>>
class KDTree
{
	constexpr static double eps = 1e-7;
	Point *first, *last;
	int *split; // save the split result for nearest
	const Coordinate coordinate;

	double min[K], max[K];

	void build(Point *l, Point *r) // [0,n) indexed
	{
		if (l == r)
			return;
		auto mid = l + ((r - l) >> 1);
		auto midIndex = mid - first;
		// get the variance of each dimension
		split[midIndex] = 0;
		double var = 0.0;
		for (int d = 0; d < K; ++d) // find the dimension with biggest variance
		{
			double ave = 0.0, tmpVar = 0.0;
			for (auto i = l; i < r; ++i)
				ave += coordinate(*i, d);
			ave /= r - l;
			for (auto i = l; i < r; ++i)
				tmpVar += pow(coordinate(*i, d) - ave, 2);
			if (tmpVar > var)
				var = tmpVar, split[midIndex] = d;
		}
		// split at the dimension just found
		std::nth_element(l, mid, r, [=](const Point &lhs, const Point &rhs) { return coordinate(lhs, split[midIndex]) < coordinate(rhs, split[midIndex]); });
		build(l, mid);
		build(mid + 1, r);
	}

	double ans;
	Point *which; // ans & which are only for function nearest, to reduce the number of parameter

	void nearest(const Point &pt, Point *l, Point *r)
	{
		if (l == r)
			return;
		auto mid = l + ((r - l) >> 1);
		auto midIndex = mid - first;
		// the euclidean distance between pt and current root(pts[mid])
		double dis2 = 0.0;
		for (int d = 0; d < K; ++d)
			dis2 += pow(coordinate(*mid, d) - coordinate(pt, d), 2);
		if (fabs(dis2) > eps && dis2 < ans)
			ans = dis2, which = mid;
		// the euclidean distance between pt and split plane
		double currOfPt = coordinate(pt, split[midIndex]),
			   currOfMid = coordinate(*mid, split[midIndex]);
		double r2 = pow(currOfMid - currOfPt, 2);
		if (currOfPt < currOfMid)
		{
			nearest(pt, l, mid);
			// draw a sphere with pt as center, current ans as radius
			// if it intersect with the split plane, the nearest point maybe in another subtree
			if (r2 <= ans)
				nearest(pt, mid + 1, r);
		}
		else
		{
			nearest(pt, mid + 1, r);
			if (r2 <= ans)
				nearest(pt, l, mid);
		}
	}

	bool found; // only for function `any`

	template <int... Index>
	__attribute__((always_inline)) bool unrollAnyGreater(const Point &mid, const Point &pt, std::integer_sequence<int, Index...>)
	{
		return (... && (coordinate(mid, Index) >= coordinate(pt, Index)));
	}

	void anyGreater(const Point &pt, Point *l, Point *r)
	{
		if (l == r)
			return;
		auto mid = l + ((r - l) >> 1);
		auto midIndex = mid - first;
		if (unrollAnyGreater(*mid, pt, std::make_integer_sequence<int, K>()))
			return void(found = true);
		anyGreater(pt, mid + 1, r);
		if (!found && coordinate(pt, split[midIndex]) <= coordinate(*mid, split[midIndex]))
			anyGreater(pt, l, mid);
	}

	template <typename Func, int... Index>
	__attribute__((always_inline)) void unrollForEach(Func f, Point *it, std::integer_sequence<int, Index...>) const
	{
		(f(it[Index]), ...);
	}

  public:
	KDTree(Point *first, Point *last, int *split, Coordinate coordinate = Coordinate())
		: first(first), last(last), coordinate(coordinate), split(split)
	{
		build(first, last);
		for (int d = 0; d < K; ++d)
		{
			auto [minPoint, maxPoint] = std::minmax_element(first, last, [=](const Point &lhs, const Point &rhs) { return coordinate(lhs, d) < coordinate(rhs, d); });
			min[d] = coordinate(const_cast<const Point &>(*minPoint), d);
			max[d] = coordinate(const_cast<const Point &>(*maxPoint), d);
		}
	}

	struct Result
	{
		const Point &point;
		double dis2;
	};

	Result nearest(const Point &point)
	{
		ans = std::numeric_limits<double>::max(), which = nullptr;
		nearest(point, first, last);
		return {*which, ans};
	}

	bool allGreater(const Point &point)
	{
		bool allEq = true;
		for (int i = 0; i < K; ++i)
		{
			double diff = min[i] - coordinate(point, i);
			if (diff < 0)
				return false;
			else if (diff > 0)
				allEq = false;
		}
		return !allEq;
	}

	bool anyGreater(const Point &point)
	{
		found = false;
		anyGreater(point, first, last);
		return found;
	}

	bool anyLess(const Point &point)
	{
		found = false;
		anyLess(point, first, last);
		return found;
	}

	template <typename Func>
	void forEach(Func f) const
	{
		constexpr int UnrollStep = 16;
		auto it = first;
		for (; it + UnrollStep < last; it += UnrollStep)
			unrollForEach(f, it, std::make_integer_sequence<int, UnrollStep>());
		for (; it < last; ++it)
			f(*it);
	}
};

#endif //MST_KDTREE_H
