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

const unsigned int RES_BPP = 16;

#ifdef USE_OPENGL
	#include <SDL_opengl.h>
#endif

#include "utils/Geometry.h"

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
	void setResolution(const CRect<Uint16>& res);

	CRect<Uint16> m_GameRect;	// Also called Screenspace.
	CRect<Uint16> m_DisplayRect; // The Fullscreen-Resolution or the Window Size

	bool Fullscreen;
	short m_ScaleXFilter;
	unsigned short Zoom;
	bool m_opengl;
	bool m_aspect_correction;
	bool m_special_fx;
	bool showfps;
	bool vsync;

#ifdef USE_OPENGL
	GLint m_opengl_filter;
#endif

	st_camera_bounds m_CameraBounds;
};

#endif /* CVIDCONFIG_H_ */
