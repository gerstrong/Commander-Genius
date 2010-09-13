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

short openOGGSound(const std::string& filename, SDL_AudioSpec *pspec, stHQSound *psound);

bool openOGGStream(const std::string& filename, SDL_AudioSpec *pspec, OggVorbis_File  &oggStream);

bool readOGGStream( OggVorbis_File  &oggStream, char *buffer, const size_t &size, const SDL_AudioSpec &OGGAudioSpec );

bool readOGGStreamAndResample( OggVorbis_File  &oggStream, char *buffer, const size_t output_size, const size_t input_size, const SDL_AudioSpec &OGGAudioSpec );

void cleanupOGG(OggVorbis_File  &oggStream);

#if defined(TREMOR)
int ov_fopen(char *path,OggVorbis_File *vf)
{
	int result;
    FILE *fp = fopen(path, "rb");
	if((result = ov_open(fp, vf, NULL, 0)) < 0)
		fclose(fp);
	return result;
}
#endif

#endif
