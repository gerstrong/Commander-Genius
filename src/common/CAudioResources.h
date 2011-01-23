/*
 * CAudioResources.h
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIORESOURCES_H_
#define CAUDIORESOURCES_H_

#include "sdl/sound/sounds.h"
#include "sdl/sound/CSoundSlot.h"
#include <string>


class CAudioResources {
public:
	CAudioResources(const SDL_AudioSpec &AudioSpec);
	virtual ~CAudioResources();

	virtual bool loadSoundData() = 0;
	virtual void unloadSound() = 0;

	CSoundSlot *getSlotPtr() { return m_soundslot; }

protected:
    // TODO: use std::map for Soundslots.
	static const int MAX_SOUNDS = 50;
	CSoundSlot	m_soundslot[MAX_SOUNDS];
};

#endif /* CAUDIORESOURCES_H_ */
