/*
 * CSoundChannel.cpp
 *
 *  Created on: 23.05.2009
 *      Author: gerstrong
 */

#include "CSoundChannel.h"

CSoundChannel::CSoundChannel(SDL_AudioSpec AudioSpec) :
m_AudioSpec(AudioSpec)
{
	m_sound_ptr = 0;
	m_sound_playing = false;
	m_sound_paused = true;
	m_sound_forced = false;
	m_desiredfreq = 0;     	// current desired frequency in hz
	m_balance = 0;
	
}

void CSoundChannel::stopSound()
{
    m_sound_ptr = 0;
    m_sound_playing = false;
}

void CSoundChannel::setupSound(const unsigned char current_sound,
					const unsigned int sound_timer,
					const bool playing,
					const unsigned int freqtimer,
					const bool sound_forced,
					const Uint16 format)
{
	m_current_sound = current_sound;
	m_sound_playing = playing;
	m_sound_ptr = 0;
	m_sound_forced = sound_forced;
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
		// -127 is only for the left speaker, while 127 is for the right speaker. 0 Is center
		Sint32 Pulse32;
		const Sint32 Silence = m_AudioSpec.silence;
		const Sint32 balance = m_balance;
		const Uint32 length = len/sizeof(T);
		
		// balance the left channel.
		for( Uint32 index = 0 ; index < length ; )
		{
			/// balance here!
			// first the left channel
			Pulse32 = waveform[index] - Silence;
			Pulse32 *= (129 - balance);
			Pulse32 >>= 8;
			waveform[index++] = Pulse32 + Silence;

			// then the right channel
			Pulse32 = waveform[index] - Silence;
			Pulse32 *= (129 + balance);
			Pulse32 >>= 8;
			waveform[index++] = Pulse32 + Silence;
		}
	}
}

void CSoundChannel::readWaveform(CSoundSlot *pSndSlot, Uint8* waveform, int len, Uint8 channels, int frequency)
{
    if (m_sound_playing)
    {
    	CSoundSlot &SndSlot = pSndSlot[m_current_sound];

    	byte *snddata = SndSlot.getSoundData();
    	const Uint32 length = SndSlot.getSoundlength();
    	if ((m_sound_ptr + (Uint32)len) >= length)
    	{
    		// Fill the rest with silence
    		memset(waveform, m_AudioSpec.silence, len );
    		m_sound_ptr = 0;
    		m_sound_playing = false;
    	}
    	else
    	{
    		memcpy(waveform, snddata + m_sound_ptr, len);
    		m_sound_ptr += len;
    	}
		
    	if(channels == 2)
    	{
    		if(m_AudioSpec.format == AUDIO_U16 || m_AudioSpec.format == AUDIO_S16)
    			transintoStereoChannels((Sint16*) (void *) waveform, len);
    		else
    			transintoStereoChannels(waveform, len);
    	}

    }
	else
		memset(waveform, m_AudioSpec.silence, len);
}
