/*
 * Mixer.cpp
 *
 *  Created on: 31.07.2010
 *      Author: gerstrong
 *
 *  This class is an own mixer.
 *  In the past we have been using SDL_MixAudio
 *  I think that led to many problems,
 */

#include <SDL.h>

/**
 * This will mix 16-bit signed streams together.
 */
void mixAudioSinged16(Uint8 *dst, const Uint8 *src, Uint32 len, Uint8 volume)
{
	len /= 2;
	Sint16 *s_dst = (Sint16*)dst;
	Sint16 *s_src = (Sint16*)src;
	Sint32 chnl_src;
	Sint32 chnl_dst;
	Sint32 outputValue;

    for ( Uint32 i=0 ; i<len ; i++ ) {
    	chnl_src = *s_src;
    	chnl_dst = *s_dst;

        chnl_src *= volume;
        chnl_src /= SDL_MIX_MAXVOLUME;

        outputValue = chnl_src + chnl_dst;  // just add the channels
        if (outputValue > 32766)
        	outputValue = 32766;        	// and clip the result
        if (outputValue < -32767)
        	outputValue = -32767;			// and clip the result

        *s_dst = outputValue;

        s_src++;
        s_dst++;
    }
}

/**
 * This will mix 8-bit unsigned streams together.
 */
void mixAudioUnsinged8(Uint8 *dst, const Uint8 *src, Uint32 len, Uint8 volume)
{
    Sint32 chnl_src;
    Sint32 chnl_dst;
    Sint32 outputValue;           // just add the channels
    for (Uint32 i=0;i<len;i++) {
        chnl_src = *src;
        chnl_dst = *dst;

        chnl_src *= volume;
        chnl_src /= SDL_MIX_MAXVOLUME;

        outputValue = chnl_src + chnl_dst;           // just add the channels
        if (outputValue > 255) outputValue = 255;        // and clip the result

        *dst = outputValue;

        src++;
        dst++;
    }
}

/**
 * Our own Mix function for Audio Streams
 */
void mixAudio(Uint8 *dst, const Uint8 *src, Uint32 len, Uint8 volume, Uint16 format)
{
	if(format == AUDIO_S16)
		mixAudioSinged16(dst, src, len, volume);
	else if(format == AUDIO_U8)
		mixAudioUnsinged8(dst, src, len, volume);
}

