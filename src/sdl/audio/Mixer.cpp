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

#define WAVE_SILENCE_U8         128
#define WAVE_SILENCE_S8         0

#define WAVE_SILENCE_U16        32768
#define WAVE_SILENCE_S16        0

/**
 * This will mix 16-bit signed streams together.
 */
void mixAudioSinged16(Uint8 *dst, const Uint8 *src, Uint32 len, Uint8 volume)
{
	len /= 2;
	Sint16 *s_dst = (Sint16*) (void *)dst;
	Sint16 *s_src = (Sint16*) (void *)src;
	Sint32 chnl_src=0;
	Sint32 chnl_dst=0;
	Sint32 outputValue=0;

    for ( Uint32 i=0 ; i<len ; i++ ) 
    {
    	chnl_src = *s_src;
    	chnl_dst = *s_dst;

        chnl_src *= volume;
        chnl_src /= SDL_MIX_MAXVOLUME;

        outputValue = chnl_src + chnl_dst;  // just add the channels
        if (outputValue > 32767)
        	outputValue = 32767;        // and clip the result
        if (outputValue < -32768)
        	outputValue = -32768;	// and clip the result

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
    Sint32 chnl_src=0;
    Sint32 chnl_dst=0;
    Sint32 outputValue=0;           // just add the channels
    
    for (Uint32 i=0;i<len;i++) 
    {
        chnl_src = *src;
        chnl_dst = *dst;

        chnl_src -= WAVE_SILENCE_U8;
        chnl_dst -= WAVE_SILENCE_U8;

        chnl_src *= volume;
        chnl_src /= SDL_MIX_MAXVOLUME;

        outputValue = chnl_src + chnl_dst + WAVE_SILENCE_U8;           // just add the channels
        if (outputValue > 255) outputValue = 255;        // and clip the result
        if (outputValue < 0) outputValue = 0;

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

