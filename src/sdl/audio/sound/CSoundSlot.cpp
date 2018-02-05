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


CSoundSlot::CSoundSlot()
{}

#if !(TARGET_OS_IPHONE) || !(TARGET_IPHONE_SIMULATOR)
void CSoundSlot::openOGGSound(const std::string& filename, SDL_AudioSpec *pspec, Uint8 *&SoundBuffer, Uint32 &SoundLen)
{

#if defined(OGG) || defined(TREMOR)

    OggVorbis_File  oggStream;     				// stream handle
    const unsigned int BUFFER_SIZE = 32768;     // 32 KB buffers
    SoundBuffer = nullptr;

    const SDL_AudioSpec &audioSpec = gSound.getAudioSpec();

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

    if(mpWaveChunk)
    {
        Mix_FreeChunk(mpWaveChunk);
    }

    if(!(mpWaveChunk = Mix_QuickLoad_RAW(mSounddata.data(), mSounddata.size())))
    {
        gLogging.ftextOut("Mix_QuickLoad_WAV: %s\n", Mix_GetError());
        // handle error
    }
}

void CSoundSlot::setupWaveForm( const std::vector<Uint8>& waveform )
{
    setupWaveForm( (Uint8*)(&waveform[0]),
                   Uint32(waveform.size()) );
}

bool CSoundSlot::HQSndLoad(const std::string& gamepath,
                           const std::string& soundname)
{
    // load sample.wav in to sample
    std::string buf;

    buf = getResourceFilename("snd/" + soundname + ".OGG", gamepath, false, true); // Start with OGG

    if(buf != "")
    {
        buf = getResourceFilename("snd/" + soundname + ".WAV", gamepath, false); // Start with OGG
    }

    if(buf != "")
    {
        return false;
    }

    if(!(mpWaveChunk=Mix_LoadWAV(buf.c_str())))
    {
        gLogging.ftextOut("Mix_LoadWAV: %s\n", Mix_GetError());
        // handle error
        return false;
    }

    return true;
}

void CSoundSlot::unload()
{
    if(!mSounddata.empty())
    {
        mSounddata.clear();
    }        

    if(mpWaveChunk)
    {
        Mix_FreeChunk(mpWaveChunk);
        mpWaveChunk = nullptr;
    }
}


