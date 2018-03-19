/***
 * Error.h
 *
 * Copyright(c) 2013 - Eng.º Anderson Marques Ribeiro.
 *
 * 06 de Janeiro de 2013.
 */

#ifndef __CIVIL_ERROR
#define __CIVIL_ERROR

#include <string>

using namespace std;

namespace CIVIL
{

namespace UTILS
{

#define DECLARE_ERROR_CODE( name ) \
	const CIVIL::UTILS::ErrorCodeType name = CIVIL::UTILS::LAST_ERROR_CODE++

#define BEGIN_DECLARE_ERROR( name ) \
class name : public CIVIL::UTILS::Error \
{ \
public: \
	name (CIVIL::UTILS::ErrorCodeType ec) : CIVIL::UTILS::Error(ec) \
	{} \
protected: \
	string getErrorMessage(CIVIL::UTILS::ErrorCodeType ec) const \
	{

#define DECLARE_ERROR( name, message ) \
		if (name == getError()) return message;

#define END_DECLARE_ERROR \
		return CIVIL::UTILS::Error::getErrorMessage(ec); \
	} \
}

#define RAISE( errorName, errorCode ) throw errorName( errorCode )

	typedef long ErrorCodeType;

	class Error
	{
	public:

		Error(ErrorCodeType err) :
			m_ec(err)
		{};

	private:

		ErrorCodeType
			m_ec;

	protected:

		virtual string getErrorMessage(ErrorCodeType ec) const
		{
			return "Unknown error";
		};

	public:

		ErrorCodeType getError() const
		{
			return m_ec;
		};
		string getMessage() const
		{
			return getErrorMessage(getError());
		};

	}; // Error

	extern ErrorCodeType LAST_ERROR_CODE;

} // namespace UTILS

} // namespace CIVIL

#endif // ifndef __CIVIL_ERROR
