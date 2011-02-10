/*
 * CSoundSlot.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#ifndef CSOUNDSLOT_H_
#define CSOUNDSLOT_H_

#include "sdl/sound/hq_sound.h"
#include "fileio/TypeDefinitions.h"
#include <string>
#include <vector>

class CSoundSlot {
public:

	std::string m_gamepath;

	CSoundSlot();
	
	void setupWaveForm( Uint8 *buf, Uint32 len );
	void setupWaveForm( const std::vector<Uint8>& waveform );
	bool loadSound(Uint8 *buffer, const Uint32 buf_size, const std::string& path, const std::string& searchname, unsigned int loadnum);
	void unload();
	
	void setpAudioSpec(const SDL_AudioSpec *pAudioSpec){ m_pAudioSpec = const_cast<SDL_AudioSpec*>(pAudioSpec); }
	
	bool isHighQuality(){ return m_hqsound.enabled; }
	byte *getSoundData(){ return m_sounddata; }
	unsigned int getSoundlength() { return m_soundlength; }
	stHQSound	*getHQSoundPtr() { return &m_hqsound; }
	
	~CSoundSlot();

private:
	byte *m_sounddata;
	unsigned int m_soundlength;
	stHQSound m_hqsound;
	SDL_AudioSpec *m_pAudioSpec;
};

#endif /* CSOUNDSLOT_H_ */
