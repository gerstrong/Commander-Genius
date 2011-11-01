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

#ifdef USE_OPENGL
	#include "sdl/video/COpenGL.h"
#endif


#define g_pVideoDriver CVideoDriver::Get()

#include <SDL.h>
#include <iostream>
#include <list>

class CVideoDriver : public CSingleton<CVideoDriver>
{
public:
	CVideoDriver();
	void resetSettings();
	
	bool applyMode();
	SDL_Surface* createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format );

	void stop();
	bool start();
	void isFullscreen(bool value);
	void drawConsoleMessages();

	void blitScrollSurface();
	void setScrollBuffer(Sint16 *pbufx, Sint16 *pbufy);

	void collectSurfaces();
	void clearSurfaces();
	void updateScreen();

	void DeleteConsoleMsgs(void);
	void AddConsoleMsg(const char *the_msg);

	// Drawing related stuff

	void pollDrawingTasks();
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

	SDL_Surface *getBlitSurface() { return mp_VideoEngine->getBlitSurface(); }

	bool isOpenGL(void) { return m_VidConfig.m_opengl; }
#ifdef USE_OPENGL
	unsigned char getOGLFilter(void) { return (m_VidConfig.m_opengl_filter==GL_LINEAR); }
#else
	unsigned char getOGLFilter(void) { return 0; }
#endif
	SDL_Surface *getScrollSurface(void);

	void setVidConfig(const CVidConfig& VidConf);
	void setMode(int width, int height,int depth);
	void setMode(const CRect<Uint16>& res);
	void setSpecialFXMode(bool SpecialFX);
	void setFilter(short value);
	void setZoom(short vale);
#ifdef USE_OPENGL
	void enableOpenGL(bool value) { m_VidConfig.m_opengl = value; }
	void setOGLFilter(unsigned char value) { m_VidConfig.m_opengl_filter = (value==1) ? GL_LINEAR : GL_NEAREST ; }
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

	void setAspectCorrection(bool value) { m_VidConfig.m_aspect_correction = value; }
	bool getAspectCorrection(void) { return m_VidConfig.m_aspect_correction; }
	bool getSpecialFXConfig(void) { return m_VidConfig.m_special_fx; }
	bool getRefreshSignal() { return m_mustrefresh; }
	void setRefreshSignal(const bool value) { m_mustrefresh = value;  }

	st_camera_bounds &getCameraBounds();

	virtual ~CVideoDriver();

	CVideoEngine *mp_VideoEngine;

	CEventContainer mDrawTasks;

	std::list< CRect<Uint16> > m_Resolutionlist;
	std::list< CRect<Uint16> > :: iterator m_Resolution_pos;

private:

	CVidConfig m_VidConfig;
	bool m_mustrefresh;

	Sint16 *mp_sbufferx;
	Sint16 *mp_sbuffery;
};
#endif /* CVIDEODRIVER_H_ */
