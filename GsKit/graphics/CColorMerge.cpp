/*
 * CColorMerge.cpp
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 */

#include "CColorMerge.h"
#include <base/video/CVideoDriver.h>


CColorMerge::CColorMerge(const Uint8 speed) :
m_Speed(speed)
{
    getSnapshot();
}

// use this function. If you don't that, the effect won't work.
void CColorMerge::getSnapshot()
{
    gVideoDriver.collectSurfaces();
    mOldSurface.createCopy(gVideoDriver.mpVideoEngine->gameSfc());
}

// Effect cycle
void CColorMerge::ponder(const float deltaT)
{
	// Process the effect
    mOldSurface.setAlpha(255-m_Alpha);
	
	const int sum = m_Alpha + m_Speed;

	if(sum > 255)
	{
	  m_Alpha = 255;
	  mFinished = true;
	}
	else 
	{
	  m_Alpha += m_Speed;
	}
}

void CColorMerge::render()
{
    mOldSurface.blitTo(gVideoDriver.gameSfc());
}


