/*
 * oggsupport.h
 *
 *  Created on: 06.01.2009
 *      Author: gerstrong
 */

#ifdef OGG

#include <SDL.h>
// vorbis headers
#include <codec.h>
#include <vorbisfile.h>

#include <cstdio>
#include <iostream>

#define BUFFER_SIZE   32768     // 32 KB buffers

#include "hqp/hq_sound.h"
#include "sdl/CVideoDriver.h"
#include "CLogFile.h"

short openOGGSound(FILE *fp, SDL_AudioSpec *pspec, stHQSound *psound);

bool openOGGStream(FILE *fp, SDL_AudioSpec *pspec, OggVorbis_File  &oggStream);

void readOGGStream( OggVorbis_File  &oggStream, char *buffer, size_t size );

void cleanupOGG(OggVorbis_File  &oggStream);

#endif
