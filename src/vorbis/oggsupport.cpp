/*
 * oggsupport.c
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
#include <vector>
using namespace std;

#define BUFFER_SIZE   32768     // 32 KB buffers

#include "../hqp/hq_sound.h"
#include "../sdl/CVideoDriver.h"
#include "../CLogFile.h"

short openOGGSound(FILE *fp, SDL_AudioSpec *pspec, Uint16 format, stHQSound *psound)
{
	// If Ogg detected, decode it into the stream psound->sound_buffer.
	// It must fit into the Audio_cvt structure, so that it can be converted

	int result;
    OggVorbis_File  oggStream;     // stream handle

    if((result = ov_open_callbacks(fp, &oggStream, NULL, 0, OV_CALLBACKS_DEFAULT)) < 0)
    {
        fclose(fp);
        return 1;
    }
    else
    {
    	long bytes;
    	char array[BUFFER_SIZE];
    	vector<char> buffer;
        vorbis_info*    vorbisInfo;    // some formatting data
        vorbis_comment* vorbisComment; // user comments

    	int bitStream;
        vorbisInfo = ov_info(&oggStream, -1);
        vorbisComment = ov_comment(&oggStream, -1);

        pspec->format = AUDIO_S16LSB; // Ogg Audio seems to always use this format
        //pspec->format = (AUDIO_S16LSB*147)/160; // Ogg Audio seems to always use this format

        pspec->channels = vorbisInfo->channels;
        pspec->freq = vorbisInfo->rate;

        psound->sound_len = 0;
        do {
			// Read up to a buffer's worth of decoded sound data
			bytes = ov_read(&oggStream, array, BUFFER_SIZE, 0, 2, 1, &bitStream);
			// Append to end of buffer
			buffer.insert(buffer.end(), array, array + bytes);
        } while (bytes > 0);

        ov_clear(&oggStream);

        psound->sound_len = buffer.size();

        psound->sound_buffer = (Uint8*) malloc( psound->sound_len );
        for(Uint32 i=0; i<psound->sound_len ; i++ )
        {
        	memcpy( &(psound->sound_buffer[i]), &(buffer[i]), 1);
        }

		return 0;
    }
}
#endif
