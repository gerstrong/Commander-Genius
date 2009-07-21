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

#ifdef USE_OPENGL
#include "COpenGL.h"
#endif

#include <SDL.h>

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
	void stop(void);
	bool start(void);
	void setMode(unsigned int srcW, unsigned int srcH,
			  unsigned short srcD);
	void isFullscreen(bool value);
	//void reset(void);
	void drawConsoleMessages(void);

	void pal_set(short colour, char red, char green, char blue);
	void pal_apply(void);

	void sb_blit(void);
	void blitBGLayer(void);
	void update_screen(void);
	void noscale(char *dest, char *src, short bbp);
	void scale2xnofilter(char *dest, char *src, short bbp);
	void scale3xnofilter(char *dest, char *src, short bbp);
	void scale4xnofilter(char *dest, char *src, short bbp);

	void setpixel(unsigned int x, unsigned int y, unsigned char c);
	unsigned char getpixel(int x, int y);

	void DeleteConsoleMsgs(void);
	void AddConsoleMsg(const char *the_msg);

	short getZoomValue(void);
	unsigned short getFrameskip(void);
	bool getShowFPS(void);
	short getFiltermode(void);
	bool getFullscreen(void);
	unsigned int getWidth(void);
	unsigned int getHeight(void);
	unsigned short getDepth(void);
	bool isOpenGL(void) { return m_opengl; }
#ifdef USE_OPENGL
	unsigned char getOGLFilter(void) { return (m_opengl_filter==GL_LINEAR); }
#else
	unsigned char getOGLFilter(void) { return 0; }
#endif
	SDL_Surface *getScrollSurface(void);
	SDL_Surface *getBGLayerSurface(void);

	void setFrameskip(unsigned short value);
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
	void setTargetFPS(unsigned int targetfps){ if( targetfps >= 0 && targetfps <= 70 ) m_targetfps = targetfps; }
	unsigned char getTargetFPS(void){ return m_targetfps; }

	void showFPS(bool value);

	void setAspectCorrection(bool value) { m_aspect_correction = value; }
	bool getAspectCorrection(void) { return m_aspect_correction; }

private:
#ifdef USE_OPENGL
	COpenGL	*mp_OpenGL;
#endif

	  unsigned int Width;
	  unsigned int Height;
	  unsigned short Depth;
	  unsigned int Mode;
	  bool Fullscreen;
	  short Filtermode;
	  unsigned short Zoom;
	  unsigned short FrameSkip;
	  unsigned int m_targetfps;	// Used for automatic frame skipping
	  bool showfps;
	  bool m_opengl;
	  int m_opengl_filter;
	  bool m_aspect_correction;

	  SDL_Rect screenrect;
	  SDL_Rect blitrect;

	  SDL_Color MyPalette[256];

	  SDL_Surface *screen;                   // the actual video memory/window
	  SDL_Surface *BGLayerSurface;           // Special surface which support more colors than the scrollsurface
											 //for (Ex. HQ-Images)
	  SDL_Surface *ScrollSurface;       // 512x512 scroll buffer
	  SDL_Surface *FGLayerSurface;       // Scroll buffer for Messages
	  // pointer to the surface that sb_blit is to assemble the scroll buffer into.
	  // if zoom=1 this is the same as "screen", else it's allocated as it's own
	  // buffer of 320x200.
	  SDL_Surface *BlitSurface;
};
#endif /* CVIDEODRIVER_H_ */
