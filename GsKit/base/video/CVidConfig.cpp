/*
 * CVidConfig.cpp
 *
 *  Created on: 04.02.2011
 *      Author: gerhard
 */

#include "CVidConfig.h"

#include <cstring>
#include <cstdio>

CVidConfig::CVidConfig()
{
	reset();
}

bool CVidConfig::operator==(const CVidConfig& target)
{
	return ( memcmp( &target, this, sizeof(CVidConfig)) == 0);
}

bool CVidConfig::operator!=(const CVidConfig& target)
{
	return !(*this == target);
}

void CVidConfig::reset()
{
	// Default values
	mVSync = true;

    mDisplayRect.pos.x = 0;
    mDisplayRect.pos.y = 0;

#if defined(CAANOO) || defined(WIZ) || defined(GP2X) || defined(DINGOO) || defined(NANONOTE)
    mDisplayRect.dim.x=320;
    mDisplayRect.h=240;
#if defined(GP2X) || defined(NANONOTE)
	//m_Resolution.depth=32;
#else
	//m_Resolution.depth=16;
#endif
	Fullscreen=true;
#else
    mDisplayRect.dim.x = 800;
    mDisplayRect.dim.y = 600;
	mFullscreen=false;
#endif

    m_ScaleXFilter = NONE;
    m_normal_scale = true;
	Zoom=1;

#ifdef USE_OPENGL
    mOpengl = true;
#else
    mOpengl = false; // Must stay optional for better compatibility
#endif

    mAspectCorrection.dim.x = 4;
    mAspectCorrection.dim.y = 3;
	m_CameraBounds.reset();

    mGameRect.pos.x = 0;
    mGameRect.pos.y = 0;
    //mGameRect.dim.x = 960;
    //mGameRect.dim.y = 540;
    mGameRect.dim.x = 1920;
    mGameRect.dim.y = 1080;

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    mOpengl = true;
	mAspectCorrection.dim.x=0;
	mAspectCorrection.h=0;
#endif

#if defined(ANDROID)
	mAspectCorrection.dim.x=0;
	mAspectCorrection.h=0;
#endif

}


void CVidConfig::setResolution(const int width, const int height, const int depth)
{
	const GsRect<Uint16> res(width, height);
	setResolution(res);
}

void CVidConfig::setResolution(const GsRect<Uint16>& res)
{
    mDisplayRect = res;
}
