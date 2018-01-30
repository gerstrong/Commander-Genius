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

#include <base/GsLogging.h>

#if defined(USE_SDLMIXER)
#include <SDL_mixer.h>
#endif

int CSoundChannel::mTotNumChannels = 0;

CSoundChannel::
CSoundChannel(const SDL_AudioSpec &AudioSpec) :
m_AudioSpec(AudioSpec)
{
    stopSound();
    mId = mTotNumChannels;
    mTotNumChannels++;
}

CSoundChannel::
~CSoundChannel()
{
    mTotNumChannels--;
}

void
CSoundChannel::
stopSound()
{
#if defined(USE_SDLMIXER)
    SDL_LockAudio();
#endif

    mpCurrentSndSlot = nullptr;
    mBalance = 0;
    mSoundPtr = 0;
    mSoundPaused = true;
    mSoundPlaying = false;

#if defined(USE_SDLMIXER)
    SDL_UnlockAudio();
#endif
}

void CSoundChannel::setupSound( CSoundSlot &SndSlottoPlay,
								const bool sound_forced )
{
#if defined(USE_SDLMIXER)

    mpCurrentSndSlot = &SndSlottoPlay;
    mSoundPlaying = true;
    mSoundPtr = 0;
    mSoundForced = sound_forced;

    auto waveChunk = mpCurrentSndSlot->WaveChunk();

    // play sample on first free unreserved channel
    // play it exactly once through
    if(Mix_PlayChannel(mId, waveChunk, 0) == -1)
    {
        gLogging.ftextOut("Mix_PlayChannel: %s\n", Mix_GetError());
    }

#else

    SDL_LockAudio();

    mpCurrentSndSlot = &SndSlottoPlay;
    mSoundPlaying = true;
    mSoundPtr = 0;
    mSoundForced = sound_forced;

    SDL_UnlockAudio();
#endif
}

/** \brief This program reads the balance information and balances the stereo sound
 * 	\param waveform	pass it as 8-bit or 16-bit Waveform pointer depeding on what depth you have
 *  \param len 		length in bytes of the waveform
 */
template <typename T>
void CSoundChannel::transintoStereoChannels(T* waveform, const Uint32 len)
{
    if(mBalance != 0) // Because, if it is not zero, there is no balance, and waves must be adapted
	{
		// m_balance -127 is only for the left speaker, while 127 is for the right speaker. 0 Is center. Everything beyond is played but partially muted.
		Sint32 Pulse32;
		const Sint32 Silence = m_AudioSpec.silence;
		const Uint32 length = len/sizeof(T);

        Sint32 leftamt = -mBalance;
        Sint32 rightamt = mBalance;

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
            // For the left channel
			Pulse32 = waveform[index] - Silence;
			Pulse32 *= (129 + leftamt);
			Pulse32 >>= 8;
            waveform[index++] = T(Pulse32 + Silence);

            // For the Right channel
			Pulse32 = waveform[index] - Silence;
			Pulse32 *= (129 + rightamt);
			Pulse32 >>= 8;
            waveform[index++] = T(Pulse32 + Silence);
		}
	}
}

void CSoundChannel::readWaveform( Uint8 * const waveform, const Uint32 len )
{
    auto snddata = mpCurrentSndSlot->getSoundData();
    const Uint32 sndlength = mpCurrentSndSlot->getSoundlength();    

    if ((mSoundPtr + len) >= sndlength)
	{
		// Fill up the buffer and the rest with silence
        const Uint32 len_left = sndlength-mSoundPtr;
        memcpy(waveform, snddata + mSoundPtr, len_left );
		memset(waveform+len_left, m_AudioSpec.silence, len-len_left );
        mSoundPtr = 0;
        mSoundPlaying = false;
	}
	else
	{
        memcpy(waveform, snddata + mSoundPtr, len );
        mSoundPtr += len;
	}

	if(m_AudioSpec.channels == 2)
	{
		if(m_AudioSpec.format == AUDIO_U16 || m_AudioSpec.format == AUDIO_S16)
        {
			transintoStereoChannels((Sint16*) (void *) waveform, len);
        }
		else
        {
			transintoStereoChannels(waveform, len);
        }
	}
}
