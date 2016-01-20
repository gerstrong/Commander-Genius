/*
 * CAudioResources.h
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIORESOURCES_H_
#define CAUDIORESOURCES_H_

#include "sdl/audio/sound/CSoundSlot.h"
#include "sdl/audio/base/COPLEmulator.h"
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
SOUND_KEEN_WALK = 0, SOUND_KEEN_WALK2,
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
SOUND_WLD_WALK,

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
    CAudioResources();
	virtual ~CAudioResources() {}

    virtual bool loadSoundData(const unsigned int dictOffset) = 0;
	virtual void unloadSound() = 0;

    bool readISFintoWaveForm(CSoundSlot &soundslot, const byte *imfdata, const Uint8 formatsize );
	
	CSoundSlot *getSlotPtr(){	return &m_soundslot[0];	}
	CSoundSlot *getSlotPtrAt(const unsigned int idx){	return &m_soundslot[idx];	}
	unsigned int getNumberofSounds() {	return m_soundslot.size();	}

protected:

    std::vector<CSoundSlot> m_soundslot;

    /**
     * @brief generateWave      Generates a wave from the PC Speaker emulation engine
     * @param waveform          waveform where to write that data for the soundcard
     * @param waveSampleSize    size of the wavesample (16-bit sound has two bytes, 8-bit only one)
     * @param wavetime          total time in frame the beep has to run
     * @param inBuffer          Input data which is to convert
     * @param numOfBeeps        number of Beeps to produce
     * @param isVorticons       Is it vorticon or Galaxy Keen PC Speaker format?
     * @param AMP               Amplitude
     * @param audioSpec         SDL_Structure used for the waveform
     */
    void generateWave(byte* waveform,
                      const int waveSampleSize,
                      const unsigned int wavetime,
                      byte *inBuffer,
                      unsigned int numOfBeeps,
                      bool isVorticons,
                      const int& AMP,
                      const SDL_AudioSpec &audioSpec);

private:

    /**
     * @brief generateBeep      Converts a classical PC Speaker Beep into a waveform
     * @param waveform          address of the waveform where in which to generate the beep
     * @param sample            non-waveform sample
     * @param sampleSize        size of the wavesample (16-bit sound has two bytes, 8-bit only one)
     * @param wavesample        current wavesample state
     * @param freqtimer         Timer of the PC Speaker
     * @param AMP               Amplitude
     * @param silence           Silence level
     * @param channels          Number of channels the waveform has...
     * @param wavetime          time in frames the beep has to run
     * @param frequency         Frequency of the waveform
     */
    void generateBeep(byte *waveform,
                      word sample,
                      word sampleSize,
                      int wavesample,
                      Uint64 &freqtimer,
                      const int AMP,
                      const int silence,
                      const int channels,
                      const unsigned int wavetime,
                      const int frequency);





};

#endif /* CAUDIORESOURCES_H_ */
