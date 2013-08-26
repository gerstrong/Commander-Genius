/*
 * COGGPlayer.cpp
 *
 *  Created on: 17.02.2011
 *      Author: Gerstrong
 */

#if defined(OGG) || defined(TREMOR)

#include "COGGPlayer.h"

#include "engine/galaxy/res/CAudioGalaxy.h"
#include "FindFile.h"
#include "sdl/sound/Sampling.h"
#include "fileio/ResourceMgmt.h"
#include "CLogFile.h"


COGGPlayer::COGGPlayer(const SDL_AudioSpec& AudioSpec) :
m_AudioSpec(AudioSpec),
m_pcm_size(0),
m_music_pos(0),
m_reading_stream(false),
m_bitStream(0)
{
    m_Audio_cvt.buf = nullptr;
}


COGGPlayer::COGGPlayer(const std::string& filename, const SDL_AudioSpec& AudioSpec) :
m_filename(filename),
m_AudioSpec(AudioSpec),
m_pcm_size(0),
m_music_pos(0),
m_reading_stream(false),
m_bitStream(0)
{
    m_Audio_cvt.buf = nullptr;
}


COGGPlayer::~COGGPlayer()
{
    if(!m_filename.empty())
	close();
}

#if defined(TREMOR)
int ov_fopen(char *path,OggVorbis_File *vf)
{
	int result;
    FILE *fp = fopen(path, "rb");
	if((result = ov_open(fp, vf, NULL, 0)) < 0)
		fclose(fp);
	return result;
}
#endif

bool COGGPlayer::open()
{
	// If Ogg detected, decode it into the stream psound->sound_buffer.
	// It must fit into the Audio_cvt structure, so that it can be converted
    mHasCommonFreqBase = true;

    if(ov_fopen((char*)GetFullFileName(m_filename).c_str(), &m_oggStream) != 0)
        return false;

    mVorbisInfo = ov_info(&m_oggStream, -1);
    ov_comment(&m_oggStream, -1);

    m_AudioFileSpec.format = AUDIO_S16LSB; // Ogg Audio seems to always use this format

    m_AudioFileSpec.channels = mVorbisInfo->channels;
    m_AudioFileSpec.freq = mVorbisInfo->rate;

    // Since I cannot convert with a proper quality from 44100 to 48000 Ogg wave output
    // we set m_AudioFileSpec frequency to the same as the one of the SDL initialized AudioSpec
    // scale just the buffer using readOGGStreamAndResample.
    // This is base problem, but we have workarounds for that...
    if( (m_AudioFileSpec.freq%m_AudioSpec.freq != 0) &&
        (m_AudioSpec.freq%m_AudioFileSpec.freq != 0) )
    {
        m_AudioFileSpec.freq = m_AudioSpec.freq;
        mHasCommonFreqBase = false;
    }

    m_pcm_size = ov_pcm_total(&m_oggStream,-1);
    m_pcm_size *= (mVorbisInfo->channels*sizeof(Sint16));
    m_music_pos = 0;

	g_pLogFile->ftextOut("OGG-Player: File \"%s\" was opened successfully!<br>", m_filename.c_str());
	int ret = SDL_BuildAudioCVT(&m_Audio_cvt,
			m_AudioFileSpec.format, m_AudioFileSpec.channels, m_AudioFileSpec.freq,
			m_AudioSpec.format, m_AudioSpec.channels, m_AudioSpec.freq);
	if(ret == -1)
		return false;

	const size_t length = m_AudioSpec.size;
    //m_Audio_cvt.len = (length*m_Audio_cvt.len_mult)/m_Audio_cvt.len_ratio;

    m_Audio_cvt.len = (length)/m_Audio_cvt.len_ratio;

    m_Audio_cvt.len = (m_Audio_cvt.len>>2)<<2;

    m_Audio_cvt.buf = new Uint8[m_Audio_cvt.len*m_Audio_cvt.len_mult];

    return true;
}


bool COGGPlayer::loadMusicForLevel(const CExeFile& ExeFile, const int level)
{
	// Now get the proper music slot reading the assignment table.
	Uint16 music_order = 0;
	const int Idx = ExeFile.getEpisode()-4;
	memcpy( &music_order, ExeFile.getRawData()+GalaxySongAssignments[Idx]+level*sizeof(Uint16), sizeof(Uint16));
	
	if(music_order > 20)
	{
	  g_pLogFile->textOut("Sorry, this track is invalid! Please report the developers.");
	  return false;
	}
	
	m_filename = "slot" + itoa(music_order) + ".ogg";
	m_filename = getResourceFilename(JoinPaths("music", m_filename), ExeFile.getDataDirectory(), false, false);

	if(m_filename.empty())	
	   return false;
	    
    return open();
}


bool COGGPlayer::readOGGStream( OggVorbis_File  &oggStream, char *buffer, const size_t &size, const SDL_AudioSpec &OGGAudioSpec )
{
	long bytes = 0;
	unsigned long pos = 0;

	m_reading_stream = true;

	while( pos<size )
	{
		if(m_pcm_size<=0)
			break;
		// Read up to a buffer's worth of decoded sound data
	#if defined(OGG)
		bytes = ov_read(&m_oggStream, buffer+pos, size-pos, 0, 2, 1, &m_bitStream);
	#elif defined(TREMOR)
		bytes = ov_read(&m_oggStream, buffer+pos, size-pos, &m_bitStream);
	#endif
		pos += bytes;
		m_music_pos += bytes;
		if( bytes <= 0 || m_music_pos >= m_pcm_size )
		{
			memset( buffer+pos, OGGAudioSpec.silence, size-pos );
			pos = size;
			m_bitStream = 0;
			m_music_pos = 0;
			m_reading_stream = false;
			return true;
		}
	}
	m_reading_stream = false;
	return false;
}

bool COGGPlayer::readOGGStreamAndResample( OggVorbis_File  &oggStream, Uint8 *buffer, const size_t output_size, const size_t input_size, const SDL_AudioSpec &OGGAudioSpec )
{
	Uint8 buf[input_size];

	bool eof = readOGGStream( oggStream, reinterpret_cast<char*>(buf), input_size, OGGAudioSpec );

    resample( buffer, buf, output_size, input_size, OGGAudioSpec.format, OGGAudioSpec.channels);

	return eof;
}

void COGGPlayer::readBuffer(Uint8* buffer, Uint32 length)
{
	if(!m_playing || !m_Audio_cvt.buf)
		return;

	bool rewind = false;

	// read the ogg stream
    if( !mHasCommonFreqBase )
	{
        Uint64 insize = (m_Audio_cvt.len*mVorbisInfo->rate)/m_AudioSpec.freq;
		Uint8 mult = m_AudioFileSpec.channels;

		if(m_AudioFileSpec.format == AUDIO_S16)
			mult <<= 1;

		insize /= mult;
		insize++;
		insize *= mult;

        memset(m_Audio_cvt.buf, 0, length);

        rewind = readOGGStreamAndResample(m_oggStream,
                                          m_Audio_cvt.buf,
                                          m_Audio_cvt.len_cvt,
                                          insize,
                                          m_AudioFileSpec);
    }
    else
	{
        rewind = readOGGStream(m_oggStream,
                               reinterpret_cast<char*>(m_Audio_cvt.buf),
                               m_Audio_cvt.len,
                               m_AudioFileSpec);
    }

    //memset(m_Audio_cvt.buf, 0, length);

	// then convert it into SDL Audio buffer
	// Conversion to SDL Format
	SDL_ConvertAudio(&m_Audio_cvt);

	memcpy(buffer, m_Audio_cvt.buf, length);

	if(rewind)
	{
		close();
		open();
		play(true);
	}

}

void COGGPlayer::close()
{
 	if(m_Audio_cvt.buf)
		delete [] m_Audio_cvt.buf;
	m_Audio_cvt.buf = NULL;
	
	m_playing = false;
	while(m_reading_stream);
	m_music_pos = 0;
	m_pcm_size = 0;		
	ov_clear(&m_oggStream);
}

#endif
