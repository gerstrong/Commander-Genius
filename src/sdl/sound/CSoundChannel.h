/*
 * CSoundChannel.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#ifndef CSOUNDCHANNEL_H_
#define CSOUNDCHANNEL_H_

#include <SDL.h>
#include "CSoundSlot.h"

#define WAVEFORM_VOLUME         5
#define WAVE_SILENCE            128
#define WAVE_IN                 (WAVE_SILENCE-WAVEFORM_VOLUME)
#define WAVE_OUT                (WAVE_SILENCE+WAVEFORM_VOLUME)

#define SLOW_RATE      90		// Wait time for resampling PC Speaker Sound.
								// The higher it is, the faster sound is played!


class CSoundChannel {
public:
	CSoundChannel();
	virtual ~CSoundChannel();

	void stopSound(void);
	bool isPlaying() { return m_sound_playing; }
	bool isForcedPlaying() { return (m_sound_playing && m_sound_forced); }
	unsigned short getCurrentsound() { return m_current_sound; }
	void readWaveform(Uint8* waveform, int len, Uint8 channels, int frequency);
	void generateWaveform(Uint8 *waveform, unsigned int len, int frequency, bool stereo);
	void transintoStereoChannels(Uint8* waveform, unsigned int len);

	short getBalance() { return m_balance; }
	void setBalance(short value) { m_balance = value; }
	void setFrequencyCorrection(int freq);
	void enableHighQuality(bool value) { m_hq = value; }

	void setupSound(unsigned short current_sound,
					unsigned int sound_timer,
					bool playing,
					int waveState,
					unsigned int freqtimer,
					bool sound_forced);

	void setSoundSlotPtr(CSoundSlot	*pSoundSlot) { m_pSoundSlot = pSoundSlot;}

private:
    bool m_sound_playing;           	// true = a sound is currently playing
    bool m_hq;					   		// true = the sound is high quality
    unsigned short m_current_sound;   	// # of the sound that is currently playing
    Uint32 m_sound_ptr;               	// position within sound that we're at
    unsigned int m_sound_timer;     	// used to slow down the rate of playback
	bool m_sound_paused;             	// true = pause playback
    bool m_sound_forced;

    unsigned int m_desiredfreq;     	// current desired frequency in hz
    unsigned int m_changerate;      	// frequency in samples (calculated)
    unsigned int m_freqtimer;       	// time when to change waveform state
    int m_waveState;                	// current position of the output waveform
    short m_balance;					// This variable is used for stereo sound, and to calculate where the sound must be played!
    unsigned int m_freq_corr;				// used for correcting PC-Speaker sampling for different frequencies

    CSoundSlot	*m_pSoundSlot;			// Pointer to the Soundslots of CSound
};

#endif /* CSOUNDCHANNEL_H_ */
