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

#include "vorbis/oggsupport.h"
#include "fileio/CExeFile.h"
#include "sdl/music/CMusicPlayer.h"

#include <SDL.h>
#include <string>
#include <vector>

#include "CSingleton.h"
#define g_pMusicPlayer CMusic::Get()

class CMusic : public CSingleton<CMusic>
{
public:
	CMusic();
	virtual ~CMusic();

	bool LoadFromAudioCK(const CExeFile& ExeFile, const int level);
	bool load(const std::string &musicfile);
	void unload();
	void reload();
	void play();
	void pause();
	void stop();
	void readBuffer(Uint8* buffer, size_t length); // Reads the next chunk of the ogg stream
	bool LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename);
	bool LoadfromMusicTable(const CExeFile& Exefile, const int level);

	bool playing()
	{
		if(mp_player)
			return mp_player->playing();
		return false;
	}

	SDL_AudioSpec m_AudioSpec;
	CMusicPlayer *mp_player;
	bool m_busy;
};

#endif /* CMUSIC_H_ */
