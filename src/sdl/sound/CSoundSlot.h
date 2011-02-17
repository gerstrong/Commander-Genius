/*
 * CSoundSlot.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#ifndef CSOUNDSLOT_H_
#define CSOUNDSLOT_H_

#include "fileio/TypeDefinitions.h"
#include <string>
#include <vector>

class CSoundSlot {
public:

	std::string m_gamepath;

	CSoundSlot();
	
	void openOGGSound(const std::string& filename, SDL_AudioSpec *pspec, Uint8 *&SoundBuffer, Uint32 &SoundLen);
	void setupWaveForm( Uint8 *buf, Uint32 len );
	void setupWaveForm( const std::vector<Uint8>& waveform );
	bool HQSndLoad(const std::string& gamepath, const std::string& soundname);
	void unload();
	
	void setpAudioSpec(const SDL_AudioSpec *pAudioSpec){ m_pAudioSpec = const_cast<SDL_AudioSpec*>(pAudioSpec); }
	
	byte *getSoundData(){ return m_sounddata; }
	SDL_AudioSpec &getAudioSpec() const { return *m_pAudioSpec; }
	unsigned int getSoundlength() { return m_soundlength; }
	
	~CSoundSlot();

private:
	byte *m_sounddata;
	unsigned int m_soundlength;
	SDL_AudioSpec *m_pAudioSpec;
};

#endif /* CSOUNDSLOT_H_ */
