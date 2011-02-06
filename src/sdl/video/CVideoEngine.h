/*
 * CVideoEngine.h
 *
 *  Created on: 05.02.2011
 *      Author: gerhard
 *
 *  This is a base class which will process the video settings on your system.
 *  CVideoDriver calls that Engine through an overloaded Extra class like COpenGL or CSDLVideo
 */

#ifndef CVIDEOENGINE_H_
#define CVIDEOENGINE_H_

#include <SDL.h>

#include "sdl/CVidConfig.h"

class CVideoEngine {
public:

	CVideoEngine(const CVidConfig& VidConfig);

	virtual void init() = 0;
	virtual void updateScreen() = 0;
	virtual void shutdown() = 0;

	SDL_Surface *getBlitSurface() { return BlitSurface; }

protected:

	void scale2xnofilter(char *dest, char *src, short bbp);
	void scale3xnofilter(char *dest, char *src, short bbp);
	void scale4xnofilter(char *dest, char *src, short bbp);

	SDL_Surface *BlitSurface;
	SDL_Surface *FGLayerSurface;       	// Scroll buffer for Messages
	SDL_Surface *ScrollSurface;       	// 512x512 scroll buffer
	SDL_Surface *FXSurface;

	bool m_blitsurface_alloc;

	CVidConfig &m_VidConfig;
};

#endif /* CVIDEOENGINE_H_ */
