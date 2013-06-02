/*
 * CVideoDriver.h
 *
 *  Created on: 17.03.2009
 *      Author: gerstrong
 *
 *  The driver is in charge of driving the card the card. This means it will do everthing
 *  needed to get the stuff rendered properly. It will also do some checks wether it's possible to accomplish something
 *  or not.
 */

#ifndef CVIDEODRIVER_H_
#define CVIDEODRIVER_H_

#include "CSingleton.h"
#include "CVidConfig.h"
#include "video/CVideoEngine.h"
#include "video/DrawEvents.h"
#include "engine/CEventContainer.h"
#include "common/CMap.h"

#ifdef USE_OPENGL
	#include "sdl/video/COpenGL.h"
#endif


#define g_pVideoDriver CVideoDriver::Get()

#include <SDL.h>
#include <iostream>
#include <list>
#include <memory>

class CVideoDriver : public CSingleton<CVideoDriver>
{
public:
	CVideoDriver();
	~CVideoDriver();
	void resetSettings();
	
	bool applyMode();
	SDL_Surface* createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format );
	
	bool start();
	void isFullscreen(bool value);

	void blitScrollSurface();
	void updateScrollBuffer(CMap &map);
	void updateScrollBuffer(std::shared_ptr<CMap> &map)
	{ updateScrollBuffer(*map.get()); }	
	
    SDL_Surface *getOverlaySurface()
    { return mpVideoEngine->getOverlaySurface(); }

	void collectSurfaces();
	void clearSurfaces();
	void updateScreen();

	// Drawing related stuff
	void pollDrawingTasks();
	SDL_Rect toBlitRect(const CRect<float> &rect);
	void clearDrawingTasks();

	/**
	 * \description This function saves the given camera bounds. It is usually called
	 * 				by a menu item.
	 * \param		CameraBounds	The Camera Bound configuration as input.
	 * 								Those might be corrected depending on how the user
	 * 								chose them.
	 */
	void saveCameraBounds(st_camera_bounds &CameraBounds);

	CVidConfig &getVidConfig();
	short getZoomValue();
	bool getShowFPS();
	bool getFullscreen();
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	unsigned short getDepth() const;
	CRect<Uint16> getGameResolution() const { return m_VidConfig.m_GameRect; }

	SDL_Surface *getBlitSurface() { return mpVideoEngine->getBlitSurface(); }

	bool isOpenGL(void) { return m_VidConfig.m_opengl; }
#ifdef USE_OPENGL
	unsigned char getOGLFilter(void) { return m_VidConfig.m_opengl_filter; }
#else
	unsigned char getOGLFilter(void) { return 0; }
#endif
	SDL_Surface *getScrollSurface(void);

	void setVidConfig(const CVidConfig& VidConf);
	void setMode(int width, int height,int depth);
	void setMode(const CRect<Uint16>& res);
	void setSpecialFXMode(bool SpecialFX);
	void setFilter(short value);
	void setScaleType(bool IsNormal);
	void setZoom(short vale);
#ifdef USE_OPENGL
	void enableOpenGL(bool value) { m_VidConfig.m_opengl = value; }
	void setOGLFilter(GLint value) { m_VidConfig.m_opengl_filter = value; }
#else
	void enableOpenGL(bool value) { m_VidConfig.m_opengl = false; }
	void setOGLFilter(unsigned char value) { }
#endif

	/*
	 * \brief Check whether this resolution is okay to be used or needs some adjustments if possible.
	 * 		  It could be that, the screen dim can be used but instead of 32bpp 16bpp. This function
	 * 		  will check and adapt it to the resolution your supports
	 * \param resolution The resolution structure of the mode it is desired to be used
	 * \param SDL uses some flags like Fullscreen or HW Acceleration, those need to be passed in order to verify
	 *        the mode properly.
	 * \return nothing. It does not return because it always adapts the resolution to some working mode.
	 *         If video cannot be opened at all, another function of LibSDL will find that out.
	 */
	void verifyResolution( CRect<Uint16>& resolution, const int flags );
	CRect<Uint16>& getResolution() const { return *m_Resolution_pos; }

	void initResolutionList();

	void setAspectCorrection(const int w, const int h) 
	{ 
	  m_VidConfig.mAspectCorrection.w = w; 
	  m_VidConfig.mAspectCorrection.h = h; 
	}
	//bool getAspectCorrection(void) { return m_VidConfig.m_aspect_correction; }
	bool getSpecialFXConfig(void) { return m_VidConfig.m_special_fx; }
	bool getRefreshSignal() { return m_mustrefresh; }
	void setRefreshSignal(const bool value) { m_mustrefresh = value;  }

	st_camera_bounds &getCameraBounds();

	std::unique_ptr<CVideoEngine> mpVideoEngine;

	CEventContainer mDrawTasks;

	std::list< CRect<Uint16> > m_Resolutionlist;
	std::list< CRect<Uint16> > :: iterator m_Resolution_pos;

private:

	CVidConfig m_VidConfig;
	bool m_mustrefresh;
	bool mSDLImageInUse;
};
#endif /* CVIDEODRIVER_H_ */
