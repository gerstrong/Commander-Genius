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
	void updateScreen();

	void DeleteConsoleMsgs(void);
	void AddConsoleMsg(const char *the_msg);

	void saveCameraBounds(st_camera_bounds &CameraBounds);

	CVidConfig &getVidConfig();
	short getZoomValue(void);
	bool getShowFPS(void);
	short getFiltermode(void);
	bool getFullscreen(void);
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	unsigned short getDepth() const;
	SDL_Rect getGameResolution() { return m_VidConfig.m_Gamescreen; }

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
	void setMode(st_resolution &res);
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
	void checkResolution( st_resolution& resolution, int flags );
	st_resolution getResolution() const { return *m_Resolution_pos; }

	void initResolutionList();

	void setAspectCorrection(bool value) { m_VidConfig.m_aspect_correction = value; }
	bool getAspectCorrection(void) { return m_VidConfig.m_aspect_correction; }
	bool getSpecialFXConfig(void) { return m_VidConfig.m_special_fx; }
	bool getRefreshSignal() { return m_mustrefresh; }
	void setRefreshSignal(const bool value) { m_mustrefresh = value;  }

	st_camera_bounds &getCameraBounds();

	virtual ~CVideoDriver();

	CVideoEngine *mp_VideoEngine;

	std::list<st_resolution> m_Resolutionlist;
	std::list<st_resolution> :: iterator m_Resolution_pos;

private:

	CVidConfig m_VidConfig;
	bool m_mustrefresh;

	Sint16 *mp_sbufferx;
	Sint16 *mp_sbuffery;
};
#endif /* CVIDEODRIVER_H_ */
