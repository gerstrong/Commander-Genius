/*
 * CCamera.cpp
 *
 *  Created on: 09.03.2010
 *      Author: pizza2004
 */

#include "CCamera.h"
#include "../../CLogFile.h"

CCamera::CCamera() :
m_scrolltriggerright(168),
m_scrolltriggerleft(152),
m_scrolltriggerup(92),
m_scrolltriggerdown(108),
m_scrollspeed(20)
{
	g_pLogFile->ftextOut("Starting the camera system...<br>");
}

void CCamera::setScrollTriggers(int left, int up, int right, int down, int speed)
{
	m_scrolltriggerleft = left;
	m_scrolltriggerup = up;
	m_scrolltriggerright = right;
	m_scrolltriggerdown = down;
	m_scrollspeed = speed;
}

CCamera::~CCamera()
{
	// TODO Auto-generated destructor stub
}
