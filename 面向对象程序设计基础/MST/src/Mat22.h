//
// Created by MashPlant on 18-5-6.
//

#ifndef MST_MAT22_H
#define MST_MAT22_H

#include "Point2d.h"

struct Mat22
{
	double x11, x12, x21, x22;

	Mat22 &operator*=(double rhs)
	{
		x11 *= rhs, x12 *= rhs, x21 *= rhs, x22 *= rhs;
		return *this;
	}

	friend Mat22 operator*(Mat22 lhs, double rhs)
	{ return lhs *= rhs; }

	friend Mat22 operator*(double lhs, Mat22 rhs)
	{ return rhs *= lhs; }

	Mat22 &operator/=(double rhs)
	{
		// divide is normally slower than multiply
		// and compiler is normally not willing to optimize it
		// due to the precision loss may be different
		rhs = 1 / rhs;
		x11 *= rhs, x12 *= rhs, x21 *= rhs, x22 *= rhs;
		return *this;
	}

	friend Mat22 operator/(Mat22 lhs, double rhs)
	{ return lhs /= rhs; }

	friend Mat22 operator/(double lhs, Mat22 rhs)
	{ return rhs /= lhs; }


	Mat22 operator*(const Mat22 &rhs) const
	{
		Mat22 ret;
		ret.x11 = x11 * rhs.x11 + x12 * rhs.x21;
		ret.x12 = x11 * rhs.x12 + x12 * rhs.x22;
		ret.x21 = x21 * rhs.x11 + x22 * rhs.x21;
		ret.x22 = x21 * rhs.x12 + x22 * rhs.x22;
		return ret;
	}

	Mat22 &operator*=(const Mat22 &rhs)
	{ return *this = *this * rhs; }

	Mat22 &operator+=(const Mat22 &rhs)
	{
		x11 += rhs.x11, x12 += rhs.x12, x21 += rhs.x21, x22 += rhs.x22;
		return *this;
	}

	friend Mat22 operator+(Mat22 lhs, const Mat22 &rhs)
	{ return lhs += rhs; }


	Mat22 &operator-=(const Mat22 &rhs)
	{
		x11 -= rhs.x11, x12 -= rhs.x12, x21 -= rhs.x21, x22 -= rhs.x22;
		return *this;
	}

	friend Mat22 operator-(Mat22 lhs, const Mat22 &rhs)
	{ return lhs -= rhs; }

	Mat22 inv() const
	{
		Mat22 ret{x22, -x12, -x21, x11};
		return ret / (x11 * x22 - x12 * x21);
	}
};

inline Point2d operator*(const Mat22 &lhs, const Point2d &rhs)
{
	Point2d ret;
	ret.x = rhs.x * lhs.x11 + rhs.y * lhs.x12;
	ret.y = rhs.x * lhs.x21 + rhs.y * lhs.x22;
	return ret;
}

#endif //MST_MAT22_H
