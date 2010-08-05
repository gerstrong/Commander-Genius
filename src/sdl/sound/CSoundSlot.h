/*
 * CSoundSlot.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#ifndef CSOUNDSLOT_H_
#define CSOUNDSLOT_H_

#include "../../hqp/hq_sound.h"
#include <string>

class CSoundSlot {
public:

	std::string m_gamepath;

	CSoundSlot();
	
	bool loadSound(const std::string& fname, const std::string& path, const std::string& searchname, unsigned int loadnum);
	
	void setpAudioSpec(SDL_AudioSpec *pAudioSpec){ m_pAudioSpec = pAudioSpec; }
	
	bool isHighQuality(void){ return m_hqsound.enabled; }
	unsigned int *getSoundData(void){ return m_sounddata; }
	unsigned int getSoundlength(void) { return m_soundlength; }
	stHQSound	*getHQSoundPtr(void) { return &m_hqsound; }
	
	virtual ~CSoundSlot();

private:
	unsigned int *m_sounddata;
	unsigned int m_soundlength;
	stHQSound m_hqsound;
	SDL_AudioSpec *m_pAudioSpec;
};

#endif /* CSOUNDSLOT_H_ */
