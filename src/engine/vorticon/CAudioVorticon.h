/*
 * CAudioVorticon.h
 *
 *  Created on: 22.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIOVORTICON_H_
#define CAUDIOVORTICON_H_

#include "common/CAudioResources.h"
#include "fileio/CExeFile.h"

// Map for the vorticon sound slots
const unsigned char SndSlotMapVort[]=
{
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

class CAudioVorticon : public CAudioResources {
public:
	CAudioVorticon(const CExeFile &ExeFile, const SDL_AudioSpec &AudioSpec);

	bool loadSoundData();
	void unloadSound();

private:
	Uint8* loadSoundStream(Uint32 &buffer_size, Uint8* exedata);

	template <typename T>
	void generateWave(std::vector<T> &waveform, word sample, T &wave, Uint64 &freqtimer, bool IsSigned, const int& AMP);

	template <typename T>
	bool loadPCSpeakerSound(Uint8 *buffer, const Uint32 buf_size, std::vector<T> &waveform, const std::string& searchname, bool IsSigned);

	bool loadSound(Uint8 *buffer, const Uint32 buf_size, const std::string& path, const std::string& searchname, unsigned int loadnum);

	const CExeFile &m_ExeFile;
};

#endif /* CAUDIOVORTICON_H_ */
