/*
 * CMusic.h
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#ifndef CMUSIC_H_
#define CMUSIC_H_

enum playstatus{
	PLAY_MODE_STOP,
	PLAY_MODE_PAUSE,
	PLAY_MODE_PLAY
};

#include <SDL.h>
#include <string>
#include <vector>

#include "../CSingleton.h"
#define g_pMusicPlayer CMusic::Get()

class CMusic : public CSingleton<CMusic>
{
public:
	CMusic();
	virtual ~CMusic();

	bool load(const SDL_AudioSpec AudioSpec, const std::string &musicfile);
	void reload(const SDL_AudioSpec AudioSpec);
	void unload(void);
	void play(void);
	void stop(void);
	Uint8 *passBuffer(int length); // returns the buffer that has to be played in every callback
	bool LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename);

	int playing(void){return playmode;}

private:
	std::vector<Uint8> music_buffer;
	long music_len;
	int music_pos;
	int playmode;
	std::string usedMusicFile;
};

#endif /* CMUSIC_H_ */
