/*
 * CAudioResources.cpp
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#include "CAudioResources.h"

#include "fileio/ResourceMgmt.h"
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>
#include "Audio.h"

CAudioResources::CAudioResources()
{}

bool CAudioResources::readISFintoWaveForm( CSoundSlot &soundslot, const byte *imfdata, const Uint8 formatsize )
{
	byte *imfdata_ptr = (byte*)imfdata;
	const longword size = READLONGWORD(imfdata_ptr);
	soundslot.priority = READWORD(imfdata_ptr);
    COPLEmulator &OPLEmulator = g_pSound->getOPLEmulatorRef();

	// It's time make it Adlib Sound structure and read it into the waveform
	AdLibSound AL_Sound = *((AdLibSound*) imfdata_ptr);
	imfdata_ptr += sizeof(AdLibSound);
	const unsigned int data_size = size;
	const byte *AL_Sounddata_start = imfdata_ptr;
	const byte *AL_Sounddata_end = AL_Sounddata_start+data_size;

	OPLEmulator.ShutAL();
	Bit8u alBlock = ((AL_Sound.block & 7) << 2) | 0x20;
	if (!(AL_Sound.inst.mSus | AL_Sound.inst.cSus))
	{
		// TODO: Bad instrument. Please tell that also...
		return false;
	}
	OPLEmulator.AlSetFXInst(AL_Sound.inst);

    const SDL_AudioSpec &audioSpec = g_pSound->getAudioSpec();


    const unsigned int samplesPerMusicTick = audioSpec.freq/OPLEmulator.getIMFClockRate();
	const unsigned waittimes = 4;
    const unsigned int wavesize = (data_size*waittimes*samplesPerMusicTick*audioSpec.channels*formatsize );
	byte waveform[wavesize];
	byte *waveform_ptr = waveform;
	Bit32s mix_buffer[samplesPerMusicTick];

	OPLEmulator.ALStopSound();

	// TODO: This does not work correctly yet...

	for(byte *AL_Sounddata_ptr = (byte*) AL_Sounddata_start ;
			  AL_Sounddata_ptr < AL_Sounddata_end ;
			  AL_Sounddata_ptr++ )
	{
		if(*AL_Sounddata_ptr)
		{
			OPLEmulator.Chip__WriteReg( alFreqL, *AL_Sounddata_ptr );
			OPLEmulator.Chip__WriteReg( alFreqH, alBlock );
		}
		else
			OPLEmulator.Chip__WriteReg( alFreqH, 0 );

   		if(formatsize == 2) // 16-Bit Sound
   		{
   			for( size_t count=0 ; count<waittimes ; count++ )
   			{
   				Sint16 *buffer = (Sint16*) (void*) waveform_ptr;

   				OPLEmulator.Chip__GenerateBlock2( samplesPerMusicTick, mix_buffer );

   				// Mix into the destination buffer, doubling up into stereo.
   				for (unsigned int i=0; i<samplesPerMusicTick; ++i)
   				{
                    for( unsigned int ch=0 ; ch<audioSpec.channels ; ch++ )
				    {
                        buffer[i * audioSpec.channels + ch] = (int16_t) (mix_buffer[i]+audioSpec.silence);
				    }
   				}

                waveform_ptr += samplesPerMusicTick*audioSpec.channels*formatsize;
   			}
   		}
   		else // 8-Bit Sound
   		{
   			for( unsigned int count=0 ; count<waittimes ; count++ )
   			{
   				Uint8 *buffer = (Uint8*) waveform_ptr;

   				OPLEmulator.Chip__GenerateBlock2( samplesPerMusicTick, mix_buffer );

   				// Mix into the destination buffer, doubling up into stereo.
   				for (unsigned int i=0; i<samplesPerMusicTick; ++i)
   				{
                    for( unsigned int ch=0 ; ch<audioSpec.channels ; ch++ )
				    {
                    buffer[i * audioSpec.channels + ch] = (Uint8) ((mix_buffer[i]>>8)+audioSpec.silence);
				    }
   				}

                waveform_ptr += samplesPerMusicTick*audioSpec.channels*formatsize;
   			}
   		}
	}

	soundslot.setupWaveForm(waveform, wavesize);


	return true;
}




void CAudioResources::generateBeep(byte *waveform,
                                   word sample,
                                   word sampleSize,
                                   int wavesample,
                                   Uint64 &freqtimer,
                                   const int AMP,
                                   const int silence,
                                   const int channels,
                                   const unsigned int wavetime,
                                   const int frequency)
{
    unsigned int offset = 0;

    const int low  = silence - AMP;
    const int high = silence + AMP;

    const Uint64 changerate = (frequency>>1)*Uint64(sample);

    for (unsigned int j=0; j<wavetime; j++)
    {
        if (sample != 0)
        {
            if (freqtimer > changerate)
            {
                freqtimer %= changerate;

                wavesample = (wavesample == low) ? high : low;
            }
            freqtimer += PCSpeakerTime;
        }

        // For all the channel set this value
        for(int i=0 ; i<channels ; i++)
        {
            memcpy(&waveform[offset], &wavesample, sampleSize);
            offset += sampleSize;
        }
    }

}


void CAudioResources::generateWave(byte* waveform,
                                   const int waveSampleSize,
                                   const unsigned int wavetime,
                                   byte *inBuffer,
                                   unsigned int numOfBeeps,
                                   bool isVorticons,
                                   const int& AMP,
                                   const SDL_AudioSpec &audioSpec)
{
    /** If PC_SPEAKER_WORKS_LIKE_DOSBOX_V0_74 is defined, we attempt
     * to simulate the way vanilla DOSBox v0.74 emulates the PC Speaker.
     * Might be useful for some Commander Keen packs with alternate sounds effects.
     */
    Uint64 freqtimer = 0;
    word prevsample = 0, sample;
    const int silence = audioSpec.silence;
    const int channels = audioSpec.channels;
    int wave = silence - AMP;

    unsigned int offset = 0;

    for(unsigned pos=0 ; pos<numOfBeeps ; pos++)
    {
        if (isVorticons)
        {
            sample = READWORD(inBuffer);

            if(sample == 0xffff)
                break;


#ifdef PC_SPEAKER_WORKS_LIKE_DOSBOX_V0_74
            if (prevsample != 0)
                freqtimer %= audioSpec.freq*prevsample;
#else
            // On Keen 1-3, separated consecutive samples are always separated.
            wave = silence - AMP;
            freqtimer = 0;
#endif
        }
        else
        {
            // Multiplying by some constant (60 in our case) seems to reproduces the right sound.
            sample = *(inBuffer++) * 60;
#ifdef PC_SPEAKER_WORKS_LIKE_DOSBOX_V0_74
            if (prevsample != 0)
                freqtimer %= audioSpec.freq*prevsample;
#else
            /** On Keen 4-6, consecutive samples of the exact
                 * same frequency are merged into a single tone.
                 */
            if (prevsample != sample)
            {
                wave = silence - AMP;
                freqtimer = 0;
            }
#endif

        }

        generateBeep(&waveform[offset],
                     sample, waveSampleSize, wave,
                     freqtimer, AMP,
                     silence, channels,
                     wavetime, audioSpec.freq);
        prevsample = sample;

        offset += (channels*wavetime*waveSampleSize);
        memcpy(&wave, &waveform[offset-waveSampleSize], waveSampleSize);
    }
}

