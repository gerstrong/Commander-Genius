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
	showfps=true;
	Mode=0;
#if defined(CAANOO) || defined(WIZ) || defined(GP2X) || defined(DINGOO) || defined(NANONOTE)
	m_Resolution.width=320;
	m_Resolution.height=240;
#if defined(GP2X) || defined(NANONOTE)
	m_Resolution.depth=32;
#else
	m_Resolution.depth=16;
#endif
	Fullscreen=true;
#else
	m_Resolution.width=320;
	m_Resolution.height=200;
	m_Resolution.depth=32;
#if defined(ANDROID)
	m_Resolution.depth=16;
#endif
	Fullscreen=false;
#endif
	m_ScaleXFilter=1;
	Zoom=1;
	m_targetfps = 60;	// Enable automatic frameskipping by default at 30

#ifdef USE_OPENGL
	m_opengl_filter = GL_NEAREST;
#endif

	m_opengl = false; // Must stay optional for better compatibility
	m_aspect_correction = true;
	m_CameraBounds.reset();
	m_special_fx = true;

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
	m_opengl = true;
	Zoom = 1;
	m_ScaleXFilter = 1;
	m_targetfps = 30;
	m_aspect_correction = false;
#endif
}

void CVidConfig::setResolution(const int width, const int height, const int depth)
{
	const st_resolution res(width, height, depth);
	setResolution(res);
}

void CVidConfig::setResolution(const st_resolution& res)
{
	m_Resolution = res;
}

