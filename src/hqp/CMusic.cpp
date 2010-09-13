/*
 * CMusic.cpp
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#include "sdl/sound/CSound.h"
#include "CMusic.h"
#include "hqp/hq_sound.h"
#include "CLogFile.h"
#include "FindFile.h"
#include "fileio/ResourceMgmt.h"
#include "sdl/sound/Sampling.h"
#include <fstream>

CMusic::CMusic() :
playmode(PLAY_MODE_STOP),
usedMusicFile(""),
m_open(false)
{
	m_Audio_cvt.buf = NULL;
}

bool CMusic::load(const std::string &musicfile)
{
	if(musicfile == "")
		return false;

	m_AudioSpec = g_pSound->getAudioSpec();
	m_open = false;

	if(m_AudioSpec.format != 0)
	{
		
#if defined(OGG) || defined(TREMOR)

		if(!openOGGStream(musicfile.c_str(), &m_AudioFileSpec, m_oggStream))
		{
			g_pLogFile->textOut(PURPLE,"Music Driver(): OGG file could not be opened: \"%s\". File is damaged or something is wrong with your soundcard!<br>", musicfile.c_str());
			return false;
		}
		
		g_pLogFile->ftextOut("Music Driver(): File \"%s\" opened successfully!<br>", musicfile.c_str());
		usedMusicFile = musicfile;
		m_open = true;
		int ret = SDL_BuildAudioCVT(&m_Audio_cvt,
								m_AudioFileSpec.format, m_AudioFileSpec.channels, m_AudioFileSpec.freq,
								m_AudioSpec.format, m_AudioSpec.channels, m_AudioSpec.freq);
		if(ret == -1)
			return false;

		if(	m_Audio_cvt.buf )
			delete [] m_Audio_cvt.buf;
		const size_t &length = g_pSound->getAudioSpec().size;
		m_Audio_cvt.len = (length*m_Audio_cvt.len_mult)/m_Audio_cvt.len_ratio;
		m_Audio_cvt.buf = new Uint8[m_Audio_cvt.len];

		return true;
#endif
	}
	else
		g_pLogFile->textOut(PURPLE,"Music Driver(): I would like to open the music for you. But your Soundcard is disabled!!<br>");
	
	return false;
}

void CMusic::reload()
{
	stop();
	load(usedMusicFile);
}

void CMusic::play(void)
{
	if(usedMusicFile != "")
		playmode = PLAY_MODE_PLAY;
}

void CMusic::stop(void)
{
#if defined(OGG) || defined(TREMOR)
	if( m_open )
		cleanupOGG(m_oggStream);
#endif
	playmode = PLAY_MODE_STOP;
	m_open = false;
}

void CMusic::readBuffer(Uint8* buffer, size_t length) // length only refers to the part(buffer) that has to be played
{
#if defined(OGG) || defined(TREMOR)
	bool rewind = false;

	// read the ogg stream
	if( m_AudioSpec.freq == 48000 )
	{
		size_t insize = (m_Audio_cvt.len*441)/480;
		size_t mult = m_AudioFileSpec.channels;

		if(m_AudioFileSpec.format == AUDIO_S16)
				mult <<= 1;

		insize /= mult;
		insize++;
		insize *= mult;

		rewind = readOGGStreamAndResample(m_oggStream, (char*)m_Audio_cvt.buf, m_Audio_cvt.len, insize, m_AudioFileSpec);
	}
	else
	{
		rewind = readOGGStream(m_oggStream, (char*)m_Audio_cvt.buf, m_Audio_cvt.len, m_AudioFileSpec);
	}

	// then convert it into SDL Audio buffer
	// Conversion to SDL Format
	SDL_ConvertAudio(&m_Audio_cvt);

	memcpy(buffer, m_Audio_cvt.buf, length);

	if(rewind)
	{
		reload();
		play();
	}
#else
	return;
#endif
}

bool CMusic::LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename)
{
	bool fileloaded = false;
    std::ifstream Tablefile;

    std::string musicpath = getResourceFilename("music/table.cfg", gamepath, false, true);

    if(musicpath != "")
    	fileloaded = OpenGameFileR(Tablefile, musicpath);
	
    if(fileloaded)
    {
    	std::string str_buf;
    	char c_buf[256];
		
    	while(!Tablefile.eof())
    	{
        	Tablefile.get(c_buf, 256, ' ');
    		while(c_buf[0] == '\n') memmove (c_buf, c_buf+1, 254);
        	str_buf = c_buf;
    		if( str_buf == levelfilename )	// found the level! Load the song!
    		{
    			// Get the song filename and load it!
    			Tablefile.get(c_buf, 256);
    			str_buf = c_buf;
    			TrimSpaces(str_buf);
    			std::string filename = getResourceFilename("music/" + str_buf, gamepath, false, true);
    			if( load(filename) )
    				play();
    			Tablefile.close();
    			return true;
    		}
    		Tablefile.get(c_buf, 256);
    		while(!Tablefile.get() == '\n'); // Skip the '\n' delimiters, so next name will be read.
    	}
    	Tablefile.close();
    }
	return false;
}

void CMusic::unload(void)
{
	usedMusicFile = "";
	playmode = PLAY_MODE_STOP;
}


CMusic::~CMusic() {

	if(	m_Audio_cvt.buf )
		delete [] m_Audio_cvt.buf;

	unload();
}

