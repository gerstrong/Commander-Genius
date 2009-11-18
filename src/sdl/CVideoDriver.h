/*
 * CVideoDriver.h
 *
 *  Created on: 17.03.2009
 *      Author: gerstrong
 */

#ifndef CVIDEODRIVER_H_
#define CVIDEODRIVER_H_

#include "../CSingleton.h"
#define g_pVideoDriver CVideoDriver::Get()

struct st_resolution
{ int width,height,depth; };

#ifdef USE_OPENGL
#include "COpenGL.h"
#endif

#include <SDL/SDL.h>
#include <iostream>
#include <list>

inline bool LockSurface(SDL_Surface * bmp)  {
	if (SDL_MUSTLOCK(bmp))
		return SDL_LockSurface(bmp) != -1;
	return true;
}

inline void UnlockSurface(SDL_Surface * bmp)  {
	if (SDL_MUSTLOCK(bmp))
		SDL_UnlockSurface(bmp);
}

class CVideoDriver : public CSingleton<CVideoDriver>
{
public:
	CVideoDriver();
	virtual ~CVideoDriver();

	bool applyMode(void);
	bool createSurfaces(void);
	SDL_Surface* createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format );
	void stop(void);
	bool start(void);
	void isFullscreen(bool value);
	void drawConsoleMessages(void);
	void initResolutionList();

	void pal_set(short colour, Uint8 red, Uint8 green, Uint8 blue);
	void pal_apply(void);

	void blitScrollSurface(Sint16 sbufferx, Sint16 sbuffery);
	void update_screen(void);
	void noscale(char *dest, char *src, short bbp);
	void scale2xnofilter(char *dest, char *src, short bbp);
	void scale3xnofilter(char *dest, char *src, short bbp);
	void scale4xnofilter(char *dest, char *src, short bbp);

	void DeleteConsoleMsgs(void);
	void AddConsoleMsg(const char *the_msg);

	short getZoomValue(void);
	bool getShowFPS(void);
	short getFiltermode(void);
	bool getFullscreen(void);
	unsigned int getWidth(void);
	unsigned int getHeight(void);
	unsigned short getDepth(void);
	unsigned int getWidthw(void);
	unsigned int getHeightw(void);
	unsigned short getDepthw(void);
	unsigned int getWidthf(void);
	unsigned int getHeightf(void);
	unsigned short getDepthf(void);
	std::string getWidthwf(void);
	std::string getHeightwf(void);
	std::string getDepthwf(void);
	bool isOpenGL(void) { return m_opengl; }
#ifdef USE_OPENGL
	unsigned char getOGLFilter(void) { return (m_opengl_filter==GL_LINEAR); }
#else
	unsigned char getOGLFilter(void) { return 0; }
#endif
	SDL_Surface *getScrollSurface(void);

	void setMode(int width, int height,int depth);
	void setFilter(short value);
	void setZoom(short vale);
	bool initOpenGL();
#ifdef USE_OPENGL
	void enableOpenGL(bool value) { m_opengl = value; }
	void setOGLFilter(unsigned char value) { m_opengl_filter = (value==1) ? GL_LINEAR : GL_NEAREST ; }
#else
	void enableOpenGL(bool value) { m_opengl = false; }
	void setOGLFilter(unsigned char value) { m_opengl_filter = 0; }
#endif
	st_resolution setNextResolution();

	void showFPS(bool value);

	void setAspectCorrection(bool value) { m_aspect_correction = value; }
	bool getAspectCorrection(void) { return m_aspect_correction; }
	SDL_Rect getGameResRect() { return game_resolution_rect; }

	SDL_Surface *screen;                // the actual video memory/window
	SDL_Surface *BlitSurface;
	SDL_Surface *FGLayerSurface;       	// Scroll buffer for Messages
	SDL_Surface *ScrollSurface;       	// 512x512 scroll buffer
	SDL_Surface *FXSurface;

	bool m_fading;
	std::list<st_resolution> m_Resolutionlist;

	Uint16 m_scrollx_buf;
	Uint16 m_scrolly_buf;

	bool showfps;

private:
#ifdef USE_OPENGL
	COpenGL	*mp_OpenGL;
#endif

	st_resolution m_Resolution;

	std::list<st_resolution> m_Resolutionlistempty;
	std::list<st_resolution> :: iterator m_Resolution_pos;

	unsigned int Mode;
	bool Fullscreen;
	short Filtermode;
	unsigned short Zoom;
	unsigned short FrameSkip;
	unsigned int m_targetfps;	// Used for automatic frame skipping
	bool m_opengl;
	int m_opengl_filter;
	bool m_aspect_correction;

	SDL_Rect screenrect;
	SDL_Rect blitrect;

	SDL_Rect game_resolution_rect;
};
#endif /* CVIDEODRIVER_H_ */
