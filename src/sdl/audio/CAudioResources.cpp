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

bool CAudioResources::readISFintoWaveForm( CSoundSlot &soundslot, const byte *imfdata, const unsigned int bytesize, const Uint8 formatsize )
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
