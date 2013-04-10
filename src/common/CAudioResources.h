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
#include <vector>

/** This is the PC Speaker Volume it will set.
  * When the PC Speaker Emulator generates the Sound slots
  * it will do it with that volume.
  * Remember, this is relative to the real Sound volume you can toggle in the Audio Settings
  * Menu of CG
  */
const int PC_Speaker_Volume = 20; // in percent

const Uint64 PCSpeakerTime = 0x1234DD;

typedef struct
{
    Instrument      inst;
    byte            block;
} AdLibSound;

// Game Sounds
enum GameSound
{
// Common Blocks
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
SOUND_OPEN_EXIT_DOOR,
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

// Ep3 Specific
SOUND_MEEP,
SOUND_ANKH,
SOUND_MORTIMER,
SOUND_FOOTSLAM,

// Galaxy Specific
SOUND_CANT_DO,
SOUND_GET_WETSUIT,
SOUND_KEEN_SWIM,
SOUND_KEEN_SWIM_TO_LAND,
SOUND_GET_AMMO,
SOUND_GET_DROP,
SOUND_GET_GEM,
SOUND_RESCUE_COUNCIL_MEMBER,
SOUND_STATUS_SLIDE_IN,
SOUND_STATUS_SLIDE_OUT,
SOUND_BUBBLE,
SOUND_FLAG_APPEAR,
SOUND_FLAG_LAND,
SOUND_PLAYER_PADDLE,
SOUND_COMPUTER_PADDLE,
SOUND_HIT_SIDEWALL,
SOUND_COMPUTER_POINT,
SOUND_PLAYER_POINT,
SOUND_KEEN_LAND_ALT,

// EP4
SOUND_SQUISH_SKYPEST,
SOUND_MINE_EXPLOSION,
SOUND_SPRITE_SHOT,
SOUND_MIMROCK_BOUNCE,
SOUND_TRESURE_STEALER_TELEPORT,
SOUND_WORMOUTH_STRIKE,
SOUND_LICK_FIREBREATH,
SOUND_BERKELOID_WINDUP,
SOUND_THUNDERCLOUD_STRIKE,
SOUND_BERKELOID_FIREBALL_LANDING,
SOUND_DARTGUN_SHOOT,
SOUND_DOPEFISH_BURP,
SOUND_SLUG_DEFECATE,
SOUND_BOUNCE_LOW,
SOUND_BOUNCE_HIGH,

// EP5
SOUND_MINEEXPLODE,
SOUND_SLICEBUMP,
SOUND_SPHEREFULCEILING,
SOUND_POLEZAP,
SOUND_SHOCKSUNDBARK,
SOUND_SPINDREDFLYUP,
SOUND_SPIROGRIP,
SOUND_SPINDREDSLAM,
SOUND_ROBORED_SHOOT,
SOUND_AMPTONWALK0,
SOUND_AMPTONWALK1,
SOUND_SPIROFLY,
SOUND_ROBO_STUN,
SOUND_SPARKY_CHARGE,
SOUND_SPINDREDFLYDOWN,
SOUND_MASTERSHOT,
SOUND_MASTERTELE,
SOUND_ELEVATING,
SOUND_ELEVATOR_OPEN,

// EP6
SOUND_JUMPED_GEM,
SOUND_ORBATRIX_BUMP,
SOUND_CEILICK_LICK,
SOUND_CEILICK_LAUGH,
SOUND_BABOBBA_JUMP,
SOUND_BABOBBA_LAND,
SOUND_BOBBA_LAND,
SOUND_BOBBA_SHOOT,
SOUND_KEEN_STUNNED,
SOUND_ROPE_THROW,
SOUND_ROCKET_DRIVE,
SOUND_ROCKET_LAUNCH,
SOUND_GRABBITER_SLEEP,
SOUND_GRABBITER_HUNGRY,
SOUND_BIP_SHIP_CRASH,
SOUND_BIP_SHIP_LAND,
SOUND_SQUISH,
SOUND_AUTOGUN,
SOUND_BIPSHIP_SHOOT,
SOUND_SATELITE_MOUNT,
SOUND_GET_SPECIAL_ITEM,
SOUND_BLOOGGUARD_STUB,
SOUND_BABOBBA_CINDER

};

class CAudioResources
{
public:
	CAudioResources(const SDL_AudioSpec &AudioSpec);
	virtual ~CAudioResources() {}

	virtual bool loadSoundData() = 0;
	virtual void unloadSound() = 0;

	template <typename T>
	void generateWave(std::vector<T> &waveform, byte *inBuffer, unsigned int numOfBeeps, bool isVorticons, const int& AMP)
	{
		/** If PC_SPEAKER_WORKS_LIKE_DOSBOX_V0_74 is defined, we attempt
		 * to simulate the way vanilla DOSBox v0.74 emulates the PC Speaker.
		 * Might be useful for some Commander Keen packs with alternate sounds effects.
		 */
		Uint64 freqtimer = 0;
		word prevsample = 0, sample;
		T wave = m_AudioSpec.silence - AMP;
		if (isVorticons)
		{
			const unsigned int wavetime = m_AudioSpec.freq*1000/145575;
			for (sample = READWORD(inBuffer); sample != 0xffff; sample = READWORD(inBuffer))
			{
				#ifdef PC_SPEAKER_WORKS_LIKE_DOSBOX_V0_74
				if (prevsample != 0)
					freqtimer %= m_AudioSpec.freq*prevsample;
				#else
				// On Keen 1-3, separated consecutive samples are always separated.
				wave = m_AudioSpec.silence - AMP;
				freqtimer = 0;
				#endif
				generateBeep(waveform, sample, wave, freqtimer, AMP, wavetime, (m_AudioSpec.freq>>1)*Uint64(sample));
				prevsample = sample;
			}
		}
		/** Effective number of samples is actually size-1, so we enumerate from 1.
		 * Reason: The vanilla way, right after beginning the very last sample output,
		 * it's stopped. (That should be validated in some way...)
		 */
		else
		{
			const unsigned int wavetime = m_AudioSpec.freq*1000/140026;
			for(unsigned pos=1 ; pos<numOfBeeps ; pos++)
			{
				// Multiplying by some constant (60 in our case) seems to reproduces the right sound.
				sample = *(inBuffer++) * 60;
				#ifdef PC_SPEAKER_WORKS_LIKE_DOSBOX_V0_74
				if (prevsample != 0)
					freqtimer %= m_AudioSpec.freq*prevsample;
				#else
				/** On Keen 4-6, consecutive samples of the exact
				 * same frequency are merged into a single tone.
				 */
				if (prevsample != sample)
				{
					wave = m_AudioSpec.silence - AMP;
					freqtimer = 0;
				}
				#endif
				generateBeep(waveform, sample, wave, freqtimer, AMP, wavetime, (m_AudioSpec.freq>>1)*Uint64(sample));
				prevsample = sample;
			}
		}
	}

	bool readISFintoWaveForm( CSoundSlot &soundslot, const byte *imfdata, const unsigned int bytesize, const Uint8 formatsize );
	
	CSoundSlot *getSlotPtr(){	return &m_soundslot[0];	}
	CSoundSlot *getSlotPtrAt(const unsigned int idx){	return &m_soundslot[idx];	}
	unsigned int getNumberofSounds() {	return m_soundslot.size();	}

protected:
	std::vector<CSoundSlot>m_soundslot;
	const SDL_AudioSpec &m_AudioSpec;

private:
	template <typename T>
	void generateBeep(std::vector<T> &waveform, word sample, T &wave, Uint64 &freqtimer, const int& AMP, const unsigned int& wavetime, const Uint64& changerate)
	{
		if (sample != 0)
			for (unsigned int j=0; j<wavetime; j++)
			{
				if (freqtimer > changerate)
				{
					freqtimer %= changerate;

					if (wave == m_AudioSpec.silence - AMP)
						wave = m_AudioSpec.silence + AMP;
					else
						wave = m_AudioSpec.silence - AMP;
				}
				freqtimer += PCSpeakerTime;

				for(Uint8 chnl=0 ; chnl<m_AudioSpec.channels ; chnl++ )
					waveform.push_back(wave);
			}
		else
			for (unsigned int j=0; j<wavetime*m_AudioSpec.channels; j++)
				waveform.push_back(wave);
	}
};

#endif /* CAUDIORESOURCES_H_ */
