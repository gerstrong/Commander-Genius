/*
 * CSoundChannel.h
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#ifndef CSOUNDCHANNEL_H_
#define CSOUNDCHANNEL_H_

#include <SDL.h>
#include <map>
#include "CSoundSlot.h"
#include "sounds.h"

// 8 bit sound
#define WAVEFORM_VOLUME_8	5
#define WAVE_SILENCE_U8         128
#define WAVE_SILENCE_S8         0
#define WAVE_IN_U8		(WAVE_SILENCE_U8-WAVEFORM_VOLUME_8)
#define WAVE_OUT_U8		(WAVE_SILENCE_U8+WAVEFORM_VOLUME_8)
#define WAVE_IN_S8		(WAVE_SILENCE_S8-WAVEFORM_VOLUME_8)
#define WAVE_OUT_S8		(WAVE_SILENCE_S8+WAVEFORM_VOLUME_8)
// 16 bit sound
#define WAVEFORM_VOLUME_16	1280
#define WAVE_SILENCE_U16        32768
#define WAVE_SILENCE_S16        0
#define WAVE_IN_U16		(WAVE_SILENCE_U16-WAVEFORM_VOLUME_16)
#define WAVE_OUT_U16		(WAVE_SILENCE_U16+WAVEFORM_VOLUME_16)
#define WAVE_IN_S16		(WAVE_SILENCE_S16-WAVEFORM_VOLUME_16)
#define WAVE_OUT_S16		(WAVE_SILENCE_S16+WAVEFORM_VOLUME_16)

#define SLOW_RATE      90		// Wait time for resampling PC Speaker Sound.
//#define SLOW_RATE      45		// Wait time for resampling PC Speaker Sound.
// The higher it is, the faster sound is played!


class CSoundChannel {
public:
	CSoundChannel(SDL_AudioSpec AudioSpec);
	virtual ~CSoundChannel();

	void stopSound(void);
	bool isPlaying() { return m_sound_playing; }
	bool isForcedPlaying() { return (m_sound_playing && m_sound_forced); }
	GameSound getCurrentsound() { return m_current_sound; }
	void readWaveform(Uint8* waveform, int len, Uint8 channels, int frequency);
	template <typename T>
	void generateWaveform(T *waveform, unsigned int len, int frequency, bool stereo );
	void transintoStereoChannels(Uint8* waveform, unsigned int len);

	short getBalance() { return m_balance; }
	void setBalance(short value) { m_balance = value; }
	void setFrequencyCorrection(int freq);
	void enableHighQuality(bool value) { m_hq = value; }

	void setFormat( Uint16 format );
	void setupSound(GameSound current_sound,
					unsigned int sound_timer,
					bool playing,
					unsigned int freqtimer,
					bool sound_forced,
					Uint16 format);

	void setSoundSlotPtr(CSoundSlot *pSoundSlot) { m_pSoundSlot = pSoundSlot;}

private:
    bool m_sound_playing;           	// true = a sound is currently playing
    bool m_hq;					   		// true = the sound is high quality
    GameSound m_current_sound;   	    // # of the sound that is currently playing
    Uint32 m_sound_ptr;               	// position within sound that we're at
    unsigned int m_sound_timer;     	// used to slow down the rate of playback
	bool m_sound_paused;             	// true = pause playback
    bool m_sound_forced;

    unsigned int m_desiredfreq;     	// current desired frequency in hz
    unsigned int m_changerate;      	// frequency in samples (calculated)
    unsigned int m_freqtimer;       	// time when to change waveform state
    Sint32 m_waveState;                	// current position of the output waveform
    short m_balance;					// This variable is used for stereo sound, and to calculate where the sound must be played!
    unsigned int m_freq_corr;			// used for correcting PC-Speaker sampling for different frequencies

    SDL_AudioSpec m_AudioSpec;
    Sint32 m_waveout;
    Sint32 m_wavein;
    Sint32 m_volume;

    CSoundSlot	*m_pSoundSlot;			// Pointer to the Soundslots of CSound
};

#endif /* CSOUNDCHANNEL_H_ */
