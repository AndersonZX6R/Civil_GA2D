#ifndef __CIVIL_DYN_ARRAY
#define __CIVIL_DYN_ARRAY

#include <iostream>
#include <limits>

#include "CivilRange.h"

namespace CIVIL
{

namespace UTILS
{
	template <typename _type>
	struct DynArray
	{
	public:

		typedef CIVIL::UTILS::Range<short int, 1, SHRT_MAX> SizeType;
		typedef CIVIL::UTILS::Range<short int, 0, SHRT_MAX - 1> IndexType;

		DynArray()
		{
			m_pItems = (_type **)malloc(sizeof(_type *));
		}
		DynArray(const SizeType &rowCount, const SizeType &colCount) :
			m_intRowCount(rowCount),
			m_intColCount(colCount)
		{
			m_pItems = (_type **)malloc(sizeof(_type *) * rowCount);

			for (register int i = 0; i < rowCount; i++)
				m_pItems[i] = (_type *)malloc(sizeof(_type) * colCount);
		}
		DynArray(const DynArray &da) :
			DynArray(da.getRowCount(), da.getColCount())
		{
			for (register int i = 0; i < da.getRowCount(); i++)
				for (register int j = 0; j < da.getColCount(); j++)
					setItem(i, j, da.getItem(i, j));
		}
		~DynArray()
		{
			for (register int i = 0; i < m_intRowCount; i++)
				free(m_pItems[i]);

			if (m_intRowCount > 0)
				free(m_pItems);
		}

	private:

		_type
			**m_pItems;
		short int
			m_intRowCount = 0,
			m_intColCount = 0;

	public:

		SizeType getRowCount() const
		{
			return m_intRowCount;
		}
		int getColCount() const
		{
			return m_intColCount;
		}
		void setDims(const SizeType &rows, const SizeType &cols)
		{
			if (rows == m_intRowCount && cols == m_intColCount) return;

			register int
				i;

			if (rows < m_intRowCount)
			{

				for (i = 0; i < rows; i++)
					m_pItems[i] = (_type *)realloc(m_pItems[i], sizeof(_type) * cols);
				for (i = rows; i < m_intRowCount; i++)
					free(m_pItems[i]);

				m_pItems = (_type **)realloc(m_pItems, rows * sizeof(_type *));

			}
			else if (rows > m_intRowCount)
			{

				m_pItems = (_type **)realloc(m_pItems, rows * sizeof(_type *));

				for (i = 0; i < m_intRowCount; i++)
					m_pItems[i] = (_type *)realloc(m_pItems[i], sizeof(_type) * cols);
				for (i = m_intRowCount; i < rows; i++)
					m_pItems[i] = (_type *)malloc(cols * sizeof(_type));

			}
			else
			{
				for (i = 0; i < m_intRowCount; i++)
					m_pItems[i] = (_type *)realloc(m_pItems[i], sizeof(_type) * cols);
			}

			m_intRowCount = rows;
			m_intColCount = cols;
		}

		/*void setColCount(const SizeType &value)
		{
			if (value == m_intColCount) return;

			for (int i = 0; i < m_intRowCount; i++)
			{
				m_pItems[i] = (_type *)realloc(m_pItems[i], value * sizeof(_type));
			}

			m_intColCount = value;
		}*/

		_type getItem(const IndexType &row, const IndexType &col) const
		{
			return m_pItems[row][col];
		}
		void setItem(const IndexType &row, const IndexType &col, const _type &value)
		{
			m_pItems[row][col] = value;
		}

		DynArray &operator=(const DynArray &value)
		{
			setDims(value.getRowCount(), value.getColCount());

			for (register int i = 0; i < value.getRowCount(); i++)
				for (register int j = 0; j < value.getColCount(); j++)
					setItem(i, j, value.getItem(i, j));

			return *this;
		}

	}; /* DynArray */

} // namespace UTILS

} // namespace CIVIL

#endif // ifndef __CIVIL_DYN_ARRAY