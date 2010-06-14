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
	int i;
	if(left>right)
	{
		i = left-right;
		i = i/2;
		left = left-i;
		right = right+i;
		if(left>right)
			left = left-1;
	}
	if(up>down)
	{
		i = up-down;
		i = i/2;
		up = up-i;
		down = down+i;
		if(up>down)
			up = up-1;
	}
	bool invalid_value = (left<50) || (up<50) || (right<50) || (down<50) || (speed<1) || (left>270) || (up>150) || (right>270) || (down>150) || (speed>50);
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
