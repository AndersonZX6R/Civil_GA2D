#ifndef __CIVIL_GA2D
#define __CIVIL_GA2D

#include <math.h>
#include <regex>

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

		friend Matrix2D operator*(const Matrix2D &mat1, const Matrix2D &mat2)
		{
			Matrix2D
				res;

			for (register int i = 0; i < 3; i++)
				for (register int j = 0; j < 3; j++)
				{
					double
						dblItem = 0;

					for (register int k = 0; k < 3; k++)
						dblItem += mat1.items[i][k] * mat2.items[k][j];

					res.items[i][j] = dblItem;
				}

			return res;
		}
		Matrix2D &operator*=(const Matrix2D &mat)
		{
			*this = *this * mat;

			return *this;
		}

#ifdef MATRIX_DYNAMIC
		operator Matrix<double, 3, 3>()
		{
			Matrix<double, 3, 3>
				mat(3, 3);
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

	struct Point2D
	{
	public:

		typedef Range<short int, 1, 4> Quadrant;

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
		static double dist(double x1, double y1, double x2, double y2)
		{
			double
				_x = x2 - x1,
				_y = y2 - y1;

			return (double)sqrt(_x * _x + _y * _y);
		}

		Point2D transform(const Matrix2D &mat)
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

		void moveTo(const Point2D &pnt)
		{
			Point2D
				p = pnt - pnt1;

			*this = transform(Matrix2D::translation(p.x, p.y));
		}
		void moveTo(double x, double y)
		{
			Point2D
				p(x - pnt1.x, y - pnt1.y);

			*this = transform(Matrix2D::translation(p.x, p.y));
		}

		friend Vector2D operator+(const Vector2D &vtr1, const Vector2D &vtr2)
		{
			Vector2D
				v = vtr2;

			v.moveTo(vtr1.pnt2);

			return Vector2D(vtr1.pnt1, v.pnt2);
		}

		friend Vector2D operator-(const Vector2D &vtr1, const Vector2D &vtr2)
		{
			return Vector2D(vtr2.pnt2, vtr1.pnt2);
		}

	}; /* Vector2D */

} // namespace CIVIL::MATH::GA2D

#endif // ifndef __CIVIL_GA2D