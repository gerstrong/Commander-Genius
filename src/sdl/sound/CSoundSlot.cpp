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
#include "CLogFile.h"
#include "fileio.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/TypeDefinitions.h"
#include "sdl/sound/Sampling.h"
#include "sdl/music/COGGPlayer.h"
#include "FindFile.h"

#ifdef OGG
#include <vorbisfile.h>
#elif defined  TREMOR
#include <ivorbisfile.h>
#endif


CSoundSlot::CSoundSlot() :
m_sounddata(NULL),
m_soundlength(0),
m_pAudioSpec(NULL)
{}

void CSoundSlot::openOGGSound(const std::string& filename, SDL_AudioSpec *pspec, Uint8 *&SoundBuffer, Uint32 &SoundLen)
{
    OggVorbis_File  oggStream;     				// stream handle
    const unsigned int BUFFER_SIZE = 32768;     // 32 KB buffers
    SoundBuffer = NULL;

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
}


void CSoundSlot::setupWaveForm( Uint8 *buf, Uint32 len )
{
	m_soundlength = len;
	m_sounddata = new Uint8[m_soundlength];
	memcpy(m_sounddata, buf, m_soundlength);
}

void CSoundSlot::setupWaveForm( const std::vector<Uint8>& waveform )
{
	m_soundlength = waveform.size();
	m_sounddata = new Uint8[m_soundlength];
	memcpy(m_sounddata, &waveform[0], m_soundlength);
}

bool CSoundSlot::HQSndLoad(const std::string& gamepath, const std::string& soundname)
{
	SDL_AudioSpec AudioFileSpec;
	SDL_AudioSpec &AudioSpec = *m_pAudioSpec;
	SDL_AudioCVT  Audio_cvt;

	std::string buf;
	m_sounddata = NULL;

	Uint32 length = 0;
	Uint8 *snddata = NULL;

#if defined(OGG) || defined(TREMOR)
	buf = getResourceFilename("snd/" + soundname + ".OGG", gamepath, false, true); // Start with OGG

	if(buf != "")
	{
		openOGGSound(buf, &AudioFileSpec, snddata, length);

		if(snddata == NULL)
			return false;
#else
		g_pLogFile->textOut(PURPLE,"NOTE: OGG-Support is disabled! Get another version or compile it yourself!<br>");
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
		if (SDL_LoadWAV (Utf8ToSystemNative(GetFullFileName(buf)).c_str(), &AudioFileSpec, &snddata, &length) == NULL)
			return false;

#if defined(OGG) || defined(TREMOR)
	}
#endif
	// Build AudioCVT (This is needed for the conversion from one format to the one used in the game)
	const int ret = SDL_BuildAudioCVT(&Audio_cvt,
							AudioFileSpec.format, AudioFileSpec.channels, AudioFileSpec.freq,
							AudioSpec.format, AudioSpec.channels, AudioSpec.freq);

	// Check that the convert was built
	if(ret == -1)
	{
		g_pLogFile->textOut(PURPLE,"Couldn't convert the sound correctly!<br>");
		SDL_FreeWAV(snddata);
		return false;
	}

	// Setup for conversion, copy original data to new buffer
	Audio_cvt.buf = (Uint8*) malloc(length * Audio_cvt.len_mult);
	Audio_cvt.len = length;
	memcpy(Audio_cvt.buf, snddata, length);

	// We can delete to original WAV data now
	SDL_FreeWAV(snddata);

	// And now we're ready to convert
	SDL_ConvertAudio(&Audio_cvt);

	// copy the converted stuff to the original soundbuffer
	if(AudioSpec.freq == 48000)
	{
		const float factor = float(AudioSpec.freq)/44100.0f;
		length = Audio_cvt.len_cvt;
		const unsigned long out_len = (float)length*factor;
		snddata = (Uint8*) malloc(out_len);

		resample(snddata, Audio_cvt.buf,
				out_len, length, AudioSpec.format, AudioSpec.channels);
		length = out_len;
	}
	else
	{
		length = Audio_cvt.len_cvt;
		snddata = (Uint8*) malloc(length);
		memcpy(snddata, Audio_cvt.buf, length);
	}

	setupWaveForm(snddata, length);

	free(snddata);

	// Structure Audio_cvt must be freed!
	free(Audio_cvt.buf);

	return true;
}


void CSoundSlot::unload()
{
	if(m_sounddata){ delete[] m_sounddata; }
	m_sounddata = NULL;
}

CSoundSlot::~CSoundSlot() {
	unload();
}


