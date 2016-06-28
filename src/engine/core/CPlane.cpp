/*
 * CPlane.cpp
 *
 *  Created on: 10.06.2010
 *      Author: gerstrong
 */

#include "CPlane.h"

CPlane::CPlane() :
m_width(0),
m_height(0)
{}

void CPlane::createDataMap(Uint16 width, Uint16 height)
{
	m_width = width;
	m_height = height;

    mDataVec.assign(m_width*m_height, 0);
    mTimers.assign(m_width*m_height, 0);
}
