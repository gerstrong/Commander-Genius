/*
 * CMusic.h
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#ifndef CMUSIC_H_
#define CMUSIC_H_

#include "vorbis/oggsupport.h"


enum playstatus{
	PLAY_MODE_STOP,
	PLAY_MODE_PAUSE,
	PLAY_MODE_PLAY
};

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

	bool load(const SDL_AudioSpec AudioSpec, const std::string &musicfile);
	void reload();
	void unload(void);
	void play(void);
	void stop(void);
	void readBuffer(Uint8* buffer, size_t length); // Reads the next chunk of the ogg stream
	bool LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename);

	int playing(void){return playmode;}

private:
	int playmode;
	std::string usedMusicFile;
	OggVorbis_File  m_oggStream;

	SDL_AudioSpec m_AudioSpec;
	SDL_AudioSpec m_AudioFileSpec;
};

#endif /* CMUSIC_H_ */
