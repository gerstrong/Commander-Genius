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


#define CHUNKSIZE 4

/**
 * This will mix 16-bit signed streams together.
 */
void mixAudioSigned16(Uint8 *dst, const Uint8 *src, Uint32 len, Uint32 volume)
{
	len /= 2;

    // let's try to unroll loops and get SSE/AVX work better
    const Uint32 chunks = len/CHUNKSIZE;

    Sint16 *s_dst = (Sint16*) (void *)dst;
	Sint16 *s_src = (Sint16*) (void *)src;
    Sint32 chnl_src[CHUNKSIZE];
    Sint32 chnl_dst[CHUNKSIZE];
    Sint32 outputValue[CHUNKSIZE];

    for ( Uint32 j=0 ; j<chunks ; j++ )
    {
        for ( Uint32 i=0 ; i<CHUNKSIZE ; i++ )
        {
            chnl_src[i] = s_src[i];
            chnl_dst[i] = s_dst[i];

            chnl_src[i] *= volume;
            chnl_src[i] /= SDL_MIX_MAXVOLUME;

            // Add the channels and normalize the volume
            outputValue[i] = chnl_src[i] + chnl_dst[i];// just add the channels

            // And clip the result
            if (outputValue[i] > WAVE_SILENCE_U16-1)
                outputValue[i] = WAVE_SILENCE_U16-1;
            else if(outputValue[i] < -WAVE_SILENCE_U16)
                outputValue[i] = -WAVE_SILENCE_U16;

            // And pass result to the destination
            s_dst[i] = outputValue[i];
        }
        s_src += CHUNKSIZE;
        s_dst += CHUNKSIZE;
    }
}

/**
 * This will mix 8-bit unsigned streams together.
 */
void mixAudioUnsigned8(Uint8 *dst, const Uint8 *src, Uint32 len, Uint32 volume)
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

