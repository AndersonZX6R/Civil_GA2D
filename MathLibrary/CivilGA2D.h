/***
 * CivilGA2D.h;
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Eng.º Anderson Marques Ribeiro (anderson.marques.ribeiro@gmail.com).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#ifndef __CIVIL_GA2D
#define __CIVIL_GA2D

#include <math.h>
#include <regex>
#include <limits.h>

#include "..\UtilsLibrary\CivilRange.h"
#include "..\UtilsLibrary\CivilError.h"
#include "..\MathLibrary\CivilAngle.h"

#undef MATRIX_DYNAMIC
#include "..\MathLibrary\CivilMatrix.h"

using namespace std;
using namespace CIVIL::UTILS;

namespace CIVIL::MATH::GA2D
{

	struct Point2D;

	DECLARE_ERROR_CODE(meInvalidMirrorArgs);

	BEGIN_DECLARE_ERROR(EMatrix2D)
		DECLARE_ERROR(meInvalidMirrorArgs, "Invalid argument for mirror matrix")
	END_DECLARE_ERROR;

	struct Matrix2D
	{
	public:

		Matrix2D() = default;

		double
			items[3][3];

	private:

		Matrix2D fromMatrix(const Matrix<double, 3, 3> &mat);

	public:

		bool invertible()
		{
			return ((Matrix<double, 3, 3>) *this).invertible();
		}
		Matrix2D reverse()
		{
			return fromMatrix( ((Matrix<double, 3, 3>) *this).reverse() );
		}

		static Matrix2D translation(double x, double y)
		{
			Matrix2D
				mat = M_IDENTITY;

			mat.items[0][2] = x;
			mat.items[1][2] = y;

			return mat;
		}
		static Matrix2D rotation(const Angle &ang)
		{
			double
				dblSin = sin((double)ang),
				dblCos = cos((double)ang);
			Matrix2D
				mat = M_IDENTITY;

			mat.items[0][0] = dblCos;
			mat.items[0][1] = -dblSin;
			mat.items[1][0] = dblSin;
			mat.items[1][1] = dblCos;

			return mat;
		}
		static Matrix2D rotation(const Angle &ang, double xRef, double yRef)
		{
			return translation(xRef, yRef) * rotation(ang) * translation(-xRef, -yRef);
		}
		static Matrix2D scale(double xFactor, double yFactor)
		{
			Matrix2D
				mat = M_IDENTITY;

			mat.items[1][1] = xFactor;
			mat.items[2][2] = yFactor;

			return mat;
		}
		static Matrix2D scale(double xFactor, double yFactor, double xRef, double yRef)
		{
			return translation(xRef, yRef) * scale(xFactor, yFactor) * translation(-xRef, -yRef);
		}
		inline static Matrix2D mirror(double x1, double y1, double x2, double y2);

		friend Matrix2D operator*(const Matrix2D &mat1, const Matrix2D &mat2);
		Matrix2D &operator*=(const Matrix2D &mat)
		{
			*this = *this * mat;

			return *this;
		}

#ifndef MATRIX_DYNAMIC
		operator Matrix<double, 3, 3>()
		{
			Matrix<double, 3, 3>
				mat(3, 3);

			for (int i = 0; i < 3;)
				for (int j = 0; j < 3; j++)
					mat.setItem(i, j, items[i][j]);

			return mat;
		}
#else
		operator Matrix<double>()
		{
			Matrix<double>
				mat(3, 3);

			for (register int i = 0; i < 3; i++)
				for (register int j = 0; j < 3; j++)
					mat.setItem(i, j, items[i][j]);

			return mat;
		}
#endif // ifdef MATRIX_DYNAMIC

		static const Matrix2D
			M_IDENTITY,
			M_NULL;

	}; /* Matrix2D */

	typedef Range<short int, 1, 4> Quadrant;

	struct Point2D
	{
	public:

		Point2D(double _x = 0, double _y = 0) :
			x(_x), y(_y)
		{}

		double
			x, y;

		Quadrant quadrant() const
		{
			if (y > 0)
			{
				if (x <= 0)
					return 2;
				else
					return 1;
			}
			else
			{
				if (x <= 0)
					return 3;
				else
					return 4;
			};
		}

		Point2D versor() const
		{
			return *this / dist();
		}

		double dist() const
		{
			return dist(x, y, 0, 0);
		}
		double dist(const Point2D &pnt) const
		{
			return dist(x, y, pnt.x, pnt.y);
		}
		static double dist(double x1, double y1, double x2, double y2)
		{
			double
				_x = x2 - x1,
				_y = y2 - y1;

			return (double)sqrt(_x * _x + _y * _y);
		}

		Point2D transform(const Matrix2D &mat) const
		{
			Matrix2D
				mPnt = Matrix2D::M_IDENTITY;

			mPnt.items[0][2] = x;
			mPnt.items[1][2] = y;

			mPnt = mat * mPnt;

			return *(new Point2D(mPnt.items[0][2], mPnt.items[1][2]));
		}

		Angle angleAxeX() const
		{
			return angleAxeX(x, y);
		}
		static Angle angleAxeX(double x, double y)
		{
			double
				dblDist = dist(x, y, 0, 0);
			Angle
				ang;

			if (dblDist != 0)
			{
				ang = x / dblDist;

				ang = acos((double)ang);

				if (y < 0)
					ang += Angle::ANGLE_180;
			}

			return ang;
		}
		static Angle angleBetween(double x1, double y1, double x2, double y2, double xRef, double yRef)
		{
			return angleAxeX(x1 - xRef, y1 - yRef) - angleAxeX(x2 - xRef, y2 - yRef);
		}

		double vectorProduct(const Point2D &pnt) const
		{
			return x * pnt.y - pnt.x * y;
		}

		friend Point2D operator+(const Point2D &pnt1, const Point2D &pnt2)
		{
			return *(new Point2D(pnt1.x + pnt2.x, pnt1.y + pnt2.y));
		}
		friend Point2D operator+(const Point2D &pnt, double value)
		{
			return *(new Point2D(pnt.x + value, pnt.y + value));
		}
		friend Point2D operator+(double value, const Point2D &pnt)
		{
			return operator+(value, pnt);
		}

		Point2D &operator+=(const Point2D &pnt)
		{
			*this = *this + pnt;

			return *this;
		}
		Point2D &operator+=(double value)
		{
			*this = *this + value;

			return *this;
		}

		friend Point2D operator-(const Point2D &pnt1, const Point2D &pnt2)
		{
			return *(new Point2D(pnt1.x - pnt2.x, pnt1.y - pnt2.y));
		}
		friend Point2D operator-(const Point2D &pnt, double value)
		{
			return *(new Point2D(pnt.x - value, pnt.y - value));
		}
		friend Point2D operator-(double value, const Point2D &pnt)
		{
			return operator-(value, pnt);
		}

		Point2D &operator-=(const Point2D &pnt)
		{
			*this = *this - pnt;

			return *this;
		}
		Point2D &operator-=(double value)
		{
			*this = *this - value;

			return *this;
		}

		friend Point2D operator*(const Point2D &pnt1, const Point2D &pnt2)
		{
			return *(new Point2D(pnt1.x * pnt2.x, pnt1.y * pnt2.y));
		}
		friend Point2D operator*(const Point2D &pnt, double value)
		{
			return *(new Point2D(pnt.x * value, pnt.y * value));
		}
		friend Point2D operator*(double value, const Point2D &pnt)
		{
			return operator*(pnt, value);
		}

		Point2D &operator*=(const Point2D &pnt)
		{
			*this = *this * pnt;

			return *this;
		}
		Point2D &operator*=(double value)
		{
			*this = *this * value;

			return *this;
		}

		friend Point2D operator/(const Point2D &pnt1, const Point2D &pnt2)
		{
			return *(new Point2D(pnt1.x / pnt2.x, pnt1.y / pnt2.y));
		}
		friend Point2D operator/(const Point2D &pnt, double value)
		{
			return *(new Point2D(pnt.x / value, pnt.y / value));
		}
		friend Point2D operator/(double value, const Point2D &pnt)
		{
			return operator/(pnt, value);
		}

		Point2D &operator/=(const Point2D &pnt)
		{
			*this = *this / pnt;

			return *this;
		}
		Point2D &operator/=(double value)
		{
			*this = *this / value;

			return *this;
		}

	}; /* Point2D */

	const Point2D NULL_POINT = { 0, 0 };
	const Point2D INVALID_POINT = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };

	enum SideEnum
	{
		sLeft = 1,
		sOver = 0,
		sRight = -1
	};

	template <typename NumType> inline constexpr
	int sign(NumType num)
	{
		return (NumType(0) < num) - (num < NumType(0));
	}

	struct Vector2D
	{
	public:

		Vector2D(double _x1 = 0, double _y1 = 0, double _x2 = 0, double _y2 = 0) :
			x1(_x1),
			y1(_y1),
			x2(_x2),
			y2(_y2)
		{}
		Vector2D(const Point2D &_pnt1, const Point2D &_pnt2) :
			pnt1(_pnt1),
			pnt2(_pnt2)
		{}

		union
		{
			struct { Point2D pnt1, pnt2;  };
			struct { double x1, y1, x2, y2; };
		};

		Point2D versor() const
		{
			return (pnt2 - pnt1) / getModule();
		}

		double getModule() const
		{
			return (pnt2 - pnt1).dist();
		}
		void setModule(double value)
		{
			pnt2 = pnt1 + versor() * value;
		}

		Vector2D transform(const Matrix2D &mat) const
		{
			Vector2D
				vtr = *this;

			vtr.pnt1 = vtr.pnt1.transform(mat);
			vtr.pnt2 = vtr.pnt2.transform(mat);

			return vtr;
		}

		double distPoint(const Point2D &pnt) const;
		Vector2D perpendicular(const Point2D &ref, double module = 0) const;

		Vector2D reverse() const
		{
			return Vector2D(x2, y2, x1, y1);
		}

		Point2D midPoint() const
		{
			return (pnt2 + pnt1) / 2;
		}

		SideEnum side(const Point2D &pnt) const
		{
			return (SideEnum) sign((pnt2 - pnt1).vectorProduct(pnt - pnt1));
		}

		Vector2D moveTo(const Point2D &pnt)
		{
			Point2D
				p = pnt - pnt1;

			return transform(Matrix2D::translation(p.x, p.y));
		}
		Vector2D moveTo(double x, double y)
		{
			Point2D
				p(x - pnt1.x, y - pnt1.y);

			return transform(Matrix2D::translation(p.x, p.y));
		}

		// innerLimits;
		//
		// Checks whether the perpendicular projection of a point is delimited by the extreme points of the vector.
		// ----
		bool innerLimits(const Point2D &pnt) const
		{
			Matrix2D
				mat = Matrix2D::translation(-x1, -y1) * Matrix2D::rotation(-(pnt2 - pnt1).angleAxeX());
			Vector2D
				vtr = transform(mat);
			Point2D
				p = pnt.transform(mat);

			return (p.x >= vtr.x1) && (p.x <= vtr.x2);
		}

		// checkParallel;
		//
		// Checks whether two vectors are parallel.
		// ----
		bool checkParallel(const Vector2D &vtr) const
		{
			return (vtr.pnt2 - vtr.pnt1).vectorProduct(pnt2 - pnt1) == 0;
		}

		bool intersection(const Vector2D &vtr, bool aparent, Point2D &pnt) const;

		bool intercept(const Vector2D &vtr) const
		{
			return side(vtr.pnt1) != side(vtr.pnt2) && vtr.side(pnt1) != vtr.side(pnt2);
		}

		friend Vector2D operator+(const Vector2D &vtr1, const Vector2D &vtr2)
		{
			Vector2D
				v = vtr2;

			v.moveTo(vtr1.pnt2);

			return Vector2D(vtr1.pnt1, v.pnt2);
		}
		Vector2D &operator+=(const Vector2D &vtr)
		{
			*this = *this + vtr;

			return *this;
		}

		friend Vector2D operator-(const Vector2D &vtr1, const Vector2D &vtr2)
		{
			return Vector2D(vtr2.pnt2, vtr1.pnt2);
		}
		Vector2D &operator-=(const Vector2D &vtr)
		{
			*this = *this - vtr;

			return *this;
		}

	}; /* Vector2D */

	struct Rectangle2D
	{
	public:

		Rectangle2D() = default;
		Rectangle2D(double _left, double _bottom, double _right, double _top) :
			left(_left),
			bottom(_bottom),
			right(_right),
			top(_top)
		{}

		union
		{
			struct { double left, bottom, right, top;  };
			struct { Point2D bottomLeft, topRight; };
		};

		Point2D getTopLeft() const
		{
			return Point2D(left, top);
		}

		Point2D getBottomRight() const
		{
			return Point2D(right, bottom);
		}

		double getWidth() const
		{
			return abs(right - left);
		}
		void setWidth(double value)
		{
			right = left + value;
		}
		double getHeight() const
		{
			return abs(top - bottom);
		}
		void setHeight(double value)
		{
			top = bottom + value;
		}

		double area() const
		{
			return getWidth() * getHeight();
		}
		double perimeter() const
		{
			return 2 * (getWidth() + getHeight());
		}
		Point2D center() const
		{
			return (bottomLeft + topRight) / 2;
		}

		Rectangle2D offset(double x, double y) const
		{
			return Rectangle2D(left + x, bottom + y, right + x, top + y);
		}
		Rectangle2D inflate(double x, double y) const
		{
			Point2D
				pnt = center();
			double
				dblX = getWidth() / 2,
				dblY = getHeight() / 2;

			return Rectangle2D(pnt.x - dblX - x, pnt.y - dblY - y, pnt.x + dblX + x, pnt.y + dblY + y);
		}

		static Rectangle2D combine(const Rectangle2D &rect1, const Rectangle2D &rect2)
		{
			return Rectangle2D(min(rect1.left, rect2.left), min(rect1.bottom, rect2.bottom), max(rect1.right, rect2.right), max(rect1.top, rect2.top));
		}

	}; /* Rectangle2D */

	DECLARE_ERROR_CODE(ceNullRadius);
	DECLARE_ERROR_CODE(ceLinearPoints);

	BEGIN_DECLARE_ERROR(ECircle2D)
		DECLARE_ERROR(ceNullRadius, "Null radius")
		DECLARE_ERROR(ceLinearPoints, "Linear points")
	END_DECLARE_ERROR;

	typedef Range<unsigned short int, 0, 3> QuadrantType;

	enum IntersectionEnum
	{
		iNone,
		iTangent,
		iTwoPoints
	};

	struct Circle2D
	{
	public:

		Circle2D() :
			center(0, 0),
			m_dblRadius(1)
		{};
		Circle2D(const Point2D &center, double radius);
		Circle2D(const Point2D &center, const Point2D &secPoint);
		Circle2D(const Point2D &pnt1, const Point2D &pnt2, const Point2D &pnt3);

		Point2D center;

	private:

		double m_dblRadius;

	public:

		double getRadius()
		{
			return m_dblRadius;
		};
		void setRadius(double value);

		Point2D getSecondPoint()
		{
			return quadrantPoint(0);
		}

		// QuadrantPoint;
		//
		// Calculates the coordinates of one of the 4 points that limit the quadrants 
		// ---- of the circumference
		// *************************************************************************
		Point2D quadrantPoint(QuadrantType quad);

		Circle2D transform(const Matrix2D &mat);

		// BoundsRect;
		//
		// Returns the smallest rectangular region containing the circle.
		// ----
		// *************************************************************************
		Rectangle2D boundsRect() const;

		double perimeter() const;
		double area() const;

		double inertiaX(const Point2D &ref = NULL_POINT) const;
		double inertiaY(const Point2D &ref = NULL_POINT) const;

	private:

		bool teste(const Point2D &pnt1, const Point2D &pnt2);

	public:

		bool intercept(const Rectangle2D &rect);
		IntersectionEnum intercept(const Vector2D &vector, Vector2D &res);

		bool contains(const Point2D &pnt);

	}; /* Circle2D */

} // namespace CIVIL::MATH::GA2D

#endif // ifndef __CIVIL_GA2D