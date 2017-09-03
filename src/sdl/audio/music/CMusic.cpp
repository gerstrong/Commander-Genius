/*
 * CMusic.cpp
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#include "sdl/audio/Audio.h"
#include "sdl/audio/base/Sampling.h"
#include "sdl/audio/music/COGGPlayer.h"
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

    gSound.pauseAudio();

    gLogging.textOut("Load track number " + itoa(track) + "");

#if defined(OGG) || defined(TREMOR)
    mpPlayer.reset( new COGGPlayer );

    if(mpPlayer->loadMusicTrack(track))
    {
        gSound.resumeAudio();
        return true;
    }
#endif

    mpPlayer.reset( new CIMFPlayer );
    if(!mpPlayer->loadMusicTrack(track))
    {
        gLogging.textOut("No music to be loaded for Track" + itoa(track) + ".");
    }

    gSound.resumeAudio();
	return true;
}


bool CMusic::load(const std::string &musicfile)
{        
	mpPlayer.reset();

	if(musicfile == "")
		return false;

	const SDL_AudioSpec &audioSpec = gSound.getAudioSpec();

	if(audioSpec.format != 0)
	{
		std::string extension = GetFileExtension(musicfile);

        gSound.pauseAudio();

		stringlwr(extension);

		if( extension == "imf" )
		{
            mpPlayer.reset( new CIMFPlayer );

            if(!mpPlayer->loadMusicFromFile(musicfile))
            {
                return false;
            }
		}
		else if( extension == "ogg" )
		{
#if defined(OGG) || defined(TREMOR)
            mpPlayer.reset( new COGGPlayer );
            mpPlayer->loadMusicFromFile(musicfile);
#else
		    gLogging.ftextOut("Music Manager: Neither OGG bor TREMOR-Support are enabled! Please use another build<br>");
            gSound.resumeAudio();
		    return false;
#endif
		}

        if(!mpPlayer->open(true))
		{
		    mpPlayer.reset();
		    gLogging.textOut(FONTCOLORS::PURPLE,"Music Manager: File could not be opened: \"%s\". File is damaged or something is wrong with your soundcard!<br>", musicfile.c_str());

            gSound.resumeAudio();
		    return false;
        }

        gSound.resumeAudio();
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

	if(!mpPlayer)
	{
		return;
	}

	mpPlayer->reload();

    gSound.resumeAudio();
}

void CMusic::play()
{
	if(!mpPlayer)
	{
		return;
	}

	mpPlayer->play(true);
}

void CMusic::pause()
{
	if(!mpPlayer)
		return;

	mpPlayer->play(false);
}

void CMusic::stop()
{
	if(!mpPlayer)
		return;

    gSound.pauseAudio();

    mpPlayer->close(true);
    mpPlayer.reset();

    gSound.resumeAudio();
}

// length only refers to the part(buffer) that has to be played
void CMusic::readWaveform(Uint8* buffer, size_t length)
{
	if( !mpPlayer )
		return;

	mpPlayer->readBuffer(buffer, length);
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
    				play();
    			return true;
    		}
    	}
    }
	return false;
}

bool CMusic::LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename)
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
            }
            return true;
        }
        Tablefile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return false;
}

CMusic::~CMusic()
{
	stop();
}

