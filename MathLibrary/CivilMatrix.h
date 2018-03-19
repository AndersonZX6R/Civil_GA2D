#ifndef __CIVIL_MATRIX
#define __CIVIL_MATRIX 

#ifdef _MANAGED
#pragma unmanaged
#endif // ifdef _MANAGED

#include "..\UtilsLibrary\CivilError.h"
#include "..\UtilsLibrary\CivilRange.h"

using namespace CIVIL::UTILS;

namespace CIVIL::MATH::GA2D
{

#define MAX_ROWS 20
#define MAX_COLS 20

	DECLARE_ERROR_CODE(meOverflow);
	DECLARE_ERROR_CODE(meInvalidIndex);
	DECLARE_ERROR_CODE(meCantRemoveDim);
	DECLARE_ERROR_CODE(meInvertible);
	DECLARE_ERROR_CODE(meNotSquare);
	DECLARE_ERROR_CODE(meIncompatible);

	BEGIN_DECLARE_ERROR(EMatrix)
		DECLARE_ERROR(meOverflow, "The matrix does not contain the specified size")
		DECLARE_ERROR(meInvalidIndex, "Invalid index")
		DECLARE_ERROR(meCantRemoveDim, "Row/colunm can not be removed")
		DECLARE_ERROR(meInvertible, "impossible to calculate the inverse matrix")
		DECLARE_ERROR(meNotSquare, "Invalid operation for non-square matrix")
		DECLARE_ERROR(meIncompatible, "Matrices incompatible for operation")
	END_DECLARE_ERROR;

#ifdef MATRIX_DYNAMIC
	template<typename _type = double>
#else
	template<typename _type = double, int _rows = MAX_ROWS, int _cols = MAX_COLS>
#endif // ifdef MATRIX_DYNAMIC
	struct Matrix
	{
	public:

#ifndef MATRIX_DYNAMIC
		typedef Range<short int, 0, _rows - 1> RowRangeType;
		typedef Range<short int, 0, _cols - 1> ColRangeType;
		typedef Range<short int, 1, _rows> RowSizeType;
		typedef Range<short int, 1, _cols> ColSizeType;
#else
		typedef Range<short int, 0, SHRT_MAX - 1> RowRangeType, ColRangeType;
		typedef Range<short int, 1, SHRT_MAX> RowSizeType, ColSizeType;
#endif // ifndef MATRIX_DYNAMIC

		Matrix()
#ifdef MATRIX_DYNAMIC
			: m_aItems(0, 0)
#endif // ifdef MATRIX_DYNAMIC
		{}
		Matrix(const RowSizeType &rows, const ColSizeType &cols)
#ifndef MATRIX_DYNAMIC
			: m_intRowsCount(rows), m_intColsCount(cols)
#else
			: m_aItems( DynArray<_type>(rows, cols) )
#endif
		{}
		Matrix(const Matrix &mat)
#ifdef MATRIX_DYNAMIC
			: m_aItems(mat.m_aItems)
#endif // ifdef MATRIX_DYNAMIC
		{
#ifndef MATRIX_DYNAMIC
			*this = mat;
#endif // ifndef MATRIX_DYNAMIC
		}
#ifndef MATRIX_DYNAMIC
		Matrix(_type *values, const RowSizeType &rows, const ColSizeType &cols) :
			m_intRowsCount(rows), m_intColsCount(cols)
		{
			memcpy(&m_aItems, values, (rows + cols) * sizeof(_type));
		}
#endif // ifndef MATRIX_DYNAMIC

	private:

#ifndef MATRIX_DYNAMIC
		_type
			m_aItems[_rows][_cols];
		int
			m_intRowsCount = 0,
			m_intColsCount = 0;
#else
		DynArray<_type>
			m_aItems;
#endif // MATRIX_DYNAMIC

	public:

#ifndef MATRIX_DYNAMIC
		static const int
			maxRows = _rows,
			maxCols = _cols;
#endif // ifndef MATRIX_DYNAMIC

		RowSizeType getRowCount() const
		{
#ifndef MATRIX_DYNAMIC
			return m_intRowsCount;
#else
			return m_aItems.getRowCount();
#endif // ifndef MATRIX_DYNAMIC
		}
		ColSizeType getColCount() const
		{
#ifndef MATRIX_DYNAMIC
			return m_intColsCount;
#else
			return m_aItems.getColCount();
#endif // ifndef MATRIX_DYNAMIC
		}
		void setDims(const RowSizeType &rows, const ColSizeType &cols)
		{
#ifndef MATRIX_DYNAMIC
			if (rows > _rows || cols > _cols)
				RAISE(EMatrix, meOverflow);

			m_intRowsCount = rows;
			m_intColsCount = cols;
#else
			m_aItems.setDims(rows, cols);
#endif // ifndef MATRIX_DYNAMIC
		}

		_type getItem(const RowRangeType &row, const ColRangeType &col) const
		{
#ifndef MATRIX_DYNAMIC
			if (row >= m_intRowsCount || col >= m_intColsCount)
				RAISE(EMatrix, meInvalidIndex);

			return m_aItems[row][col];
#else
			return m_aItems.getItem(row, col);
#endif // ifndef MATRIX_DYNAMIC
		}
		void setItem(const RowRangeType &row, const ColRangeType &col, _type value)
		{
#ifndef MATRIX_DYNAMIC
			if (row >= m_intRowsCount || col >= m_intColsCount)
				RAISE(EMatrix, meInvalidIndex);

			m_aItems[row][col] = value;
#else
			m_aItems.setItem(row, col, value);
#endif // ifndef MATRIX_DYNAMIC
		}

		void removeRow(const RowRangeType &row)
		{
			if (row >= getRowCount())
				RAISE(EMatrix, meInvalidIndex);

			if (getRowCount() == 1)
				RAISE(EMatrix, meCantRemoveDim);

#ifndef MATRIX_DYNAMIC
			for (register int i = row ; i < m_intRowsCount - 1; i++)
				for (register int j = 0; j < m_intColsCount; j++)
					m_aItems[i][j] = m_aItems[i + 1][j];

			m_intRowsCount--;
#else
			for (register int i = row; i < m_aItems.getRowCount() - 1; i++)
				for (register int j = 0; j < m_aItems.getColCount(); j++)
					m_aItems.setItem(i, j, m_aItems.getItem(i + 1, j));

			m_aItems.setDims(m_aItems.getRowCount() - 1, m_aItems.getColCount());
#endif // ifndef MATRIX_DYNAMIC
		}
		void removeCol(const ColRangeType &col)
		{
			if (col >= getColCount())
				RAISE(EMatrix, meInvalidIndex);

			if (getColCount() == 1)
				RAISE(EMatrix, meCantRemoveDim);

#ifndef MATRIX_DYNAMIC
			for (int i = 0; i < m_intRowsCount; i++)
				for (int j = col; j < m_intColsCount - 1; j++)
					m_aItems[i][j] = m_aItems[i][j + 1];
			m_intColsCount--;
#else
			for (int i = 0; i < m_aItems.getRowCount(); i++)
				for (int j = col; j < m_aItems.getColCount() - 1; j++)
					m_aItems.setItem(i, j, m_aItems.getItem(i, j + 1));
			m_aItems.setDims(m_aItems.getRowCount(), m_aItems.getColCount() - 1);
#endif // ifndef MATRIX_DYNAMIC
		}

		static Matrix null(const RowSizeType &rows, const ColSizeType &cols)
		{
			Matrix
				mat(rows, cols);

			for (register int i = 0; i < rows; i++)
				for (register int j = 0; j < cols; j++)
					mat.setItem(i, j, 0);

			return mat;
		}
		static Matrix identity(const RowSizeType &size)
		{
			Matrix
				mat(size, size);

			for (register int i = 0; i < size; i++)
				for (register int j = 0; j < size; j++)
					mat.setItem(i, j, i == j ? 1 : 0);

			return mat;
		}

		Matrix cofats() const
		{
			register int
				i, j;

			if (getRowCount() != getColCount())
				RAISE(EMatrix, meNotSquare);

			Matrix
				tmp,
				res(getRowCount(), getColCount());

			for (register int i = 0; i < getRowCount(); i++)
				for (register int j = 0; j < getColCount(); j++)
				{
					tmp = *this;
					tmp.removeRow(i);
					tmp.removeCol(j);

					res.setItem(i, j, (_type)pow(-1, (i + 1) + (j + 1)) * tmp.calcDet());
				}

			return res;
		}

		Matrix transposed() const
		{
			Matrix
				res(getColCount(), getRowCount());

			for (register int i = 0; i < getRowCount(); i++)
				for (register int j = 0; j < getColCount(); j++)
					res.setItem(j, i, getItem(i, j));

			return res;
		}

		bool invertible() const
		{
			return getRowCount() == getColCount() && calcDet() != 0;
		}
		Matrix reverse() const
		{
			if (!invertible())
				RAISE(EMatrix, meInvertible);

			return cofats().transposed() * (1 / calcDet());
		}

		Matrix primaryDiagonal() const
		{
			if (getRowCount() != getColCount())
				RAISE(EMatrix, meNotSquare);

			Matrix
				res(1, getRowCount());

			for (register int i = 0; i < getRowCount(); i++)
				res.setItem(0, i, getItem(i, i));

			return res;
		}
		Matrix secundaryDiagonal() const
		{
			if (getRowCount() != getColCount())
				RAISE(EMatrix, meNotSquare);

			Matrix
				res(1, getRowCount());
			register int
				j = getRowCount() - 1;

			for (register int i = 0; i < getRowCount(); i++)
			{
				res.setItem(0, j, getItem(j, i));
				j--;
			}

			return res;
		}

		Matrix &operator=(const Matrix &mat)
		{
#ifndef MATRIX_DYNAMIC
			/*setRowCount(mat.getRowCount());
			setColCount(mat.getColCount());

			for (register int i = 0; i < mat.getRowCount(); i++)
				for (register int j = 0; j < mat.getColCount(); j++)
					setItem(i, j, mat.getItem(i, j));*/
			m_intRowsCount = mat.m_intRowsCount;
			m_intColsCount = mat.m_intColsCount;

			memcpy(&m_aItems, &mat.m_aItems, sizeof(_type) * _rows * _cols);
#else
			m_aItems = mat.m_aItems;
#endif // ifndef MATRIX_DYNAMIC

			return *this;
		}

		friend Matrix operator+(const Matrix &mat1, const Matrix &mat2)
		{
			if (mat1.getRowCount() != mat2.getRowCount() || mat1.getColCount() != mat2.getColCount())
				RAISE(EMatrix, meIncompatible);

			Matrix
				res(mat1.getRowCount(), mat1.getColCount());

			for (register int i = 0; i < getRowCount(); i++)
				for (register int j = 0; j < getColCount(); j++)
					res.setItem(i, j, mat1.getItem(i, j) + mat2.getItem(i, j));

			return res;
		}
		friend Matrix operator+(const Matrix &mat, _type value)
		{
			Matrix
				res(mat.getRowCount(), mat.getColCount());

			for (register int i = 0; i < mat.getRowCount(); i++)
				for (register int j = 0; j < mat.getColCount(); j++)
					res.setItem(i, j, mat.getItem(i, j) + value);

			return res;
		}
		friend Matrix operator+(_type value, const Matrix &mat)
		{
			return operator+(mat, value);
		}
		Matrix &operator+=(const Matrix &mat)
		{
			*this = *this + mat;

			return *this;
		}
		Matrix &operator+=(_type value)
		{
			*this = *this + value;

			return *this;
		}

		friend Matrix operator-(const Matrix &mat1, const Matrix &mat2)
		{
			if (mat1.getRowCount() != mat2.getRowCount() || mat1.getColCount() != mat2.getColCount())
				RAISE(EMatrix, meIncompatible);

			Matrix
				res(mat1.getRowCount(), mat1.getColCount());

			for (register int i = 0; i < res.getRowCount(); i++)
				for (register int j = 0; j < res.getColCount(); j++)
					res.setItem(i, j, mat1.getItem(i, j) - mat2.getItem(i, j));

			return res;
		}
		friend Matrix operator-(const Matrix &mat, _type value)
		{
			Matrix
				res(mat.getRowCount(), mat.getColCount());

			for (register int i = 0; i < res.getRowCount(); i++)
				for (register int j = 0; j < res.getColCount(); j++)
					res.setItem(i, j, mat.getItem(i, j) - value);

			return res;
		}
		friend Matrix operator-(_type value, const Matrix &mat)
		{
			return operator-(mat, value);
		}
		Matrix &operator-=(const Matrix &mat)
		{
			*this = *this - mat;

			return *this;
		}
		Matrix &operator-=(_type value)
		{
			*this = *this - value;

			return *this;
		}

		friend Matrix operator*(const Matrix &mat1, const Matrix &mat2)
		{
			Matrix
				res(mat1.getRowCount(), mat2.getColCount());

			for (register int i = 0; i < res.getRowCount(); i++)
				for (register int j = 0; j < res.getColCount(); j++)
				{
					_type
						item = 0;

					for (register int k = 0; k < mat1.getColCount(); k++)
						item += mat1.getItem(i, k) * mat2.getItem(k, j);

					res.setItem(i, j, item);
				}

			return res;
		}
		friend Matrix operator*(const Matrix &mat, _type value)
		{
			Matrix
				res(mat.getRowCount(), mat.getColCount());

			for (register int i = 0; i < mat.getRowCount(); i++)
				for (register int j = 0; j < mat.getColCount(); j++)
					res.setItem(i, j, mat.getItem(i, j) * value);

			return res;
		}
		friend Matrix operator*(_type value, const Matrix &mat)
		{
			return operator*(mat, value);
		}
		Matrix &operator*=(const Matrix &mat)
		{
			*this = *this * mat;

			return *this;
		}
		Matrix &operator*=(_type value)
		{
			*this = *this * value;

			return *this;
		}

		friend Matrix operator/(const Matrix &mat1, const Matrix &mat2)
		{
			return mat1 * mat2.reverse();
		}
		friend Matrix operator/(const Matrix &mat, _type value)
		{
			Matrix
				res(getRowCount(), getColCount());

			for (register int i = 0; i < getRowCont(); i++)
				for (register int j = 0; j < getColCount(); j++)
					res.setItem(i, j, mat.getItem(i, j) / value);

			return res;
		}
		friend Matrix operator/(_type value, const Matrix &mat)
		{
			return operator/(mat, value);
		}
		Matrix &operator/=(const Matrix &mat)
		{
			*this = *this / mat;

			return *this;
		}
		Matrix &operator/=(_type value)
		{
			*this = *this / value;

			return *this;
		}

	private:
			
		_type calcDet(const Matrix &mat)
		{
			_type
				det;
			Matrix
				matTemp;

			det = 0;

			switch (mat.getRowCount())
			{
			case 1:
				return mat.getItem(0, 0);
			case 2:
				return mat.getItem(0, 0) * mat.getItem(1, 1) - mat.getItem(1, 0) * mat.getItem(0, 1);
			default:
				for (register int i = 0; i < mat.getColCount(); i++)
				{
					matTemp = mat;
					matTemp.removeRow(0);
					matTemp.removeCol(i);
					det += mat.getItem(0, i) * (_type) pow(-1, i) * calcDet(matTemp);
				}
			}

			return det;
		}

	public:

		_type calcDet()
		{
			return calcDet(*this);
		}

	}; /* Matrix */

} // namespace CIVIL::MATH::GA2D

#endif // ifndef __CIVIL_MATRIX
