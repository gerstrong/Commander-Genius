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

#include <SDL.h>
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

	bool applyMode();
	SDL_Rect adaptGameResolution();
	SDL_Surface* createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format );
	//void resetSettings();
	//bool createSurfaces(void);

	void stop(void);
	bool start(void);
	void isFullscreen(bool value);
	void drawConsoleMessages(void);

	void pal_set(short colour, Uint8 red, Uint8 green, Uint8 blue);
	void pal_apply(void);

	void blitScrollSurface();
	void setScrollBuffer(Sint16 *pbufx, Sint16 *pbufy);

	void collectSurfaces();
	void updateScreen();
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
	SDL_Rect getGameResolution() { return game_resolution_rect; }

	SDL_Surface *getBlitSurface() { return BlitSurface; }

	bool isOpenGL(void) { return m_opengl; }
#ifdef USE_OPENGL
	unsigned char getOGLFilter(void) { return (m_opengl_filter==GL_LINEAR); }
#else
	unsigned char getOGLFilter(void) { return 0; }
#endif
	SDL_Surface *getScrollSurface(void);

	void setMode(st_resolution Resolution);

	void setMode(int width, int height,int depth);
	void setSpecialFXMode(bool SpecialFX);
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
	void checkResolution( st_resolution& resolution, int flags );
	st_resolution getResolution(){ return *m_Resolution_pos; }
	st_resolution getNextResolution();

	void showFPS(bool value);
	void initResolutionList();

	void setAspectCorrection(bool value) { m_aspect_correction = value; }
	bool getAspectCorrection(void) { return m_aspect_correction; }
	bool getSpecialFXConfig(void) { return m_special_fx; }

	virtual ~CVideoDriver();

	SDL_Surface *BlitSurface;
	SDL_Surface *FGLayerSurface;       	// Scroll buffer for Messages
	SDL_Surface *ScrollSurface;       	// 512x512 scroll buffer
	SDL_Surface *FXSurface;

	std::list<st_resolution> m_Resolutionlist;

	bool showfps;

private:

	bool createSurfaces();

#ifdef USE_OPENGL
	COpenGL	*mp_OpenGL;
#endif
	st_resolution m_Resolution;

	std::list<st_resolution> m_Resolutionlistempty;
	std::list<st_resolution> :: iterator m_Resolution_pos;

	unsigned int Mode;
	bool Fullscreen;
	short m_ScaleXFilter;
	unsigned short Zoom;
	unsigned int m_targetfps;	// Used for automatic frame skipping
	bool m_opengl;
	int m_opengl_filter;
	int m_maxwidth;
	bool m_aspect_correction;
	bool m_special_fx;
	float m_aspect_ratio;

	SDL_Rect screenrect;
	SDL_Rect blitrect;
	SDL_Rect game_resolution_rect;	// Also called Screenspace. Yet very limited.

	Sint16 *mp_sbufferx, *mp_sbuffery;

	SDL_Surface *screen;                // the actual video memory/window

	// Those variables are used for the rendering process, so they don't need to be recalculated
	unsigned m_dst_slice, m_src_slice;
	bool m_blitsurface_alloc;

	// pointer to the line in VRAM to start blitting to when stretchblitting.
	// this may not be the first line on the display as it is adjusted to
	// center the image on the screen when in fullscreen.
	unsigned char *VRAMPtr;
};
#endif /* CVIDEODRIVER_H_ */
