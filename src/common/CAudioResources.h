/*
 * CAudioResources.h
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIORESOURCES_H_
#define CAUDIORESOURCES_H_

#include "sdl/sound/CSoundSlot.h"
#include "hardware/COPLEmulator.h"
#include <string>

typedef struct
{
    Instrument      inst;
    byte            block;
} AdLibSound;

// Game Sounds
enum GameSound{
SOUND_KEEN_WALK, SOUND_KEEN_WALK2,
SOUND_KEEN_JUMP, SOUND_KEEN_POGO,
SOUND_KEEN_DIE, SOUND_KEEN_FALL,
SOUND_KEEN_BUMPHEAD,
SOUND_KEENSLEFT,
SOUND_KEEN_FIRE,
SOUND_GUN_CLICK,
SOUND_SHOT_HIT,
SOUND_GET_ITEM,
SOUND_GET_PART,
SOUND_LEVEL_DONE,
SOUND_GAME_OVER,
SOUND_TELEPORT,
SOUND_EXTRA_LIFE,
SOUND_CANNONFIRE,
SOUND_CHUNKSMASH,
SOUND_GOINDOOR,
SOUND_GET_CARD,
SOUND_USE_KEY,
SOUND_DOOR_OPEN,
SOUND_YORP_BUMP,
SOUND_YORP_STUN,
SOUND_YORP_DIE,
SOUND_GARG_DIE,
SOUND_VORT_DIE,
SOUND_KEEN_LAND,
SOUND_GET_BONUS,

SOUND_ENTER_LEVEL,
SOUND_SWITCH_TOGGLE,
SOUND_EARTHPOW,
SOUND_TANK_FIRE,
SOUND_KEEN_BLOK,

SOUND_MEEP,
SOUND_ANKH,
SOUND_MORTIMER,
SOUND_FOOTSLAM
};

class CAudioResources {
public:
	CAudioResources(const SDL_AudioSpec &AudioSpec);

	virtual bool loadSoundData() = 0;
	virtual void unloadSound() = 0;

	bool readISFintoWaveForm( CSoundSlot &soundslot, const byte *imfdata, const unsigned int bytesize );

	CSoundSlot *getSlotPtr() { return m_soundslot; }

protected:
	static const int MAX_SOUNDS = 50;
	CSoundSlot	m_soundslot[MAX_SOUNDS];
	const SDL_AudioSpec &m_AudioSpec;
};

#endif /* CAUDIORESOURCES_H_ */
