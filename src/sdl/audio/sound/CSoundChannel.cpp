/*
 * CSoundChannel.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#include "CSoundChannel.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>


CSoundChannel::CSoundChannel(const SDL_AudioSpec &AudioSpec) :
mp_current_SndSlot(nullptr),
m_AudioSpec(AudioSpec)
{
    stopSound();
}

void CSoundChannel::stopSound()
{
    SDL_LockAudio();

    mp_current_SndSlot = nullptr;
	m_balance = 0;
    m_sound_ptr = 0;
    m_sound_paused = true;
    m_sound_playing = false;

    SDL_UnlockAudio();
}

void CSoundChannel::setupSound( CSoundSlot &SndSlottoPlay,
								const bool sound_forced )
{
    SDL_LockAudio();

	mp_current_SndSlot = &SndSlottoPlay;
	m_sound_playing = true;
	m_sound_ptr = 0;
	m_sound_forced = sound_forced;

    SDL_UnlockAudio();
}

/** \brief This program reads the balance information and balances the stereo sound
 * 	\param waveform	pass it as 8-bit or 16-bit Waveform pointer depeding on what depth you have
 *  \param len 		length in bytes of the waveform
 */
template <typename T>
void CSoundChannel::transintoStereoChannels(T* waveform, const Uint32 len)
{
	if(m_balance != 0) // Because, if it is not zero, there is no balance, and waves must be adapted
	{
		// m_balance -127 is only for the left speaker, while 127 is for the right speaker. 0 Is center. Everything beyond is played but partially muted.
		Sint32 Pulse32;
		const Sint32 Silence = m_AudioSpec.silence;
		const Uint32 length = len/sizeof(T);

		Sint32 leftamt = -m_balance;
		Sint32 rightamt = m_balance;

		if(leftamt > 127)
		{
		    leftamt = 254 - leftamt;
		    rightamt = 0;
		}

		if(rightamt > 127)
		{
		    rightamt = 254 - rightamt;
		    leftamt = 0;
		}

		// balance the left channel.
		for( Uint32 index = 0 ; index < length ; )
		{
			/// balance here!
			// first the left channel
			Pulse32 = waveform[index] - Silence;
			Pulse32 *= (129 + leftamt);
			Pulse32 >>= 8;
			waveform[index++] = Pulse32 + Silence;

			// then the right channel
			Pulse32 = waveform[index] - Silence;
			Pulse32 *= (129 + rightamt);
			Pulse32 >>= 8;
			waveform[index++] = Pulse32 + Silence;
		}
	}
}

void CSoundChannel::readWaveform( Uint8 * const waveform, const Uint32 len )
{
	byte *snddata = mp_current_SndSlot->getSoundData();
	const Uint32 sndlength = mp_current_SndSlot->getSoundlength();

	if ((m_sound_ptr + len) >= sndlength)
	{
		// Fill up the buffer and the rest with silence
		const Uint32 len_left = sndlength-m_sound_ptr;
		memcpy(waveform, snddata + m_sound_ptr, len_left );
		memset(waveform+len_left, m_AudioSpec.silence, len-len_left );
		m_sound_ptr = 0;
		m_sound_playing = false;
	}
	else
	{
		memcpy(waveform, snddata + m_sound_ptr, len );
		m_sound_ptr += len;
	}

	if(m_AudioSpec.channels == 2)
	{
		if(m_AudioSpec.format == AUDIO_U16 || m_AudioSpec.format == AUDIO_S16)
			transintoStereoChannels((Sint16*) (void *) waveform, len);
		else
			transintoStereoChannels(waveform, len);
	}
}
