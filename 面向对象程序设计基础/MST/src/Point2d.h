//
// Created by MashPlant on 18-5-5.
//

#ifndef MST_POINT2D_H
#define MST_POINT2D_H

#include <cmath>

struct Point2d
{
	double x, y;

	double &operator[](int pos)
	{ return (&x)[pos]; }

	double operator[](int pos) const
	{ return (&x)[pos]; }

	Point2d &operator+=(const Point2d &rhs)
	{ return x += rhs.x, y += rhs.y, *this; }

	friend Point2d operator+(Point2d lhs, const Point2d &rhs)
	{ return lhs += rhs; }

	Point2d &operator-=(const Point2d &rhs)
	{ return x -= rhs.x, y -= rhs.y, *this; }

	friend Point2d operator-(Point2d lhs, const Point2d &rhs)
	{ return lhs -= rhs; }

	Point2d operator-() const
	{ return {-x, -y}; }

	Point2d &operator*=(double rhs)
	{ return x *= rhs, y *= rhs, *this; }

	friend Point2d operator*(Point2d lhs, double rhs)
	{ return lhs *= rhs; }

	friend Point2d operator*(double lhs, Point2d rhs)
	{ return rhs *= lhs; }

	Point2d &operator/=(double rhs)
	{ return x /= rhs, y /= rhs, *this; }

	friend Point2d operator/(Point2d lhs, double rhs)
	{ return lhs /= rhs; }

	static double dis2(const Point2d &lhs, const Point2d &rhs)
	{ return (rhs.x - lhs.x) * (rhs.x - lhs.x) + (rhs.y - lhs.y) * (rhs.y - lhs.y); }

	double length2() const
	{ return x * x + y * y; }

	double length() const
	{ return std::sqrt(x * x + y * y); }

	static double dot(const Point2d &lhs, const Point2d &rhs)
	{ return lhs.x * rhs.x + lhs.y * rhs.y; }

	static double cross(const Point2d &lhs, const Point2d &rhs)
	{ return lhs.x * rhs.y - lhs.y * rhs.x; }
};

template<typename OS>
OS &&operator<<(OS &&os, const Point2d &point)
{ return os << '(' << point.x << ',' << point.y << ')'; }

#endif //MST_POINT2D_H
