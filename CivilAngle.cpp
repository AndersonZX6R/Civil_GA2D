#include "CivilAngle.h"

#include <math.h>
#include <iostream>

namespace CIVIL
{

namespace MATH
{

namespace GA2D
{

/*
 * Angle.
 * ---------------------------------------------------------------------------------------------------------------------------------------------------
 */

const Angle
	Angle::ANGLE_45 = Angle(0, 45, 0, 0),
	Angle::ANGLE_90 = Angle(0, 90, 0, 0),
	Angle::ANGLE_180 = Angle(0, 180, 0, 0),
	Angle::ANGLE_360 = Angle(1, 0, 0, 0);
regex
	Angle::m_diag = regex("^(-?[0-9]+)º(?:([0-5]?[0-9])')?(?:([0-5]?[0-9])\")?$", regex_constants::optimize);
short int
	Angle::m_turnsValid = 0,
	Angle::m_degreesValid = 0,
	Angle::m_minutesValid = 0,
	Angle::m_secondsValid = 0;

void
Angle::calcRadians()
{
	m_dblRadians = m_degrees + ((double) m_minutes / 60) + ((double) m_seconds / 3600);
	m_dblRadians = M_PI * m_dblRadians / 180;
	m_dblRadians += m_turns * M_PI * 2;
}

void
Angle::calcDegrees()
{
	double
		dblDecimal = 180 * (m_dblRadians / M_PI);
	
	m_turns = (short int) floor(abs(dblDecimal) / 360);

	if (dblDecimal < 0)
		dblDecimal += (m_turns * 360);
	else
		dblDecimal -= (m_turns * 360);

	double
		dblMinute;

	m_degrees = (short int) floor(dblDecimal);
	dblMinute = abs((dblDecimal - m_degrees) * 60);
	m_minutes = (short int) floor(dblMinute);
	m_seconds = (short int) floor((dblMinute - m_minutes) * 60);
}

bool
Angle::validateString(const string &str)
{
	std::cmatch
		m;

	if (regex_match(str.c_str(), m, m_diag))
	{
		short int
			intDeg;
		std::cmatch::iterator
			it = m.end();

		m_turnsValid = 0;
		m_degreesValid = 0;
		m_minutesValid = 0;
		m_secondsValid = 0;

		it--;
		if (it->matched)
			m_secondsValid = atoi(it->str().c_str());
		it--;
		if (it->matched)
			m_minutesValid = atoi(it->str().c_str());
		it--;
		if (it->matched)
		{
			intDeg = atoi(it->str().c_str());
			m_turnsValid = (short int)floor(abs(intDeg) / 360);

			if (m_turnsValid > 0)
				m_degreesValid = intDeg - m_turnsValid * 360;
			else
				m_degreesValid = intDeg + m_turnsValid * 360;
		}

		return true;
	}
	else
		return false;
}

} // namespace GA2D

} // namespace MATH

} // namespace Civil
