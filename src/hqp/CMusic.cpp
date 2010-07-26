/*
 * CMusic.cpp
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#include "../sdl/sound/CSound.h"
#include "CMusic.h"
#include "../hqp/hq_sound.h"
#include "../CLogFile.h"
#include "../vorbis/oggsupport.h"
#include "../FindFile.h"
#include "../fileio/ResourceMgmt.h"
#include <fstream>

CMusic::CMusic() :
music_len(0),
music_pos(0),
playmode(PLAY_MODE_STOP),
usedMusicFile("")
{}

bool CMusic::load(const SDL_AudioSpec AudioSpec, const std::string &musicfile)
{
	if(musicfile == "")
		return false;

	if(AudioSpec.format != 0)
	{
		
#ifdef OGG
		SDL_AudioSpec AudioFileSpec;
		SDL_AudioCVT  Audio_cvt;
		
		stHQSound	pOggAudio;
		
		pOggAudio.sound_buffer=NULL;
		pOggAudio.sound_len=0;
		pOggAudio.sound_pos=0;
		
		FILE *fp;
		fp = OpenGameFile(musicfile.c_str(),"rb");
		if(fp == NULL)
		{
			g_pLogFile->textOut(PURPLE,"Music Driver(): \"%s\". File cannot be read!<br>", musicfile.c_str());
			return -1;
		}
		
		if(openOGGSound(fp, &AudioFileSpec, AudioSpec.format, &pOggAudio) != 0)
		{
			g_pLogFile->textOut(PURPLE,"Music Driver(): OGG file could not be opened: \"%s\". File is damaged or something is wrong with your soundcard!<br>", musicfile.c_str());
			return false;
		}
		
		g_pLogFile->ftextOut("Music Driver(): File \"%s\" opened successfully!<br>", musicfile.c_str());
		usedMusicFile = musicfile;
		
		int ret;
		
		ret = SDL_BuildAudioCVT(&Audio_cvt,
								AudioFileSpec.format, AudioFileSpec.channels, AudioFileSpec.freq,
								AudioSpec.format, AudioSpec.channels, AudioSpec.freq);
		
		music_pos = 0;
		
		if(ret == -1){
			g_pLogFile->textOut(PURPLE,"Music Driver(): Couldn't convert the audio for an appropriate format!<br>");
			free(pOggAudio.sound_buffer);
			pOggAudio.sound_len = 0;
			pOggAudio.sound_pos = 0;
			return false;
		}
		
		music_len = pOggAudio.sound_len;
		Audio_cvt.buf = (Uint8*) malloc(music_len * (Audio_cvt.len_mult) * sizeof(Uint8));
		Audio_cvt.len = music_len;
		//Audio_cvt.;
		memcpy(Audio_cvt.buf, pOggAudio.sound_buffer, music_len*sizeof(Uint8));
		
		free(pOggAudio.sound_buffer);
		
		SDL_ConvertAudio(&Audio_cvt);
		
		// Special conversion for 48 kHz
		if(AudioSpec.freq == 48000)
		{
			music_buffer.assign((music_len*(Audio_cvt.len_mult)*48)/44, 0);
			adaptTo48Khz(&music_buffer.at(0), Audio_cvt.buf,
					music_len*(Audio_cvt.len_mult), AudioSpec.format);
		}
		else
		{
			music_len = Audio_cvt.len_cvt;
			music_buffer.assign(music_len, 0);
			memcpy(&music_buffer.at(0), Audio_cvt.buf, music_len);
		}

		// Structure Audio_cvt must be freed!
		free(Audio_cvt.buf);
		
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
	if(!music_buffer.empty())
		playmode = PLAY_MODE_PLAY;
}

void CMusic::stop(void)
{
	playmode = PLAY_MODE_STOP;
}

Uint8 *CMusic::passBuffer(int length) // length only refers to the part(buffer) that has to be played
{
	if(music_buffer.empty())
		return NULL;
	
	if(length < music_len - music_pos)
	{
		Uint8* ptr = &music_buffer.at(0) + music_pos;
		music_pos += length;
		return ptr;
	}
	else
	{
		music_pos = 0;
		return &music_buffer.at(0);
	}
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
	if(!music_buffer.empty())
		music_buffer.clear();
	music_len = 0;
	music_pos = 0;
	playmode = PLAY_MODE_STOP;
}


CMusic::~CMusic() {
	unload();
}

