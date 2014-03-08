/*
 *  PerSurfaceAlpha.h
 *  CommanderGenius
 *
 *  Created by Albert Zeyer on 25.07.10.
 *  code under GPL
 *
 */

#ifndef __PERSURFACEALPHA_H__
#define __PERSURFACEALPHA_H__

#include <SDL.h>

// WARNING: Doesn't do any check if we really have persurface alpha on this surface.
// If you want such check, I'd suggest to add another seperate function.
// We mostly use this where it is anyway given that we can do this.
inline Uint8 getPerSurfaceAlpha(SDL_Surface* surf)
{
#if SDL_MAJOR_VERSION == 1 && SDL_MINOR_VERSION <= 2
	return surf->format->alpha;
#else // SDL >=1.3
	Uint8 alpha = 0;
	SDL_GetSurfaceAlphaMod(surf, &alpha);
	return alpha;
#endif
}

#endif
