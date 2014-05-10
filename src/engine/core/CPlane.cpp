/*
 * CPlane.cpp
 *
 *  Created on: 10.06.2010
 *      Author: gerstrong
 */

#include "CPlane.h"

CPlane::CPlane() :
mp_data(NULL),
m_width(0),
m_height(0)
{}

void CPlane::createDataMap(Uint16 width, Uint16 height)
{
	if(mp_data) delete [] mp_data;

	m_width = width;
	m_height = height;
	mp_data = new word[m_width*m_height];
}

word *CPlane::getMapDataPtr()
{
	return mp_data;
}

word CPlane::getMapDataAt(Uint16 x, Uint16 y)
{
	if( x < m_width && y < m_height )
		return mp_data[y*m_width+x];
	return 0;
}

void CPlane::setMapDataAt(Uint16 t, Uint16 x, Uint16 y)
{
	mp_data[y*m_width+x] = t;
}

bool CPlane::empty()
{	return (mp_data == NULL);	}

CPlane::~CPlane()
{
	if(mp_data)	delete [] mp_data;
	mp_data = NULL;
}
