/*
 * CVidConfig.h
 *
 *  Created on: 04.02.2011
 *      Author: gerstrong
 *
 *  This class take the configuration of the Video Driver. Here we only have variables
 *  that can be setup by the user. For the driver code please refer to the CVideoDriver class
 */

#ifndef CVIDCONFIG_H_
#define CVIDCONFIG_H_

#include <SDL.h>
//#include "hardware/Configurator.h"
#include <base/video/scaler/CScaler.h>

const unsigned int RES_BPP = 32;

#ifdef USE_OPENGL
	#include <SDL_opengl.h>
#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
    #include <SDL_opengles.h>
#endif
#endif

#include <base/utils/Geometry.h>

struct st_camera_bounds
{
	int left, right, down, up, speed;

	void reset()
	{
		// Default camera settings
		left = 140;		up = 50;
		right = 180;	down = 130;
		speed = 5;
	}
};

class CVidConfig
{
public:

	CVidConfig();
	bool operator==(const CVidConfig& target);
	bool operator!=(const CVidConfig& target);
	void reset();

	void setResolution(const int width, const int height, const int depth);
	void setResolution(const GsRect<Uint16>& res);

	GsRect<Uint16> m_GameRect;	// Also called Screenspace.
	GsRect<Uint16> m_DisplayRect; // The Fullscreen-Resolution or the Window Size

	bool Fullscreen;
    filterOptionType m_ScaleXFilter;
	bool m_normal_scale;
	unsigned short Zoom;
	bool m_opengl;
	GsRect<int> mAspectCorrection;
	bool m_special_fx;
	bool vsync;

	st_camera_bounds m_CameraBounds;

#if defined(USE_OPENGL)
	GLint m_opengl_filter;
#endif
	
};

#endif /* CVIDCONFIG_H_ */
