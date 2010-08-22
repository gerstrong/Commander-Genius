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
usedMusicFile("")
{}

bool CMusic::load(const SDL_AudioSpec AudioSpec, const std::string &musicfile)
{
	if(musicfile == "")
		return false;

	m_AudioSpec = AudioSpec;

	if(m_AudioSpec.format != 0)
	{
		
#ifdef OGG
		
		FILE *fp;
		fp = OpenGameFile(musicfile.c_str(),"rb");
		if(fp == NULL)
		{
			g_pLogFile->textOut(PURPLE,"Music Driver(): \"%s\". File cannot be read!<br>", musicfile.c_str());
			return -1;
		}
		
		if(!openOGGStream(fp, &m_AudioFileSpec, m_oggStream))
		{
			g_pLogFile->textOut(PURPLE,"Music Driver(): OGG file could not be opened: \"%s\". File is damaged or something is wrong with your soundcard!<br>", musicfile.c_str());
			return false;
		}
		
		g_pLogFile->ftextOut("Music Driver(): File \"%s\" opened successfully!<br>", musicfile.c_str());
		usedMusicFile = musicfile;

		// Special conversion for 48 kHz
		/*if(AudioSpec.freq == 48000)
		{
			const float factor = ((float)AudioSpec.freq)/(44100.0f);
			const unsigned long in_len = music_len*Audio_cvt.len_mult;
			const unsigned long out_len = (float)in_len*factor;

			music_buffer.assign((float)(music_len*Audio_cvt.len_mult)*factor, 0);

			resample(&music_buffer.at(0), Audio_cvt.buf,
					out_len, in_len, AudioSpec.format, AudioSpec.channels);
			music_len = out_len;
		}
		else
		{
			music_len = Audio_cvt.len_cvt;
			music_buffer.assign(music_len, 0);
			memcpy(&music_buffer.at(0), Audio_cvt.buf, music_len);
		}

		// Structure Audio_cvt must be freed!
		free(Audio_cvt.buf);*/

		return true;
		
#endif
	}
	else
		g_pLogFile->textOut(PURPLE,"Music Driver(): I would like to open the music for you. But your Soundcard is disabled!!<br>");
	
	return false;
}

void CMusic::reload(const SDL_AudioSpec AudioSpec)
{
	stop();
	load(AudioSpec, usedMusicFile);
}

void CMusic::play(void)
{
	if(usedMusicFile != "")
		playmode = PLAY_MODE_PLAY;
}

void CMusic::stop(void)
{
	playmode = PLAY_MODE_STOP;
}

void CMusic::readBuffer(Uint8* buffer, size_t length) // length only refers to the part(buffer) that has to be played
{
	// Prepare for conversion
	SDL_AudioCVT  Audio_cvt;
	int ret = SDL_BuildAudioCVT(&Audio_cvt,
							m_AudioFileSpec.format, m_AudioFileSpec.channels, m_AudioFileSpec.freq,
							m_AudioSpec.format, m_AudioSpec.channels, m_AudioSpec.freq);
	if(ret == -1)
		return;

	//Audio_cvt.buf = buffer;
	Audio_cvt.len = length/Audio_cvt.len_mult;
	Audio_cvt.buf = new Uint8[length];

	char buf[length];
	// read the ogg stream
	//readOGGStream(m_oggStream, (char*)Audio_cvt.buf, Audio_cvt.len);
	readOGGStream(m_oggStream, buf, Audio_cvt.len);

	Audio_cvt.buf = new Uint8[length];
	memcpy( Audio_cvt.buf, buf, length );

	// then convert it into SDL Audio buffer
	// Conversion to SDL Format
	SDL_ConvertAudio(&Audio_cvt);

	memcpy( buffer, Audio_cvt.buf, length );

	delete Audio_cvt.buf;
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
    			if( load(g_pSound->getAudioSpec(), filename) )
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
	unload();
}

