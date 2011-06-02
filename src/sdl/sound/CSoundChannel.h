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
#include "common/CAudioResources.h"

// sound play modes
// start playing sound now, unless a higher priority sound is playing
enum SoundPlayMode{
// Normal play now
PLAY_NOW,
// will not restart the sound if it is already playing.
PLAY_NORESTART,
// plays the sound regardless of priority, and does not allow any other
// sounds to play until it completes.
PLAY_FORCE,
// This will play the sound and pause the gameplay while it does
PLAY_PAUSEALL
};

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

	void stopSound();
	bool isPlaying() { return m_sound_playing; }
	bool isForcedPlaying() { return (m_sound_playing && m_sound_forced); }
	unsigned char getCurrentsound() { return m_current_sound; }

	/**
	 * \brief	Reads the sound of a specified slot into the waveform which normally is mixed
	 * \param	pSndSlot Array to the Collection of slots
	 * \param	waveform 8-bit data array where the mixform will be written to
	 * \param	length in bytes that have to be read
	 * \warning	If there is no sound curently assigned to be played, please don't call that function.
	 * 			It might crash.
	 */
	void readWaveform(CSoundSlot * const pSndSlot, Uint8 * const waveform, const Uint32 len);
	template <typename T>
	void transintoStereoChannels(T* waveform, const Uint32 len);

	short getBalance() { return m_balance; }
	void setBalance(short value) { m_balance = value; }

	void setupSound(const unsigned char current_sound,
					const unsigned int sound_timer,
					const bool playing,
					const unsigned int freqtimer,
					const bool sound_forced,
					const Uint16 format);

private:
    bool m_sound_playing;           	// true = a sound is currently playing
    unsigned char m_current_sound;   	    // # of the sound that is currently playing
    Uint32 m_sound_ptr;               	// position within sound that we're at
	bool m_sound_paused;             	// true = pause playback
    bool m_sound_forced;

    unsigned int m_desiredfreq;     	// current desired frequency in hz
    short m_balance;					// This variable is used for stereo sound, and to calculate where the sound must be played!

    SDL_AudioSpec m_AudioSpec;
};

#endif /* CSOUNDCHANNEL_H_ */
