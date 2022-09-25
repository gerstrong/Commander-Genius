/*
 * CAudioResources.h
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#ifndef CAUDIORESOURCES_H_
#define CAUDIORESOURCES_H_

#include "base/audio/sound/CSoundSlot.h"
#include "base/COPLEmulator.h"
#include <string>
#include <vector>

/** This is the PC Speaker Volume it will set.
  * When the PC Speaker Emulator generates the Sound slots
  * it will do it with that volume.
  * Remember, this is relative to the real Sound volume you can toggle in the Audio Settings
  * Menu of CG
  */
//const int PC_Speaker_Volume = 20; // in percent

const Uint64 PCSpeakerTime = 0x1234DD;

typedef struct
{
    Instrument      inst;
    gs_byte            block;
} AdLibSound;


class CAudioResources
{
public:
    CAudioResources();
    virtual ~CAudioResources();

    virtual bool loadSoundData(const unsigned int dictOffset) = 0;
	virtual void unloadSound() = 0;

    bool readISFintoWaveForm(CSoundSlot &soundslot, const gs_byte *imfdata, const Uint8 formatsize );
	
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
    void generateWave(gs_byte* waveform,
                      const int waveSampleSize,
                      const unsigned int wavetime,
                      gs_byte *inBuffer,
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
    void generateBeep(gs_byte *waveform,
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
