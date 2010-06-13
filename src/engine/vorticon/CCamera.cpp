/*
 * CCamera.cpp
 *
 *  Created on: 09.03.2010
 *      Author: pizza2004
 */

#include "CCamera.h"
#include "../../CLogFile.h"

CCamera::CCamera()
{
	g_pLogFile->ftextOut("Starting the camera system...<br>");
	setDefaults();
}

void CCamera::setDefaults()
{
	m_scrolltriggerright = 168;
	m_scrolltriggerleft = 152;
	m_scrolltriggerup = 92;
	m_scrolltriggerdown = 108;
	m_scrollspeed = 20;
}

void CCamera::setScrollTriggers(int left, int up, int right, int down, int speed)
{
	bool invalid_value = (left<0) || (up<0) || (right<0) || (down<0) || (speed<0);
	if(invalid_value)
		setDefaults();
	else
	{
		m_scrolltriggerleft = left;
		m_scrolltriggerup = up;
		m_scrolltriggerright = right;
		m_scrolltriggerdown = down;
		m_scrollspeed = speed;
	}
}

CCamera::~CCamera()
{
	// TODO Auto-generated destructor stub
}
