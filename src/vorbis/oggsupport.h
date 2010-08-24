/*
 * oggsupport.h
 *
 *  Created on: 06.01.2009
 *      Author: gerstrong
 */


#include <SDL.h>
// vorbis headers

#include <cstdio>
#include <iostream>

#define BUFFER_SIZE   32768     // 32 KB buffers

#ifdef OGG
#include <vorbisfile.h>
#elif defined  TREMOR
#include <ivorbisfile.h>
#endif

#if defined(OGG) || defined(TREMOR)

#include "hqp/hq_sound.h"
#include "sdl/CVideoDriver.h"
#include "CLogFile.h"

short openOGGSound(FILE *fp, SDL_AudioSpec *pspec, stHQSound *psound);

bool openOGGStream(FILE *fp, SDL_AudioSpec *pspec, OggVorbis_File  &oggStream);

bool readOGGStream( OggVorbis_File  &oggStream, char *buffer, size_t output_size, size_t input_size, const SDL_AudioSpec &OGGAudioSpec );

void cleanupOGG(OggVorbis_File  &oggStream);

#endif
