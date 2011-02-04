/*
 * CMusic.h
 *
 *  Created on: 02.05.2009
 *      Author: gerstrong
 */

#ifndef CMUSIC_H_
#define CMUSIC_H_

#include "vorbis/oggsupport.h"
#include "fileio/CExeFile.h"

#include <SDL.h>
#include <string>
#include <vector>

#include "CSingleton.h"
#define g_pMusicPlayer CMusic::Get()

enum musicformat
{
	MF_NONE,
	MF_OGG,
	MF_IMF
};

enum playstatus{
	PLAY_MODE_STOP,
	PLAY_MODE_PAUSE,
	PLAY_MODE_PLAY
};

class CMusic : public CSingleton<CMusic>
{
public:
	CMusic();
	virtual ~CMusic();

	bool LoadFromAudioCK(const CExeFile& ExeFile, const int level);
	bool load(const std::string &musicfile);
	void reload();
	void unload(void);
	void play();
	void pause();
	void stop(void);
	void readBuffer(Uint8* buffer, size_t length); // Reads the next chunk of the ogg stream
	bool LoadfromMusicTable(const std::string &gamepath, const std::string &levelfilename);
	bool LoadfromMusicTable(const CExeFile& Exefile, const int level);

	int playing(void){return playmode;}

private:
	int playmode;
	musicformat m_MusicFormat;
	std::string usedMusicFile;

#if defined(OGG) || defined(TREMOR)
	OggVorbis_File  m_oggStream;
#endif

	SDL_AudioSpec m_AudioSpec;
	SDL_AudioSpec m_AudioFileSpec;
	bool m_open;
	SDL_AudioCVT  m_Audio_cvt;
};

#endif /* CMUSIC_H_ */
