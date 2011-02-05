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

#ifdef USE_OPENGL
#include "sdl/video/COpenGL.h"
#endif


struct st_resolution
{
	int width,height,depth;

	st_resolution(const int lwidth=0, const int lheight=0, const int ldepth=0) :
	width(lwidth), height(lheight), depth(ldepth) {};

	bool operator==(const st_resolution target)
	{
		return (target.depth == depth &&
				target.height == height &&
				target.width == width);
	}
};

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

class CVidConfig {
public:

	CVidConfig();
	bool operator==(const CVidConfig& target);
	bool operator!=(const CVidConfig& target);
	void reset();

	void setResolution(const int width, const int height, const int depth);
	void setResolution(const st_resolution& res);

	st_resolution m_Resolution;
	unsigned int Mode;
	bool Fullscreen;
	short m_ScaleXFilter;
	unsigned short Zoom;
	unsigned int m_targetfps;	// Used for automatic frame skipping
	bool m_opengl;
	bool m_aspect_correction;
	bool m_special_fx;
	bool showfps;

#ifdef USE_OPENGL
	int m_opengl_filter;
#endif

	st_camera_bounds m_CameraBounds;
};

#endif /* CVIDCONFIG_H_ */
