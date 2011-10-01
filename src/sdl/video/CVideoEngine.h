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

#include "scalers/scalebit.h"
#include "sdl/CVidConfig.h"

#define restrict

class CVideoEngine {
public:

	CVideoEngine(const CVidConfig& VidConfig, Sint16 *&p_sbufferx, Sint16 *&p_sbuffery);

	virtual bool init();
	bool resizeDisplayScreen(const CRect& newDim);
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
	SDL_Surface *getFGLayerSurface() { return FGLayerSurface; }
	SDL_Surface *getScrollSurface() { return ScrollSurface; }
	SDL_Surface *getFXSurface() { return FXSurface; }

protected:

	void scale2xnofilter(char* restrict dest, char* restrict src, short bbp);
	void scale3xnofilter(char* restrict dest, char* restrict src, short bbp);
	void scale4xnofilter(char* restrict dest, char* restrict src, short bbp);

	SDL_Surface *BlitSurface;
	SDL_Surface *FilteredSurface;
	SDL_Surface *FGLayerSurface;       	// Scroll buffer for Messages
	SDL_Surface *ScrollSurface;       	// 512x512 scroll buffer
	SDL_Surface *FXSurface;

	//bool m_blitsurface_alloc;

	const CVidConfig &m_VidConfig;

	Sint16 *&mp_sbufferx;
	Sint16 *&mp_sbuffery;

	SDL_Surface *screen;                // the actual video memory/window

	// Those variables are used for the rendering process, so they don't need to be recalculated
	unsigned m_dst_slice, m_src_slice;

	unsigned int m_Mode;
};

#endif /* CVIDEOENGINE_H_ */
