/*
 * CVideoEngine.h
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 *
 *  This is a base class which will process the video settings on your system.
 *  CVideoDriver calls that Engine through an overloaded Extra class like COpenGL or CSDLVideo
 */

#ifndef CVIDEOENGINE_H_
#define CVIDEOENGINE_H_

#include <SDL.h>
#include <string>

#include "scalers/CScaler.h"
#include "sdl/CVidConfig.h"


class CVideoEngine
{
public:

	CVideoEngine(const CVidConfig& VidConfig);
	~CVideoEngine();

	virtual bool init();

	virtual bool resizeDisplayScreen(const CRect<Uint16>& newDim) = 0;
	void aspectCorrectResizing(const CRect<Uint16>& newDim);

	virtual void updateScreen() = 0;
	virtual void shutdown();

	SDL_Surface *createSurface( std::string name, bool alpha, int width, int height, int bpp, int mode, SDL_PixelFormat* format );
	virtual bool createSurfaces() = 0;
	void fetchStartScreenPixelPtrs(Uint8 *&ScreenPtr, Uint8 *&BlitPtr, unsigned int &width, unsigned int &height);
	virtual void collectSurfaces() = 0;
	virtual void clearSurfaces() = 0;
	void blitScrollSurface();
	void stop();

	SDL_Surface *getBlitSurface() { return BlitSurface; }
	SDL_Surface *getScreenSurface() { return screen; }
	SDL_Surface *getScrollSurface() { return ScrollSurface; }
	SDL_Surface *getFXSurface() { return FXSurface; }
	CRect<Uint16> &getAspectCorrRect() { return aspectCorrectionRect; }

	void resetScrollbuffer()
	{
		mSbufferx = 0;
		mSbuffery = 0;
	}

	inline void UpdateScrollBufX(const Sint16 SBufferX)
	{		mSbufferx = SBufferX&511;	}

	inline void UpdateScrollBufY(const Sint16 SBufferY)
	{		mSbuffery = SBufferY&511;	}

	void readScrollBuffer(Sint16 &x, Sint16 &y)
	{	x = mSbufferx; y = mSbuffery;}
    
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_GLContext glcontext;
#endif

protected:

	SDL_Surface *BlitSurface;
	SDL_Surface *FilteredSurface;
	SDL_Surface *ScrollSurface;       	// 512x512 scroll buffer
	SDL_Surface *FXSurface;

	CScaler Scaler;

	const CVidConfig &m_VidConfig;

	Sint16 mSbufferx;
	Sint16 mSbuffery;

	SDL_Surface *screen;                // the actual video memory/window

	// Those variables are used for the rendering process, so they don't need to be recalculated
	unsigned m_dst_slice, m_src_slice;

	unsigned int m_Mode;

	// A sub-rectangle where an aspect-corrected frame is displayed
	CRect<Uint16> aspectCorrectionRect;
};

#endif /* CVIDEOENGINE_H_ */
