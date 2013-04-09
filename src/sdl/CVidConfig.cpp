/*
 * CVidConfig.cpp
 *
 *  Created on: 04.02.2011
 *      Author: gerhard
 */

#include "CVidConfig.h"


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
	showfps = false;
	vsync = true;
	
#if defined(CAANOO) || defined(WIZ) || defined(GP2X) || defined(DINGOO) || defined(NANONOTE)
	//m_Resolution.width=320;
	//m_Resolution.height=240;
	m_DisplayRect.w=320;
	m_DisplayRect.h=240;
#if defined(GP2X) || defined(NANONOTE)
	//m_Resolution.depth=32;
#else
	//m_Resolution.depth=16;
#endif
	Fullscreen=true;
#else
	m_DisplayRect.w=320;
	m_DisplayRect.h=200;
	Fullscreen=false;
#endif

	m_ScaleXFilter=1;
	m_normal_scale=true;
	Zoom=1;

#ifdef USE_OPENGL
	m_opengl_filter = GL_LINEAR;
#endif

	m_opengl = false; // Must stay optional for better compatibility
	mAspectCorrection.w=4;
	mAspectCorrection.h=3;
	m_CameraBounds.reset();
	m_special_fx = true;

	m_GameRect.x = 0;
	m_GameRect.y = 0;
	m_GameRect.w = 320;
	m_GameRect.h = 200;

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	m_opengl = true;
	mAspectCorrection.w=0;
	mAspectCorrection.h=0;	
#endif

#if defined(ANDROID)
	mAspectCorrection.w=0;
	mAspectCorrection.h=0;	
#endif
	
}


void CVidConfig::setResolution(const int width, const int height, const int depth)
{
	const CRect<Uint16> res(width, height);
	setResolution(res);
}

void CVidConfig::setResolution(const CRect<Uint16>& res)
{
	m_DisplayRect = res;
}
