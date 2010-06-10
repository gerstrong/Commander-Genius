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
{
	// TODO Auto-generated constructor stub

}

void CPlane::createDataMap(size_t blocksize, Uint16 width, Uint16 height)
{
	if(mp_data) delete [] mp_data;

	m_width = width;
	m_height = height;
	mp_data = new word[blocksize];
}

word *CPlane::getMapDataPtr()
{
	return mp_data;
}

word CPlane::getMapDataAt(Uint16 x, Uint16 y)
{
	return mp_data[y*m_width+x];
}

void CPlane::setMapDataAt(Uint16 t, Uint16 x, Uint16 y)
{
	mp_data[y*m_width+x] = t;
}

CPlane::~CPlane()
{
	if(mp_data)	delete [] mp_data;
	mp_data = NULL;
}
