/*
 * CGUIControl.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include "CGUIControl.h"
#include "sdl/CVideoDriver.h"

CGUIControl::CGUIControl() :
mEnabled(true)
{
	CRect<float> NewRect(0.0f, 0.0f, 0.0f, 0.0f);
	mRect = NewRect;
}

