/*
 * CSoundSlot.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#ifndef CSOUNDSLOT_H_
#define CSOUNDSLOT_H_

#include "sdl/sound/hq_sound.h"
#include <string>

class CSoundSlot {
public:

	std::string m_gamepath;

	CSoundSlot();
	
	bool loadSound(Uint8 *buffer, const Uint32 buf_size, const std::string& path, const std::string& searchname, unsigned int loadnum);
	void unload();
	
	void setpAudioSpec(const SDL_AudioSpec *pAudioSpec){ m_pAudioSpec = const_cast<SDL_AudioSpec*>(pAudioSpec); }
	
	bool isHighQuality(){ return m_hqsound.enabled; }
	unsigned int *getSoundData(void){ return m_sounddata; }
	unsigned int getSoundlength(void) { return m_soundlength; }
	stHQSound	*getHQSoundPtr(void) { return &m_hqsound; }
	
	~CSoundSlot();

private:
	unsigned int *m_sounddata;
	unsigned int m_soundlength;
	stHQSound m_hqsound;
	SDL_AudioSpec *m_pAudioSpec;
};

#endif /* CSOUNDSLOT_H_ */
