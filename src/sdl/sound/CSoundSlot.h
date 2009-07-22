/*
 * CSoundSlot.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#ifndef CSOUNDSLOT_H_
#define CSOUNDSLOT_H_

#include "../../hqp/hq_sound.h"

class CSoundSlot {
public:
	CSoundSlot();
	virtual ~CSoundSlot();

	bool loadSound(const char *fname, const char *searchname, unsigned int loadnum);

	void setpAudioSpec(SDL_AudioSpec *pAudioSpec){ m_pAudioSpec = pAudioSpec; }

	unsigned char getPriority(void){ return m_priority; }
	bool isHighQuality(void){ return m_hqsound.enabled; }
	unsigned int *getSoundData(void){ return m_sounddata; }
	unsigned int getSoundlength(void) { return m_soundlength; }
	stHQSound	*getHQSoundPtr(void) { return &m_hqsound; }

private:
	unsigned char m_priority;
	unsigned int *m_sounddata;
	unsigned int m_soundlength;
	stHQSound m_hqsound;
	SDL_AudioSpec *m_pAudioSpec;
};

#endif /* CSOUNDSLOT_H_ */
