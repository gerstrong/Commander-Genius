/*
 * CAudioGalaxy.cpp
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#include "CAudioGalaxy.h"
#include "CLogFile.h"
#include "FindFile.h"
#include "fileio/ResourceMgmt.h"
#include <fstream>
#include "sdl/sound/CSound.h"
#include "fileio/compression/CHuffman.h"

CAudioGalaxy::CAudioGalaxy(const CExeFile &ExeFile, const SDL_AudioSpec &AudioSpec) :
CAudioResources(AudioSpec),
m_ExeFile(ExeFile)
{}

/**
 * Caution: This is Galaxy only and will be replaced
 * This function loads the PC Speaker sounds to CG (Galaxy Version, similar to Vorticon Version but not equal.)
 */
bool CAudioGalaxy::readPCSpeakerSoundintoWaveForm(CSoundSlot &soundslot, const byte *pcsdata, const unsigned int bytesize, const Uint8 formatsize)
{
	byte *pcsdata_ptr = (byte*)pcsdata;
	const longword size = READLONGWORD(pcsdata_ptr);
	soundslot.priority = READWORD(pcsdata_ptr);
	soundslot.setupAudioSpec(&m_AudioSpec);

	std::vector<Sint16> waveform;
	// There should be a better way of determining if sound is signed or not...
	int AMP;
	if ((m_AudioSpec.format == AUDIO_S8) || (m_AudioSpec.format == AUDIO_S16))
		AMP = ((((1<<(formatsize*8))>>2)-1)*PC_Speaker_Volume)/100;
	else
		AMP = ((((1<<(formatsize*8))>>1)-1)*PC_Speaker_Volume)/100;
	//int AMP = ((IsSigned ? ((1<<(formatsize*8))>>2)-1 : (1<<(formatsize*8)>>1)-1)*PC_Speaker_Volume)/100;

	generateWave(waveform, pcsdata_ptr, size, false, AMP);

	if(formatsize == 1)
	{
		std::vector<Uint8> wave8;
		std::vector<Sint16>::iterator it = waveform.begin();
		for( ; it != waveform.end(); it++ )
			wave8.push_back(*it);
		soundslot.setupWaveForm((Uint8*)&wave8[0], wave8.size()*sizeof(Uint8));
	}
	else
		soundslot.setupWaveForm((Uint8*)&waveform[0], waveform.size()*sizeof(Sint16));


	return true;
}

/**
 * \brief 	This function will load teh sounds using other dictionaries which are embedded in the Exe File.
 * 			Only galaxy supports that feature, and the original games will read two files form the EXE-file
 * 			AUDIOHED and AUDIODICT to get the sounds.
 * 			Caution: CMusic Class has a function which is similar but only loads the music from one level.
 */
bool CAudioGalaxy::LoadFromAudioCK(const CExeFile& ExeFile)
{
	if(m_AudioSpec.format != 0)
	{
		// Open the Huffman dictionary and get AUDIODICT
		CHuffman Huffman;
		Huffman.readDictionaryNumber( ExeFile, 0 );

		/// First get the size of the AUDIO.CK? File.
		uint32_t audiofilecompsize;
		std::string init_audiofilename = gpResource->audioFilename;

		std::string audiofilename = getResourceFilename( init_audiofilename, ExeFile.getDataDirectory(), true, false);

		if( audiofilename == "" )
		{
			g_pLogFile->textOut("CAudioGalaxy::LoadFromAudioCK(): Audio File not found!");
			return false;
		}

		std::ifstream AudioFile;
		OpenGameFileR(AudioFile, audiofilename);

		// Read File Size and allocate memory so we can read it
		AudioFile.seekg( 0, std::ios::end );
		audiofilecompsize = AudioFile.tellg();
		AudioFile.seekg( 0, std::ios::beg );

		// create memory so we can store the Audio.ck there and use it later for extraction
		//uint8_t *AudioCompFileData = new uint8_t[audiofilecompsize];
		uint8_t *AudioCompFileData = new uint8_t[audiofilecompsize];
		AudioFile.read((char*)AudioCompFileData, audiofilecompsize);
		AudioFile.close();

		// Open the AUDIOHED so we know where to decompress
		uint32_t number_of_audiorecs = 0;

		uint32_t *audiohedptr = reinterpret_cast<uint32_t*>(ExeFile.getHeaderData());
		bool found = false;
		for( const uint32_t *endptr = (uint32_t*) (void*) ExeFile.getHeaderData() + ExeFile.getExeDataSize()/sizeof(uint32_t);
				audiohedptr < endptr ;
				audiohedptr++ )
		{
			if(*audiohedptr == audiofilecompsize)
			{
				for( const uint32_t *startptr = (uint32_t*) (void*) ExeFile.getHeaderData() ;
						audiohedptr > startptr ; audiohedptr-- )
				{
					found = true;
					// Get the number of Audio files we have
					number_of_audiorecs++;
					if(*audiohedptr == 0x0)
						break;
				}
				break;
			}
		}

		if(!found)
		{
			g_pLogFile->textOut("CAudioGalaxy::LoadFromAudioCK(): No audio was found in that file! It seems to be empty.");
			delete [] AudioCompFileData;
			return false;
		}


		// Find the start of the embedded IMF files
		uint32_t outsize = 0;
		uint32_t al_snd_start = 0;
		uint32_t number_of_total_sounds = 0;

		for(  ; number_of_total_sounds<number_of_audiorecs ; number_of_total_sounds++ )
		{
			const uint32_t audio_start = audiohedptr[number_of_total_sounds];
			const uint32_t audio_end = audiohedptr[number_of_total_sounds+1];

			if(audio_start == audio_end)
			{
				al_snd_start = number_of_total_sounds/2;
				break;
			}
		}

		CSoundSlot zeroslot;
		m_soundslot.assign(number_of_total_sounds, zeroslot);
		
		for( unsigned int snd=0 ; snd<number_of_total_sounds ; snd++ )
		{
			/// Now we have all the data we need.
			// decompress every file of AUDIO.CK? using huffman decompression algorithm
			const uint32_t audio_start = audiohedptr[snd];
			const uint32_t audio_end = audiohedptr[snd+1];

			if( audio_start < audio_end )
			{
				const uint32_t audio_comp_data_start = audio_start+sizeof(uint32_t);
				const uint32_t *AudioCompFileData32 = reinterpret_cast<uint32_t*>(
														reinterpret_cast<void*>(AudioCompFileData + audio_start));

				outsize = *AudioCompFileData32;
				byte imfdata[outsize];

				Huffman.expand( (byte*)(AudioCompFileData+audio_comp_data_start), imfdata, audio_end-audio_comp_data_start, outsize);

				if(snd>=al_snd_start)
					readISFintoWaveForm( m_soundslot[snd], imfdata, outsize, (m_AudioSpec.format == AUDIO_S16) ? 2 : 1 );
				else
					readPCSpeakerSoundintoWaveForm( m_soundslot[snd], imfdata, outsize, (m_AudioSpec.format == AUDIO_S16) ? 2 : 1 );
			}
		}

		delete [] AudioCompFileData;
	}
	else
		g_pLogFile->textOut("CAudioGalaxy::LoadFromAudioCK(): Wrong Audio Format");

	return true;
}

/**
 * Main load function for the galaxy audio
 */
bool CAudioGalaxy::loadSoundData()
{
    COPLEmulator &OPLEmulator = *g_pSound->getOPLEmulatorPtr();
    
    OPLEmulator.shutdown();
    OPLEmulator.init();
    
	const bool ok = LoadFromAudioCK(m_ExeFile);

	if(!ok)
	{
		g_pLogFile->textOut("CAudioGalaxy::loadSoundData(): The function cannot read audio of that game");
		return false;
	}

	return true;
}

/**
 * Will free the resources of the audio
 */
void CAudioGalaxy::unloadSound()
{

}
