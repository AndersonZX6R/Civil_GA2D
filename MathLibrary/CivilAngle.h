#ifndef __CIVIL_ANGLE
#define __CIVIL_ANGLE

#define _USE_MATH_DEFINES

#ifdef _MANAGED
#pragma unmanaged
#endif // ifdef _MANAGED

#include <math.h>
#include <string>
#include <limits.h>
#include <regex>

#include "..\UtilsLibrary\CivilError.h"
#include "..\UtilsLibrary\CivilRange.h"

using namespace std;
using namespace CIVIL::UTILS;

namespace CIVIL::MATH::GA2D
{

	typedef Range<short int, 0, SHRT_MAX> TurnsType;
	typedef Range<short int, -359, 359> DegreesType;
	typedef Range<short int, 0, 59> MinutesType, SecondsType;

	DECLARE_ERROR_CODE(aeInvalidString);

	BEGIN_DECLARE_ERROR(EAngle)
		DECLARE_ERROR(aeInvalidString, "Invalid string")
	END_DECLARE_ERROR;

	enum AngleUnitEnum
	{
		auRadians,
		auDegrees
	};

	struct Angle
	{
	public:

		Angle() = default;
		Angle(double ang) :
			m_dblRadians(ang)
		{
			calcDegrees();
		}
		Angle(TurnsType turns, DegreesType deg, MinutesType min, SecondsType sec) :
			m_turns(turns),
			m_degrees(deg),
			m_minutes(min),
			m_seconds(sec)
		{
			calcRadians();
		}

	private:

		double
			m_dblRadians = 0;
		short int
			m_turns = 0,
			m_degrees = 0,
			m_minutes = 0,
			m_seconds = 0;

		void calcRadians();
		void calcDegrees();

	public:

		TurnsType getTurns() const
		{
			return m_turns;
		}
		void setTurns(const TurnsType &value)
		{
			if (m_turns == value) return;

			m_turns = value;
			calcRadians();
		}
		DegreesType getDegrees() const
		{
			return m_degrees;
		}
		void setDegrees(const DegreesType &value)
		{
			if (m_degrees == value) return;

			m_degrees = value;
			calcRadians();
		}
		MinutesType getMinutes() const
		{
			return m_minutes;
		}
		void setMinutes(const MinutesType &value)
		{
			if (m_minutes == value) return;

			m_minutes = value;
			calcRadians();
		}
		SecondsType getSeconds() const
		{
			return m_seconds;
		}
		void setSeconds(const SecondsType &value)
		{
			if (m_seconds == value) return;

			m_seconds = value;
			calcRadians();
		}

		string format(AngleUnitEnum unit) const
		{
			char
				chr[100];

			switch (unit)
			{
			case auDegrees:
				sprintf_s(chr, "%dº%d'%d\"", m_degrees + (m_degrees > 0 ? 1 : -1) * m_turns * 360, m_minutes, m_seconds);
				break;
			case auRadians:
				sprintf_s(chr, "%f", m_dblRadians);
				break;
			}

			return string(chr);
		}

		operator double() const
		{
			return m_dblRadians;
		}

		friend Angle operator+(const Angle &ang1, const Angle &ang2)
		{
			return Angle(ang1.m_dblRadians + ang2.m_dblRadians);
		}
		friend Angle operator+(const Angle &ang1, double ang2)
		{
			return Angle(ang1.m_dblRadians + ang2);
		}
		friend Angle operator+(double ang1, const Angle &ang2)
		{
			return Angle(ang1 + ang2.m_dblRadians);
		}
		Angle &operator+=(const Angle &ang)
		{
			*this = *this + ang;

			return *this;
		}
		Angle &operator+=(double ang)
		{
			*this = *this + ang;

			return *this;
		}

		friend Angle operator-(const Angle &ang1, const Angle &ang2)
		{
			return Angle(ang1.m_dblRadians - ang2.m_dblRadians);
		}
		friend Angle operator-(const Angle &ang1, double ang2)
		{
			return Angle(ang1.m_dblRadians - ang2);
		}
		friend Angle operator-(double ang1, const Angle &ang2)
		{
			return Angle(ang1 - ang2.m_dblRadians);
		}
		Angle &operator-=(const Angle &ang)
		{
			*this = *this - ang;

			return *this;
		}
		Angle &operator-=(double ang)
		{
			*this = *this - ang;

			return *this;
		}

		bool operator==(const Angle &ang) const
		{
			return m_dblRadians == ang.m_dblRadians;
		}
		bool operator!=(const Angle &ang) const
		{
			return m_dblRadians != ang.m_dblRadians;
		}
		bool operator>(const Angle &ang) const
		{
			return m_dblRadians > ang.m_dblRadians;
		}
		bool operator>=(const Angle &ang) const
		{
			return m_dblRadians >= ang.m_dblRadians;
		}
		bool operator<(const Angle &ang) const
		{
			return m_dblRadians < ang.m_dblRadians;
		}
		bool operator<=(const Angle &ang) const
		{
			return m_dblRadians <= ang.m_dblRadians;
		}

		static const Angle
			ANGLE_45,
			ANGLE_90,
			ANGLE_180,
			ANGLE_360;

	private:

		static regex
			m_diag;
		static short int
			m_turnsValid,
			m_degreesValid,
			m_minutesValid,
			m_secondsValid;

	public:

		static bool validateString(const string &str);

	}; /* Angle */

} // namespace CIVIL::MATH::GA2D

#endif // ifndef __CIVIL_ANGLE