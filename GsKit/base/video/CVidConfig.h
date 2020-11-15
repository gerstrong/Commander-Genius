/*
 * CVidConfig.h
 *
 *  Created on: 04.02.2011
 *      Author: gerstrong
 *
 *  This class take the configuration of the Video Driver. Here we only have variables
 *  that can be setup by the user. For the driver code please refer to the CVideoDriver class
 */

#ifndef CVIDCONFIG_H
#define CVIDCONFIG_H

#include <SDL.h>
#include <base/video/scaler/CScaler.h>
#include <base/utils/Color.h>

#include <string>

const unsigned int RES_BPP = 32;

#ifdef USE_OPENGL
	#include <SDL_opengl.h>
//#if (TARGET_OS_IPHONE) || (TARGET_IPHONE_SIMULATOR)
//    #include <SDL_opengles.h>
//#endif
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
    void setResolution(const GsVec2D<Uint16> &res);

    GsRect<Uint16> mGameRect;	// Also called Screenspace.
    GsRect<Uint16> mDisplayRect; // The Fullscreen-Resolution or the Window Size

    GsColor mBorderColors; // Colors of the border (Clear colors)
    bool mBorderColorsEnabled = false;

    int mHorizBorders = 0; // Horizontal border bars for a more CRT feeling

#ifdef __SWITCH__
    bool mFullscreen = true;
    bool mIntegerScaling = false;
#else
    bool mFullscreen = false;
    bool mIntegerScaling = true;
#endif
    VidFilter m_ScaleXFilter = VidFilter::NONE;
	bool m_normal_scale;

	unsigned short Zoom;

#ifdef USE_OPENGL
    bool mOpengl = true;
#else
    bool mOpengl = false;
#endif



#ifdef USE_VIRTUALPAD
#ifdef ANDROID
    bool mVPad = true;
#else
    bool mVPad = false;
#endif
    int mVPadWidth = 150;
    int mVPadHeight = 150;
#endif

	GsRect<int> mAspectCorrection;


    bool mVSync = false;

    bool mShowCursor = true;

	st_camera_bounds m_CameraBounds;

    bool mTiltedScreen = false;


    enum class RenderQuality
    { NEAREST, LINEAR }
#ifdef __SWITCH__
    mRenderScQuality = RenderQuality::NEAREST;
#else
    mRenderScQuality = RenderQuality::LINEAR;
#endif
};

#endif /* CVIDCONFIG_H */
