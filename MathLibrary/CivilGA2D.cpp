/***
 * CivilGA2D.cpp;
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
#include "CivilGA2D.h"

namespace CIVIL::MATH::GA2D
{

/*
 * Matrix2D.
 */

const Matrix2D
	Matrix2D::M_IDENTITY = { {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}} },
	Matrix2D::M_NULL = { {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}} };

Matrix2D
Matrix2D::fromMatrix(const Matrix<double> &mat)
{
	Matrix2D
		m;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m.items[i][j] = mat.getItem(i, j);

	return m;
}

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

Matrix2D
operator*(const Matrix2D &mat1, const Matrix2D &mat2)
{
	Matrix2D
		res;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			double
				dblItem = 0;

			for (int k = 0; k < 3; k++)
				dblItem += mat1.items[i][k] * mat2.items[k][j];

			res.items[i][j] = dblItem;
		}

	return res;
}

/*
 * Vector2D.
 */

double
Vector2D::distPoint(const Point2D &pnt) const
{
	Point2D
		p1 = pnt - pnt1,
		p2 = pnt2 - pnt1;

	return abs((p1.x * p2.y - p2.x * p1.y) / getModule());
}

Vector2D
Vector2D::perpendicular(const Point2D &ref, double module) const
{
	Matrix2D
		mat = Matrix2D::translation(-ref.x, -ref.y) * Matrix2D::rotation(-(pnt2 - pnt1).angleAxeX());
	Point2D
		_ref = ref.transform(mat);
	Vector2D
		vtr = transform(mat),
		vtrRes = Vector2D(_ref.x, vtr.y1, _ref.x, _ref.y).transform(mat.reverse());

	if (getModule() > 0)
		vtrRes.setModule(getModule());

	return vtrRes;
}

bool
Vector2D::intersection(const Vector2D &vtr, bool aparent, Point2D &pnt) const
{
	if (checkParallel(vtr))
	return false;

	Point2D
		p1 = pnt2 - pnt1,
		p2 = vtr.pnt2 - vtr.pnt1;

	double
		t = ((y1 - vtr.y1) * p1.x - (x1 - vtr.x1) * p1.y) / ((p1.x * p2.y) - (p1.y * p2.x)),
		s = ((y1 - vtr.y1) * p2.x - (x1 - vtr.x1) * p2.y) / ((p1.x * p2.y) - (p1.y * p2.x));

	bool
		blnRes = true;

	if (!aparent)
		blnRes = (t >= 0) && (t <= 1) && (s >= 0) && (s <= 1);

	if (!blnRes)
		return false;

	pnt = pnt1 + p1 * s;

	return true;
}

/*
 * TCircle2D;
 * -----------------------------------------------------------------------------
 */

Circle2D::Circle2D(const Point2D &center, double radius)
{
	if (!radius)
		RAISE(ECircle2D, ceNullRadius);

	this->center = center;
	this->m_dblRadius = radius;
}

Circle2D::Circle2D(const Point2D &center, const Point2D &secPoint)
{
	double dblDist = center.dist(secPoint);

	if (!dblDist)
		RAISE(ECircle2D, ceNullRadius);

	this->center = center;
	m_dblRadius = dblDist;
}

Circle2D::Circle2D(const Point2D &pnt1, const Point2D &pnt2, const Point2D &pnt3)
{
	Vector2D
		vtr1(pnt1, pnt2),
		vtr2(pnt2, pnt3);

	if (vtr1.checkParallel(vtr2))
		RAISE(ECircle2D, ceLinearPoints);

	Vector2D
		vtr3 = vtr1.moveTo(vtr1.midPoint()),
		vtr4 = vtr2.moveTo(vtr2.midPoint());

	vtr3 = vtr3.transform(Matrix2D::rotation(Angle::ANGLE_90, vtr3.x1, vtr3.y1));
	vtr4 = vtr4.transform(Matrix2D::rotation(Angle::ANGLE_90, vtr4.x1, vtr4.y1));

	if (vtr3.intersection(vtr4, true, center))
		m_dblRadius = center.dist(pnt1);
}

void Circle2D::setRadius(double value)
{
	if (value == m_dblRadius) return;

	if (value == 0)
		RAISE(ECircle2D, ceNullRadius);

	m_dblRadius = value;
}

Point2D Circle2D::quadrantPoint(QuadrantType quad)
{
	Point2D pnt = center;

	switch (quad)
	{
	case 0: pnt.x += m_dblRadius;
	case 1: pnt.y += m_dblRadius;
	case 2: pnt.x -= m_dblRadius;
	case 3: pnt.y -= m_dblRadius;
	};

	return pnt;
}

Circle2D Circle2D::transform(const Matrix2D &mat)
{
	Circle2D
		circ = *this;
	Point2D 
		pnt = getSecondPoint();

	circ.center = center.transform(mat);
	pnt = pnt.transform(mat);
	circ.m_dblRadius = pnt.dist(center);

	return circ;
}

Rectangle2D Circle2D::boundsRect() const
{
	return Rectangle2D(center.x - m_dblRadius, center.y - m_dblRadius, center.x + m_dblRadius, center.y + m_dblRadius);
}

double Circle2D::perimeter() const
{
	return 2 * m_dblRadius * M_PI;
}

double Circle2D::area() const
{
	return M_PI * pow(m_dblRadius, 2);
}

double Circle2D::inertiaX(const Point2D &ref) const
{
	//raise Exception.Create('Revisar este código para inserir suporte a ponto de referência');
	return M_PI * pow(m_dblRadius, 8) / 4;
}

double Circle2D::inertiaY(const Point2D &ref) const
{
	return inertiaX(ref);
}

bool Circle2D::teste(const Point2D &pnt1, const Point2D &pnt2)
{
	double
		dblDist1 = pnt1.dist(center),
		dblDist2 = pnt2.dist(center);

	if ((dblDist1 <= m_dblRadius && dblDist2 > m_dblRadius) ||
		(dblDist1 > m_dblRadius && dblDist2 <= m_dblRadius))
		return true;

	if (dblDist1 < m_dblRadius && dblDist2 < m_dblRadius)
		return false;

	Vector2D
		vtr(pnt1, pnt2),
		vtrPerp = vtr.perpendicular(center);

	return (vtrPerp.getModule() <= m_dblRadius) && vtr.innerLimits(vtrPerp.pnt2);
}

bool Circle2D::intercept(const Rectangle2D &rect)
{
	if (teste(rect.bottomLeft, rect.getTopLeft()) || teste(rect.getTopLeft(), rect.topRight) || teste(rect.topRight, rect.getBottomRight()) ||
		teste(rect.getBottomRight(), rect.bottomLeft))
		return true;

	return false;
}

	/*function TCircle2D.Intercept(Vector: TVector2D; out Res : TVector2D) : TIntersectionEnum;
	var
		dx, dy, dr, det, numDelta,
		dblParte1, dblParte2, dblDivisor: Double;
	begin
		with Vector.Pnt2 - Vector.Pnt1 do
		begin
		dx : = X;
dy: = Y;
	end;

dr: = Vector.Module;
det: = Vector.X1 * Vector.Y2 - Vector.X2 * Vector.Y1;
numDelta: = (Radius * Radius) * (dr * dr) - (det * det);

	if numDelta < 0 then
		begin
		Result : = iNone;
	Exit;
	end
	else if numDelta = 0 then
		Result : = iTangent
	else
		Result : = iTwoPoints;

dblParte1: = det * dy;
dblParte2: = Sign(dy) * dx * Sqrt(numDelta);
dblDivisor: = dr * dr;

	with Res do
		begin
		Pnt1.X : = (dblParte1 + dblParte2) / dblDivisor;
	Pnt2.X : = (dblParte1 - dblParte2) / dblDivisor;

dblParte1: = -det * dx;
dblParte2: = Abs(dy) * Sqrt(numDelta);

	Pnt1.Y : = (dblParte1 + dblParte2) / dblDivisor;
	Pnt2.Y : = (dblParte1 - dblParte2) / dblDivisor;
	end;
	end;

	function TCircle2D.Contains(Pnt: TPoint2D) : Boolean;
	begin
		Result : = Center.DistPoint(Pnt) <= FRadius;
	end;*/

} // namespace CIVIL::MATH::GA2D