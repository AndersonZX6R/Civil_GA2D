#include "CivilGA2D.h"

namespace CIVIL::MATH::GA2D
{

/*
 * Matrix2D.
 * -----------------------------------------------------------------------------------------------------------------------------------------------------
 */

const Matrix2D
	Matrix2D::M_IDENTITY = { {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}} },
	Matrix2D::M_NULL = { {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}} };

Matrix2D
Matrix2D::mirror(double x1, double y1, double x2, double y2)
{
	if (Point2D::dist(x1, y1, x2, y2) == 0)
		RAISE(EMatrix2D, meInvalidMirrorArgs);

	Matrix2D
		mat = M_IDENTITY,
		_mat = M_IDENTITY;

	_mat.items[1][1] = -1;

	Point2D
		pnt(x2 - x1, y2 - y1);

	if (pnt.x == 0)
	{
		Angle
			ang = Angle::ANGLE_90;

		mat = translation(x1, 0);
		mat *= rotation(ang);
		mat *= _mat;
		mat *= rotation(-ang);
		mat *= translation(-x1, 0);
	}
	else
	{
		double
			dblInt = y1 - ((y2 - y1) / (x2 - x1)) * x1;

		Angle
			ang = pnt.angleAxeX();

		mat = translation(0, dblInt);
		mat *= rotation(ang);
		mat *= _mat;
		mat *= rotation(-ang);
		mat *= translation(0, -dblInt);
	}

	return mat;
}

} // namespace CIVIL::MATH::GA2D