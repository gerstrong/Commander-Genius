/*
 * oggsupport.h
 *
 *  Created on: 06.01.2009
 *      Author: gerstrong
 */

#ifndef __OGGSUPPORT__
#define __OGGSUPPORT__

#include <SDL.h>
#include <string>
// vorbis headers

#if defined(OGG) || defined(TREMOR)

Uint8 *openOGGSound(const std::string& filename, SDL_AudioSpec *pspec, Uint32 &SoundLen);

#endif

#endif //__OGGSUPPORT__
