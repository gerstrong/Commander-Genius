/*
 * CMusic.cpp
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#include "sdl/audio/Audio.h"
#include "sdl/audio/base/Sampling.h"
#include "sdl/audio/music/CIMFPlayer.h"
#include "CMusic.h"
#include <base/GsLogging.h>
#include <base/utils/FindFile.h>
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CHuffman.h"
#include "fileio/KeenFiles.h"
#include <fstream>
#include <limits>


bool CMusic::loadTrack(const int track)
{
    gLogging.textOut("Load track number " + itoa(track) + "");

    if(track < 0)
    {
        gLogging.textOut("The requested Track-Number does not make sense. Music will not be loaded here!");
        return false;
    }

    gSound.pauseAudio();


    if(loadIMFTrack(track))
    {
        gSound.resumeAudio();
        return true;
    }

    gSound.resumeAudio();
    return false;
}


bool CMusic::load(const std::string &musicfile)
{        

    Mix_HaltMusic();

    if(mpMixMusic)
    {
        Mix_FreeMusic(mpMixMusic);
        mpMixMusic = nullptr;
    }

    unhookAll();

    if(musicfile == "") return false;

    const SDL_AudioSpec &audioSpec = gSound.getAudioSpec();

	if(audioSpec.format != 0)
	{
		std::string extension = GetFileExtension(musicfile);

        stop();

		stringlwr(extension);

		if( extension == "imf" )
		{

            if(!loadIMFFile(musicfile))
            {
                return false;
            }

		}
        else if( extension == "ogg" )
		{

            const auto fullFname = GetFullFileName(musicfile);
            mpMixMusic = Mix_LoadMUS(fullFname.c_str());
            if(!mpMixMusic)
            {
                gLogging.ftextOut("Mix_LoadMUS(\"%s\"): %s\n",
                                  fullFname.c_str(),
                                  Mix_GetError());
                return false;
            }

        }


		return true;

	}
	else
	{
		gLogging.textOut(FONTCOLORS::PURPLE,"Music Manager: I would like to open the music for you. But your Soundcard seems to be disabled!!<br>");
	}

    gLogging.textOut(FONTCOLORS::PURPLE,"Music Manager: Got a tune to play");

	return false;
}

void CMusic::reload()
{
    gSound.pauseAudio();

    play();

    Mix_RewindMusic();

    gSound.resumeAudio();
}

void CMusic::play()
{
    Mix_HaltMusic();

    if(!mpMixMusic)
    {
        return;
    }

    // Play music forever
    if(Mix_PlayMusic(mpMixMusic, -1) == -1)
    {
        gLogging.ftextOut("Mix_PlayMusic: %s\n", Mix_GetError());
        // well, there's no music, but most games don't break without music...
    }

    Mix_ResumeMusic();
}

void CMusic::pause()
{
    Mix_PauseMusic();
}

void CMusic::stop()
{
    Mix_HaltMusic();

    if(mpMixMusic)
    {
        Mix_FreeMusic(mpMixMusic);
        mpMixMusic = nullptr;
    }

    unhookAll();
}


bool CMusic::LoadfromSonglist(const std::string &gamepath, const int &level)
{
    bool fileloaded = false;
    std::ifstream Tablefile;

    std::string musicpath = getResourceFilename("songlist.lst", gamepath, false, false);

    if(musicpath != "")
    	fileloaded = OpenGameFileR(Tablefile, musicpath);

    if(fileloaded)
    {
    	std::string str_buf;
    	std::string music_filename;
    	int detected_level=-1;
    	size_t next_pos = 0;

    	while(!Tablefile.eof())
    	{
    		getline(Tablefile, str_buf);

        	next_pos = str_buf.find(' ')+1;
        	str_buf = str_buf.substr(next_pos);
        	next_pos = str_buf.find(' ');

        	// Get level number            
            detected_level = atoi(str_buf.substr(0, next_pos).c_str());

        	str_buf = str_buf.substr(next_pos);
        	next_pos = str_buf.find('"')+1;
        	str_buf = str_buf.substr(next_pos);
        	next_pos = str_buf.find('"');

        	// Get the music filename to be read
        	music_filename = str_buf.substr(0, next_pos);

    		if( detected_level == level )	// found the level! Load the song!
    		{
    			// Get the song filename and load it!
    			std::string filename = getResourceFilename( music_filename, gamepath, false, true);
    			if( load(filename) )
                {
    				play();
                }

    			return true;
    		}
    	}
    }
	return false;
}

bool CMusic::LoadfromMusicTable(const std::string &gamepath,
                                const std::string &levelfilename)
{
    bool fileloaded = false;
    std::ifstream Tablefile;

    std::string musicpath = getResourceFilename(JoinPaths("music", "table.cfg"), gamepath, false, true);

    if(musicpath == "") return false;

    fileloaded = OpenGameFileR(Tablefile, musicpath);

    if(!fileloaded) return false;

    std::string str_buf;

    while(!Tablefile.eof())
    {
        getline(Tablefile, str_buf, ' ');
        str_buf.erase(0, str_buf.find_first_not_of('\n'));
        if( str_buf == levelfilename )	// found the level! Load the song!
        {
            // Get the song filename and load it!
            getline(Tablefile, str_buf);
            TrimSpaces(str_buf);
            std::string filename = getResourceFilename(JoinPaths("music", str_buf), gamepath, false, true);
            if( load(filename) )
            {
                play();
                return true;
            }
            else
            {
                return false;
            }
        }
        Tablefile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return false;
}

int CMusic::readTrackNofromMusicTable(const std::string &gamepath,
                                      const std::string &levelfilename)
{
    bool fileloaded = false;
    std::ifstream Tablefile;

    std::string musicpath = getResourceFilename(JoinPaths("music", "table.cfg"), gamepath, false, true);

    if(musicpath == "") return -3;

    fileloaded = OpenGameFileR(Tablefile, musicpath);

    if(!fileloaded) return -2;

    std::string str_buf;

    while(!Tablefile.eof())
    {
        getline(Tablefile, str_buf, ' ');
        str_buf.erase(0, str_buf.find_first_not_of('\n'));
        if( str_buf == levelfilename )	// found the level! Load the song!
        {
            // Get the song filename and load it!
            getline(Tablefile, str_buf);
            TrimSpaces(str_buf);

            return atoi(str_buf.c_str());
        }
        Tablefile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return -1;
}



CMusic::~CMusic()
{
	stop();
}

