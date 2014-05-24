/*
 * CSoundSlot.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <vector>
#include "CSoundSlot.h"
#include <base/GsLogging.h>
#include "fileio.h"
#include "fileio/ResourceMgmt.h"
#include <base/TypeDefinitions.h>
#include "sdl/audio/base/Sampling.h"
#include "sdl/audio/Audio.h"
#include <base/utils/FindFile.h>


#if defined(OGG)
#define OV_EXCLUDE_STATIC_CALLBACKS // Will reduce some nasty warning, since we don't use those callbacks
#include <vorbisfile.h>
#elif defined(TREMOR)
#include <ivorbisfile.h>
int ov_fopen(char *path,OggVorbis_File *vf);
#endif


CSoundSlot::CSoundSlot() :
m_soundlength(0)
{}

#if !defined(TARGET_OS_IPHONE) || !defined(TARGET_IPHONE_SIMULATOR)
void CSoundSlot::openOGGSound(const std::string& filename, SDL_AudioSpec *pspec, Uint8 *&SoundBuffer, Uint32 &SoundLen)
{

#if defined(OGG) || defined(TREMOR)

    OggVorbis_File  oggStream;     				// stream handle
    const unsigned int BUFFER_SIZE = 32768;     // 32 KB buffers
    SoundBuffer = NULL;

    const SDL_AudioSpec &audioSpec = g_pSound->getAudioSpec();

    if(ov_fopen( (char *)GetFullFileName(filename).c_str(), &oggStream ) == 0)
    {
    	long bytes;
    	char array[BUFFER_SIZE];
    	std::vector<char> buffer;

    	int bitStream;
        vorbis_info*  vorbisInfo = ov_info(&oggStream, -1);
        ov_comment(&oggStream, -1);
        pspec->format = AUDIO_S16LSB; // Ogg Audio seems to always use this format
        pspec->channels = vorbisInfo->channels;

        mOggFreq = vorbisInfo->rate;

        pspec->freq = mOggFreq;


        mHasCommonFreqBase = true;

        // Since I cannot convert with a proper quality from 44100 to 48000 Ogg wave output
        // we set m_AudioFileSpec frequency to the same as the one of the SDL initialized AudioSpec
        // scale just the buffer using readOGGStreamAndResample.
        // This is base problem, but we have workarounds for that...
        if( (pspec->freq%audioSpec.freq != 0) &&
            (audioSpec.freq%pspec->freq != 0) )
        {
            pspec->freq = audioSpec.freq;
            mHasCommonFreqBase = false;
        }


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
    #endif
}
#endif


void CSoundSlot::setupWaveForm( Uint8 *buf, Uint32 len )
{
	m_soundlength = len;
    mSounddata.resize(m_soundlength);
    memcpy(mSounddata.data(), buf, m_soundlength);
}

void CSoundSlot::setupWaveForm( const std::vector<Uint8>& waveform )
{
	m_soundlength = waveform.size();
    mSounddata.resize(m_soundlength);
    memcpy(mSounddata.data(), &waveform[0], m_soundlength);
}

bool CSoundSlot::HQSndLoad(const std::string& gamepath, const std::string& soundname)
{
	SDL_AudioSpec AudioFileSpec;

    const SDL_AudioSpec &audioSpec = g_pSound->getAudioSpec();

	SDL_AudioCVT  Audio_cvt;

	std::string buf;

	Uint32 length = 0;
    Uint8 *oggdata = nullptr;
    Uint8 *wavdata = nullptr;

#if defined(OGG) || defined(TREMOR)
	buf = getResourceFilename("snd/" + soundname + ".OGG", gamepath, false, true); // Start with OGG

	if(buf != "")
	{
        openOGGSound(buf, &AudioFileSpec, oggdata, length);

        if(oggdata == nullptr)
        {
            gLogging.textOut(PURPLE,"Something is wrong with \"%s\"<br>", buf);
			return false;
        }
#else
		gLogging.textOut(PURPLE,"NOTE: OGG-Support is disabled! Get another version or compile it yourself!<br>");
#endif

#if defined(OGG) || defined(TREMOR)
	}
	else
	{
#endif

		buf = getResourceFilename("snd/" + soundname + ".WAV", gamepath, false); // Start with OGG

		if(buf == "")
			return false;

		// Check, if it is a wav file or go back to classic sounds
        if (SDL_LoadWAV (Utf8ToSystemNative(GetFullFileName(buf)).c_str(), &AudioFileSpec, &wavdata, &length) == NULL)
			return false;

#if defined(OGG) || defined(TREMOR)
	}
#endif
	// Build AudioCVT (This is needed for the conversion from one format to the one used in the game)
	const int ret = SDL_BuildAudioCVT(&Audio_cvt,
							AudioFileSpec.format, AudioFileSpec.channels, AudioFileSpec.freq,
                            audioSpec.format, audioSpec.channels, audioSpec.freq);

	// Check that the convert was built
	if(ret == -1)
	{
		gLogging.textOut(PURPLE,"Couldn't convert the sound correctly!<br>");
        SDL_FreeWAV(wavdata);
		return false;
	}

	// Setup for conversion, copy original data to new buffer
	Audio_cvt.buf = (Uint8*) malloc(length * Audio_cvt.len_mult);
	Audio_cvt.len = length;

    if(oggdata)
        memcpy(Audio_cvt.buf, oggdata, length);
    else
        memcpy(Audio_cvt.buf, wavdata, length);

	// We can delete to original WAV data now
    if(oggdata)
        free(oggdata);
    else
        SDL_FreeWAV(wavdata);


	// And now we're ready to convert
	SDL_ConvertAudio(&Audio_cvt);

	// copy the converted stuff to the original soundbuffer
    Uint8 *buffer;
    if( !mHasCommonFreqBase )
	{
        const float factor = float(audioSpec.freq)/mOggFreq;
		length = Audio_cvt.len_cvt;
		const unsigned long out_len = (float)length*factor;        
        buffer = (Uint8*) malloc(out_len);

        resample(buffer, Audio_cvt.buf,
                out_len, length, audioSpec.format, audioSpec.channels);
		length = out_len;
	}
	else
	{
		length = Audio_cvt.len_cvt;
        buffer = (Uint8*) malloc(length);
        memcpy(buffer, Audio_cvt.buf, length);
	}

    setupWaveForm(buffer, length);

    free(buffer);

	// Structure Audio_cvt must be freed!
	free(Audio_cvt.buf);

	return true;
}


void CSoundSlot::unload()
{
    if(!mSounddata.empty())
        mSounddata.clear();
}


