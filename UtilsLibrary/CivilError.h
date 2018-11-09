/***
 * CivilTokenAnalisys.h
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 - 2018 Eng.º Anderson Marques Ribeiro.
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

#ifndef __CIVIL_ERROR
#define __CIVIL_ERROR

#include <iostream>
#include <string>

using namespace std;

namespace CIVIL::UTILS
{
	// constexpr counter by Filip Roséen
	// filip.roseen@gmail.com
	// http://b.atch.se/posts/constexpr-counter/

	template<int N>
	struct flag {
		friend constexpr int adl_flag(flag<N>);
	};

	template<int N>
	struct writer {
		friend constexpr int adl_flag(flag<N>) {
			return N;
		}

		static constexpr int value = N;
	};

	template<int N, int = adl_flag(flag<N> {}) >
	int constexpr reader(int, flag<N>) {
		return N;
	}

	template<int N>
	int constexpr reader(float, flag<N>, int R = reader(0, flag<N - 1> {})) {
		return R;
	}

	int constexpr reader(float, flag<0>) {
		return 0;
	}

	template<int N = 1>
	int constexpr next(int R = writer < reader(0, flag<32> {}) + N > ::value) {
		return R;
	}

#define DECLARE_ERROR_CODE( name ) \
	const CIVIL::UTILS::ErrorCodeType name = next()

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
	if (name == ec) return message;

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

} // namespace CIVIL::UTILS

#endif // ifndef __CIVIL_ERROR