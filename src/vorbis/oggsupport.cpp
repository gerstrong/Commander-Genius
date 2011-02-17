/*
 * oggsupport.c
 *
 *  Created on: 06.01.2009
 *      Author: gerstrong
 */

#include "oggsupport.h"
#include "FindFile.h"
#include <vector>

#ifdef OGG
#include <vorbisfile.h>
#elif defined  TREMOR
#include <ivorbisfile.h>
#endif

Uint8 *openOGGSound(const std::string& filename, SDL_AudioSpec *pspec, Uint32 &SoundLen)
{
    OggVorbis_File  oggStream;     				// stream handle
    const unsigned int BUFFER_SIZE = 32768;     // 32 KB buffers
    Uint8 *SoundBuffer = NULL;

    if(ov_fopen( (char *)GetFullFileName(filename).c_str(), &oggStream ) == 0)
    {
    	long bytes;
    	char array[BUFFER_SIZE];
    	std::vector<char> buffer;
        vorbis_info*    vorbisInfo;    // some formatting data
        vorbis_comment* vorbisComment; // user comments

    	int bitStream;
        vorbisInfo = ov_info(&oggStream, -1);
        vorbisComment = ov_comment(&oggStream, -1);
        pspec->format = AUDIO_S16LSB; // Ogg Audio seems to always use this format
        pspec->channels = vorbisInfo->channels;
        pspec->freq = vorbisInfo->rate;
        SoundLen = 0;

        do {
			// Read up to a buffer's worth of decoded sound data
#if defined(OGG)
        	bytes = ov_read(&oggStream, array, BUFFER_SIZE, 0, 2, 1, &bitStream);
#elif defined(TREMOR)
        	bytes = ov_read(&oggStream, array, BUFFER_SIZE, &bitStream);
#endif
			// Append to end of buffer
			buffer.insert(buffer.end(), array, array + bytes);
        } while (bytes > 0);

        ov_clear(&oggStream);

        SoundLen = buffer.size();
        SoundBuffer = (Uint8*) malloc(SoundLen*sizeof(Uint8));
        memcpy(SoundBuffer, &(buffer[0]), SoundLen);
    }
    return SoundBuffer;
}

