#ifndef __CIVIL_RANGE
#define __CIVIL_RANGE

#include "CivilError.h"

namespace CIVIL
{

namespace UTILS
{

DECLARE_ERROR_CODE(reRangeError);

BEGIN_DECLARE_ERROR(ERange)
	DECLARE_ERROR(reRangeError, "Invalid range value")
END_DECLARE_ERROR;

	template<typename Type, Type _min, Type _max>
	struct Range
	{
	public:

		Range(Type value = 0) :
			m_value(value)
		{}

	private:

		Type
			m_value;

	public:

		static const Type
			min = _min,
			max = _max;

		operator Type() const
		{
			return m_value;
		}
		void operator=(Type value)
		{
			if (value > _max || value < _min)
				RAISE(ERange, reRangeError);

			m_value = value;
		}

	}; /* Range */

} // namespace RANGE

} // namespace CIVIL

#endif // ifndef __CIVIL_RANGE
