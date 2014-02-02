/*
 * CMusic.h
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 *
 *  This class is a singleton and management class.
 *  The Music player itself is called here!
 */

#ifndef CMUSIC_H_
#define CMUSIC_H_

#include "fileio/CExeFile.h"
#include "sdl/music/CMusicPlayer.h"

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>
#include <base/GsEvent.h>

#include "CSingleton.h"
#define g_pMusicPlayer CMusic::Get()

class CMusic : public CSingleton<CMusic>
{
public:
	virtual ~CMusic();

	/**
	 * Loads certain track of the Keen Music
	 */
	bool loadTrack(const CExeFile& ExeFile, const int track);
    bool load(const std::string &musicfile);
	void reload();
	void play();
	void pause();
	void stop();
	void readWaveform(Uint8* buffer, size_t length); // Reads the next chunk of the ogg stream
	bool LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename);
	bool LoadfromSonglist(const std::string &gamepath, const int &level);

	bool playing()
	{
		if(mpPlayer)
		{
		  return mpPlayer->playing();
		}
		return false;
	}

	std::unique_ptr<CMusicPlayer> mpPlayer;
	bool m_busy;
};

struct EventPlayTrack : CEvent {
    const uint32_t track;
    EventPlayTrack(const uint16_t t) : track(t) {}
};

#endif /* CMUSIC_H_ */
