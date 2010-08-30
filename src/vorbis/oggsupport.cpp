/*
 * oggsupport.c
 *
 *  Created on: 06.01.2009
 *      Author: gerstrong
 */

#include "oggsupport.h"
#include "sdl/sound/Sampling.h"
#include <vector>

#if defined(OGG) || defined(TREMOR)

short openOGGSound(FILE *fp, SDL_AudioSpec *pspec, stHQSound *psound)
{
	// If Ogg detected, decode it into the stream psound->sound_buffer.
	// It must fit into the Audio_cvt structure, so that it can be converted

	int result;
    OggVorbis_File  oggStream;     // stream handle

#if defined(OGG)
	if((result = ov_open_callbacks(fp, &oggStream, NULL, 0, OV_CALLBACKS_DEFAULT)) < 0)
#elif defined(TREMOR)
	if((result = ov_open(fp, &oggStream, NULL, 0)) < 0)
#endif
    {
        fclose(fp);
        return 1;
    }
    else
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

        psound->sound_len = 0;
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

        psound->sound_len = buffer.size();

        psound->sound_buffer = (Uint8*) malloc( psound->sound_len );
        for(Uint32 i=0; i<psound->sound_len ; i++ )
        {
        	memcpy( &(psound->sound_buffer[i]), &(buffer[i]), 1);
        }

		return 0;
    }
}

long pcm_size;
long music_pos;

bool openOGGStream(FILE *fp, SDL_AudioSpec *pspec, OggVorbis_File  &oggStream)
{
	// If Ogg detected, decode it into the stream psound->sound_buffer.
	// It must fit into the Audio_cvt structure, so that it can be converted

	int result;
#if defined(OGG)
	if((result = ov_open_callbacks(fp, &oggStream, NULL, 0, OV_CALLBACKS_DEFAULT)) < 0)
#elif defined(TREMOR)
	if((result = ov_open(fp, &oggStream, NULL, 0)) < 0)
#endif
    {
        fclose(fp);
        return false;
    }
    else
    {
        vorbis_info*    vorbisInfo;    // some formatting data
        vorbis_comment* vorbisComment; // user comments

        vorbisInfo = ov_info(&oggStream, -1);
        vorbisComment = ov_comment(&oggStream, -1);

        pspec->format = AUDIO_S16LSB; // Ogg Audio seems to always use this format

        pspec->channels = vorbisInfo->channels;
        pspec->freq = vorbisInfo->rate;

        pcm_size = ov_pcm_total(&oggStream,-1);
        pcm_size *= (vorbisInfo->channels*2);
        music_pos = 0;

		return true;
    }
}

bool readOGGStream( OggVorbis_File  &oggStream, char *buffer, const size_t &size, const SDL_AudioSpec &OGGAudioSpec )
{
	long bytes = 0;
	unsigned long pos = 0;
	int bitStream = 0;

	while( pos<size )
	{
		// Read up to a buffer's worth of decoded sound data
	#if defined(OGG)
		bytes = ov_read(&oggStream, buffer+pos, size-pos, 0, 2, 1, &bitStream);
	#elif defined(TREMOR)
		bytes = ov_read(&oggStream, buffer+pos, size-pos, &bitStream);
	#endif
		pos += bytes;
		music_pos += bytes;
		if( bytes <= 0 || music_pos >= pcm_size )
		{
			memset( buffer+pos, OGGAudioSpec.silence, size-pos );
			pos = size;
			bitStream = 0;
			music_pos = 0;
			return true;
		}
	}
	return false;
}

bool readOGGStreamAndResample( OggVorbis_File  &oggStream, char *buffer, const size_t output_size, const size_t input_size, const SDL_AudioSpec &OGGAudioSpec )
{
	long bytes = 0;
	char buf[input_size];

	bool eof = readOGGStream( oggStream, buf, input_size, OGGAudioSpec );

	resample((Uint8*)buffer, (Uint8*)buf, output_size, input_size, OGGAudioSpec.format, OGGAudioSpec.channels);

	return eof;
}


void cleanupOGG(OggVorbis_File  &oggStream)
{
	music_pos = 0;
	pcm_size = 0;
	ov_clear(&oggStream);
}

#endif
