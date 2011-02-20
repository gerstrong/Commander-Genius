/*
 * CMusic.cpp
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#include "sdl/sound/CSound.h"
#include "sdl/sound/Sampling.h"
#include "sdl/sound/IMFPlayer.h"
#include "CMusic.h"
#include "CLogFile.h"
#include "FindFile.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CHuffman.h"
#include "sdl/music/COGGPlayer.h"
#include "sdl/music/CIMFPlayer.h"
#include <fstream>

CMusic::CMusic() :
mp_player(NULL)
{}

bool CMusic::load(const CExeFile& ExeFile, const int level)
{
	m_AudioSpec = g_pSound->getAudioSpec();
	mp_player = new CIMFPlayer(ExeFile, level, m_AudioSpec);

	if(!mp_player->open())
	{
		delete mp_player;
		mp_player = NULL;
		return false;
	}
	return true;
}

bool CMusic::load(const std::string &musicfile)
{
	if(musicfile == "")
		return false;

	m_AudioSpec = g_pSound->getAudioSpec();

	if(m_AudioSpec.format != 0)
	{
		std::string extension = GetFileExtension(musicfile);

		if(extension == "imf")
		{
			mp_player = new CIMFPlayer(musicfile, m_AudioSpec);
		}
		else if(extension == "ogg")
		{
#if defined(OGG) || defined(TREMOR)

			mp_player = new COGGPlayer(musicfile, m_AudioSpec);
#else
			g_pLogFile->ftextOut("Music Manager: Either OGG or TREMOR-Support is disabled! Please use another build<br>");
			return false;
#endif
		}

		if(!mp_player->open())
		{
			g_pLogFile->textOut(PURPLE,"Music Manager: File could not be opened: \"%s\". File is damaged or something is wrong with your soundcard!<br>", musicfile.c_str());
			delete mp_player;
			mp_player = NULL;
			return false;
		}
		return true;

	}
	else
		g_pLogFile->textOut(PURPLE,"Music Manager: I would like to open the music for you. But your Soundcard seems to be disabled!!<br>");
	
	return false;
}

void CMusic::reload()
{
	mp_player->reload();
}

void CMusic::play()
{
	mp_player->play(true);
}

void CMusic::pause()
{
	mp_player->play(false);
}

void CMusic::stop()
{
	if(!mp_player)
		return;

	// wait until the last chunk has been played, and only shutdown then.
	while(m_busy);

	mp_player->close();
}

// length only refers to the part(buffer) that has to be played
void CMusic::readBuffer(Uint8* buffer, size_t length)
{
	if( !mp_player )
		return;

	m_busy = true;

	mp_player->readBuffer(buffer, length);

	m_busy = false;
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

CMusic::~CMusic() {
	stop();
}

