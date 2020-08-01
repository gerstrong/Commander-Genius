/*
 * Sampling.cpp
 *
 *  Created on: 05.08.2010
 *      Author: gerstrong
 *
 *  This is a special which resamples Sound and Music in case the frequencies are
 *  different and not of the same base.
 */

#include <SDL.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

#define WAVE_SILENCE_U8         128
#define WAVE_SILENCE_S16        0

/**
 * \brief This function splits the stereo for one channel so it can be manipulated
 * \param output 	Pointer to output buffer
 * \param input 	Pointer to input buffer
 * \param length 	Length of the stream in bytes,
 * \param channels	Amount of Sound Channels used in the Stream
 * \param chnl		Number of channel to split off
 * \param width		width of the amplitude. In case of U8 it is 1, for 16 bit 2
 */
void splitChannel(Uint8* output, Uint8* input, size_t length,
					size_t channels, size_t chnl, size_t width)
{
	input += (width*chnl);
	for( size_t i=0 ; i<(length/(channels*width)) ; i++ )
	{
		memcpy(output, input, width);
		input += (width*channels);
		output += width;
	}
}

/**
 * \brief This function merges a channel into the stereo stream
 * \param output 	Pointer to output buffer
 * \param input 	Pointer to input buffer
 * \param length 	Length of the stream in bytes,
 * \param channels	Amount of Sound Channels used in the Strean
 * \param chnl		Number of channel to merge
 * \param width		width of the amplitude. In case of U8 it is 1, for 16 bit 2
 */
void mergeChannel(Uint8* output, Uint8* input, size_t length,
					size_t channels, size_t chnl, size_t width)
{
	output += (width*chnl);
	for( size_t i=0 ; i<(length/(channels*width)) ; i++ )
	{
		memcpy(output, input, width);
		input += width;
		output += (width*channels);
	}
}

/**
 * \brief This will stretch a sound stream which is wide depending on the size of the used template
 * \param strechedbuf The output buffer which will be stretched
 * \param
 *  Sint16 *buf,
					size_t input_len, size_t input_len
 */
template <typename T>
void stretchSound( T *output, T *input,	size_t output_len, size_t input_len )
{
	const size_t len_a = input_len/sizeof(T);
	const size_t len_b = output_len/sizeof(T);
	const float factor = ((float) input_len)/((float) output_len);
	Sint32 v_a, v_b, value;
	size_t inter;

	for(size_t i=0, j=0 ; i<len_a && j<len_b ; i++)
	{
		output[j] = input[i];
		j++;

		inter = ((float)j)*factor;
		// linear interpolation for the gaps we get ;-)
		while( i >= inter && i<len_a && j<len_b )
		{
			v_a = input[i];
			v_b = ((i+1)<len_a) ? input[i+1] : input[i];
			value = (v_a + v_b)/2;
			output[j] = value;
			j++;
			inter = ((float)j)*factor;
		}
	}
}

/**
 * \brief Adaptation function to the 48 Khz Sound in 16 bit mode
 * \param out 			pointer to the output where the sound will written to
 * \param in		 	pointer to the adress where the sound comes from
 * \param output_len	length of the output buffer
 * \param input_len		length of the input buffer
 * \param channels		number of channels. 1 in mono, 2 in stereo
 */

template <typename T>
void resampleAnyBits(const T* out, const T *in,
		unsigned long output_len, unsigned long input_len, size_t channels)
{
	Uint8* output_buffer = (Uint8*) out;
	Uint8 *input_buffer = (Uint8*) in;
	// Temporary buffer for the stretched stream. Of course it's per channel...
	T *stretchedbuf = (T*) malloc( output_len/channels );

	// Split the channels so they won't be overlapped during the stretching
	Uint8 *onechannelbuffer = (Uint8*) malloc( input_len/channels );

	// Now, for all channels do the operation
	for(size_t ch=0 ; ch<channels ; ch++)
	{
		splitChannel( onechannelbuffer, input_buffer, input_len, channels, ch, sizeof(T) );
		stretchSound( stretchedbuf, (T*) (void *)onechannelbuffer, output_len/channels, input_len/channels );
		mergeChannel( output_buffer, (Uint8*) stretchedbuf, output_len, channels, ch, sizeof(T) );
	}
	free(onechannelbuffer);
	free(stretchedbuf);
}



// this fixes the speed of the played songs, when using any frequency
void resample(const Uint8* output_buffer, const Uint8 *input_buffer,
		const unsigned long output_len, const unsigned long input_len,
		const Uint16 format, const size_t channels )
{
	if(format == AUDIO_S16)
	{
		memset((Uint8*)output_buffer, WAVE_SILENCE_S16, output_len);

        resampleAnyBits( (Sint16*) (void *)output_buffer, (Sint16*) (void *)input_buffer,
                output_len, input_len, channels);
	}
	else if(format == AUDIO_U8)
	{
		memset((Uint8*)output_buffer, WAVE_SILENCE_U8, output_len);
		resampleAnyBits( (Uint8*) output_buffer, (Uint8*) input_buffer,
				output_len, input_len, channels);
	}
}
