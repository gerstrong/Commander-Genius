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
 *  \brief Sets up the Map assignments for the Sounds used in Keen Galaxy
 */
void CAudioGalaxy::setupAudioMap()
{     
    // Episode 4    
    sndSlotMapGalaxy[4][SOUND_KEEN_WALK] = 0;
    sndSlotMapGalaxy[4][SOUND_KEEN_WALK2] = 1;
    sndSlotMapGalaxy[4][SOUND_KEEN_JUMP] = 2;
    sndSlotMapGalaxy[4][SOUND_KEEN_LAND] = 3;
    sndSlotMapGalaxy[4][SOUND_KEEN_FIRE] = 4; 
    sndSlotMapGalaxy[4][SOUND_WORMOUTH_STRIKE] = 5;
    // sndSlotMapGalaxy[4][?] = 6; // This sounds like alternative breathe under water. not sure
    sndSlotMapGalaxy[4][SOUND_KEEN_POGO] = 7;
    sndSlotMapGalaxy[4][SOUND_GET_BONUS] = 8;
    sndSlotMapGalaxy[4][SOUND_GET_AMMO] = 9;
    sndSlotMapGalaxy[4][SOUND_GET_DROP] = 10;
    sndSlotMapGalaxy[4][SOUND_GET_ITEM] = 11;
    sndSlotMapGalaxy[4][SOUND_ENTER_LEVEL] = 12;
    sndSlotMapGalaxy[4][SOUND_LEVEL_DONE] = 13;
    sndSlotMapGalaxy[4][SOUND_CANT_DO] = 14;
    sndSlotMapGalaxy[4][SOUND_KEEN_BUMPHEAD] = 15;
    sndSlotMapGalaxy[4][SOUND_BOUNCE_HIGH] = 16;
    sndSlotMapGalaxy[4][SOUND_EXTRA_LIFE] = 17;
    sndSlotMapGalaxy[4][SOUND_DOOR_OPEN] = 18;    
    sndSlotMapGalaxy[4][SOUND_GET_GEM] = 19;
    sndSlotMapGalaxy[4][SOUND_KEEN_FALL] = 20;
    sndSlotMapGalaxy[4][SOUND_GUN_CLICK] = 21;    
    sndSlotMapGalaxy[4][SOUND_SQUISH_SKYPEST] = 22;
    sndSlotMapGalaxy[4][SOUND_KEEN_DIE] = 23;
    //sndSlotMapGalaxy[4][?] = 24;
    sndSlotMapGalaxy[4][SOUND_SHOT_HIT] = 25;
    sndSlotMapGalaxy[4][SOUND_KEEN_SWIM] = 26;    
    sndSlotMapGalaxy[4][SOUND_KEEN_SWIM_TO_LAND] = 27;
    sndSlotMapGalaxy[4][SOUND_BOUNCE_LOW] = 28;
    //sndSlotMapGalaxy[4][?] = 29;
    sndSlotMapGalaxy[4][SOUND_TRESURE_STEALER_TELEPORT] = 30;
    sndSlotMapGalaxy[4][SOUND_RESCUE_COUNCIL_MEMBER] = 31;
    sndSlotMapGalaxy[4][SOUND_LICK_FIREBREATH] = 32;
    sndSlotMapGalaxy[4][SOUND_BERKELOID_WINDUP] = 33;
    sndSlotMapGalaxy[4][SOUND_STATUS_SLIDE_IN] = 34;
    sndSlotMapGalaxy[4][SOUND_STATUS_SLIDE_OUT] = 35;
    sndSlotMapGalaxy[4][SOUND_BUBBLE] = 36;
    sndSlotMapGalaxy[4][SOUND_MINE_EXPLOSION] = 37;
    sndSlotMapGalaxy[4][SOUND_SPRITE_SHOT] = 38;
    sndSlotMapGalaxy[4][SOUND_THUNDERCLOUD_STRIKE] = 39;
    sndSlotMapGalaxy[4][SOUND_BERKELOID_FIREBALL_LANDING] = 40;    
    sndSlotMapGalaxy[4][SOUND_DARTGUN_SHOOT] = 41;
    sndSlotMapGalaxy[4][SOUND_DOPEFISH_BURP] = 42;    
    sndSlotMapGalaxy[4][SOUND_FLAG_APPEAR] = 43;
    sndSlotMapGalaxy[4][SOUND_FLAG_LAND] = 44;    
    sndSlotMapGalaxy[4][SOUND_GET_WETSUIT] = 45;    
    sndSlotMapGalaxy[4][SOUND_SLUG_DEFECATE] = 46;
    sndSlotMapGalaxy[4][SOUND_PLAYER_PADDLE] = 47;
    sndSlotMapGalaxy[4][SOUND_COMPUTER_PADDLE] = 48;
    sndSlotMapGalaxy[4][SOUND_HIT_SIDEWALL] = 49;
    sndSlotMapGalaxy[4][SOUND_COMPUTER_POINT] = 50;
    sndSlotMapGalaxy[4][SOUND_PLAYER_POINT] = 51;
        
    
    // Episode 5   
    sndSlotMapGalaxy[5][SOUND_KEEN_WALK] = 0;
    sndSlotMapGalaxy[5][SOUND_KEEN_WALK2] = 1;
    sndSlotMapGalaxy[5][SOUND_KEEN_JUMP] = 2;
    sndSlotMapGalaxy[5][SOUND_KEEN_LAND] = 3;
    sndSlotMapGalaxy[5][SOUND_KEEN_FIRE] = 4; 
    //sndSlotMapGalaxy[5][SOUND_MINEEXPLODE] = 5;
    //sndSlotMapGalaxy[5][SOUND_KEEN_POGO_LAND] = 6;
    sndSlotMapGalaxy[5][SOUND_KEEN_POGO] = 7;
    sndSlotMapGalaxy[5][SOUND_GET_BONUS] = 8;
    sndSlotMapGalaxy[5][SOUND_GET_AMMO] = 9;
    sndSlotMapGalaxy[5][SOUND_GET_DROP] = 10;
    sndSlotMapGalaxy[5][SOUND_GET_ITEM] = 11;
    sndSlotMapGalaxy[5][SOUND_ENTER_LEVEL] = 12;
    sndSlotMapGalaxy[5][SOUND_LEVEL_DONE] = 13;
    sndSlotMapGalaxy[5][SOUND_CANT_DO] = 14;
    sndSlotMapGalaxy[5][SOUND_KEEN_BUMPHEAD] = 15;
    //sndSlotMapGalaxy[5][SOUND_SPINDREDFLYUP] = 16;
    sndSlotMapGalaxy[5][SOUND_EXTRA_LIFE] = 17;
    sndSlotMapGalaxy[5][SOUND_OPEN_EXIT_DOOR] = 18;    
    sndSlotMapGalaxy[5][SOUND_GET_GEM] = 19;
    sndSlotMapGalaxy[5][SOUND_KEEN_FALL] = 20;
    sndSlotMapGalaxy[5][SOUND_GUN_CLICK] = 21;    
    //sndSlotMapGalaxy[5][?] = 22;
    sndSlotMapGalaxy[5][SOUND_KEEN_DIE] = 23;
    //sndSlotMapGalaxy[5][?] = 24;
    sndSlotMapGalaxy[5][SOUND_SHOT_HIT] = 25;
    //sndSlotMapGalaxy[5][?] = 26;    
    //sndSlotMapGalaxy[5][SOUND_SPIROGRIP] = 27;
    //sndSlotMapGalaxy[5][SOUND_SPINDREDSLAM] = 28;
    sndSlotMapGalaxy[5][SOUND_ROBORED_SHOOT] = 29;
    //sndSlotMapGalaxy[5][SOUND_ROBOSHOTHIT] = 30;
    sndSlotMapGalaxy[5][SOUND_AMPTONWALK0] = 31;
    sndSlotMapGalaxy[5][SOUND_AMPTONWALK1] = 32;
    sndSlotMapGalaxy[5][SOUND_ROBO_STUN] = 33;
    sndSlotMapGalaxy[5][SOUND_STATUS_SLIDE_IN] = 34;
    sndSlotMapGalaxy[5][SOUND_STATUS_SLIDE_OUT] = 35;
    sndSlotMapGalaxy[5][SOUND_SPARKY_CHARGE] = 36;
    //sndSlotMapGalaxy[5][SOUND_SPINDREDFLYDOWN] = 37;
    //sndSlotMapGalaxy[5][SOUND_MASTERSHOT] = 38;
    //sndSlotMapGalaxy[5][SOUND_MASTERTELE] = 39;
    //sndSlotMapGalaxy[5][SOUND_POLEZAP] = 40;    
    sndSlotMapGalaxy[5][SOUND_TELEPORT] = 41;
    //sndSlotMapGalaxy[5][SOUND_SHOCKSUNDBARK] = 42;    
    sndSlotMapGalaxy[5][SOUND_FLAG_APPEAR] = 43;
    sndSlotMapGalaxy[5][SOUND_FLAG_LAND] = 44;    
    //sndSlotMapGalaxy[5][ACTION_BARKSHOTDIE0] = 45;    
    //sndSlotMapGalaxy[5][SOUND_PLAYER_PADDLE] = 46;
    sndSlotMapGalaxy[5][SOUND_COMPUTER_PADDLE] = 47;
    sndSlotMapGalaxy[5][SOUND_HIT_SIDEWALL] = 48;
    sndSlotMapGalaxy[5][SOUND_COMPUTER_POINT] = 49;
    sndSlotMapGalaxy[5][SOUND_PLAYER_POINT] = 50;
    //sndSlotMapGalaxy[5][?] = 51;
    //sndSlotMapGalaxy[5][?] = 52;
    //sndSlotMapGalaxy[5][?] = 53;
    //sndSlotMapGalaxy[5][?] = 54;
    sndSlotMapGalaxy[5][SOUND_GET_CARD] = 55;
    sndSlotMapGalaxy[5][SOUND_ELEVATING] = 56;
    //sndSlotMapGalaxy[5][SOUND_ELEVATOR_OPEN] = 57;
    //sndSlotMapGalaxy[5][?] = 58;
    //sndSlotMapGalaxy[5][SOUND_SPHEREFULCEILING] = 59;
    sndSlotMapGalaxy[5][SOUND_DOOR_OPEN] = 60;
    //sndSlotMapGalaxy[5][SOUND_SPIROFLY] = 61;
    //sndSlotMapGalaxy[5][?] = 62;
    sndSlotMapGalaxy[5][SOUND_ELEVATOR_OPEN] = 63;
    
    
    // Episode 6    
    sndSlotMapGalaxy[6][SOUND_KEEN_WALK] = 0;
    sndSlotMapGalaxy[6][SOUND_KEEN_WALK2] = 1;
    sndSlotMapGalaxy[6][SOUND_KEEN_JUMP] = 2;
    sndSlotMapGalaxy[6][SOUND_KEEN_LAND] = 3;
    sndSlotMapGalaxy[6][SOUND_KEEN_FIRE] = 4; 
    //sndSlotMapGalaxy[6][?] = 5;
    // sndSlotMapGalaxy[4][?] = 6; // This sounds like alternative breathe under water. not sure
    sndSlotMapGalaxy[6][SOUND_KEEN_POGO] = 7;
    sndSlotMapGalaxy[6][SOUND_GET_BONUS] = 8;
    sndSlotMapGalaxy[6][SOUND_GET_AMMO] = 9;
    sndSlotMapGalaxy[6][SOUND_GET_DROP] = 10;
    sndSlotMapGalaxy[6][SOUND_GET_ITEM] = 11;
    sndSlotMapGalaxy[6][SOUND_ENTER_LEVEL] = 12;
    sndSlotMapGalaxy[6][SOUND_LEVEL_DONE] = 13;
    sndSlotMapGalaxy[6][SOUND_CANT_DO] = 14;
    sndSlotMapGalaxy[6][SOUND_KEEN_BUMPHEAD] = 15;
    sndSlotMapGalaxy[6][SOUND_BOUNCE_HIGH] = 16;
    sndSlotMapGalaxy[6][SOUND_EXTRA_LIFE] = 17;
    sndSlotMapGalaxy[6][SOUND_DOOR_OPEN] = 18;    
    sndSlotMapGalaxy[6][SOUND_GET_GEM] = 19;
    sndSlotMapGalaxy[6][SOUND_KEEN_FALL] = 20;
    sndSlotMapGalaxy[6][SOUND_GUN_CLICK] = 21;    
    //sndSlotMapGalaxy[6][?] = 22;
    sndSlotMapGalaxy[6][SOUND_KEEN_DIE] = 23;
    //sndSlotMapGalaxy[6][?] = 24;
    sndSlotMapGalaxy[6][SOUND_SHOT_HIT] = 25;
    //sndSlotMapGalaxy[6][?] = 26;    
    //sndSlotMapGalaxy[6][?] = 27;
    //sndSlotMapGalaxy[6][?] = 28;
    //sndSlotMapGalaxy[6][?] = 29;
    //sndSlotMapGalaxy[6][?] = 30;
    //sndSlotMapGalaxy[6][?] = 31;
    //sndSlotMapGalaxy[6][?] = 32;
    //sndSlotMapGalaxy[6][?] = 33;
    sndSlotMapGalaxy[6][SOUND_STATUS_SLIDE_IN] = 34;
    sndSlotMapGalaxy[6][SOUND_STATUS_SLIDE_OUT] = 35;
    //sndSlotMapGalaxy[6][?] = 36;
    //sndSlotMapGalaxy[6][?] = 37;
    //sndSlotMapGalaxy[6][?] = 38;
    //sndSlotMapGalaxy[6][?] = 39;
    //sndSlotMapGalaxy[6][?] = 40;    
    //sndSlotMapGalaxy[6][?] = 41;
    //sndSlotMapGalaxy[6][?] = 42;    
    sndSlotMapGalaxy[6][SOUND_FLAG_APPEAR] = 43;
    sndSlotMapGalaxy[6][SOUND_FLAG_LAND] = 44;    
    //sndSlotMapGalaxy[6][?] = 45;    
    //sndSlotMapGalaxy[6][?] = 46;
    sndSlotMapGalaxy[6][SOUND_PLAYER_PADDLE] = 47;
    sndSlotMapGalaxy[6][SOUND_COMPUTER_PADDLE] = 48;
    sndSlotMapGalaxy[6][SOUND_HIT_SIDEWALL] = 49;
    sndSlotMapGalaxy[6][SOUND_COMPUTER_POINT] = 50;
    sndSlotMapGalaxy[6][SOUND_PLAYER_POINT] = 51;
    
}


/**
 * \brief 	This function will load the sounds using other dictionaries which are embedded in the Exe File.
 * 			Only galaxy supports that feature, and the original games will read two files from the EXE-file
 * 			AUDIOHED and AUDIODICT to get the sounds.
 * 			Caution: CMusic Class has a function which is similar but only loads the music from one level.
 */
bool CAudioGalaxy::LoadFromAudioCK(const CExeFile& ExeFile)
{
    setupAudioMap();
    
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
		uint8_t *AudioCompFileData = new uint8_t[audiofilecompsize];
		AudioFile.read((char*)AudioCompFileData, audiofilecompsize);
		AudioFile.close();

		// Open the AUDIOHED so we know where to decompress the audio
		
		std::string audiohedfilename = gpResource->audioHedFilename;
		audiohedfilename = getResourceFilename( audiohedfilename, ExeFile.getDataDirectory(), false, false);				
		
		uint32_t *audiostarthedptr;
		uint32_t *audioendhedptr;
				
		std::vector<uint32_t> audiohed;
		
		if(audiohedfilename != "") 
		{
		    std::ifstream File; 
		    OpenGameFileR(File, audiohedfilename, std::ios::binary);
		    
		    File.seekg (0, std::ios::end);
		    size_t length = File.tellg();
		    File.seekg (0, std::ios::beg);
		    
		    audiohed.resize(length/sizeof(u_int));
		    
		    File.read( (char*) &(audiohed.front()), length);		    
		}
		else // no file found? Use the embedded one!
		{
		    audiostarthedptr = reinterpret_cast<uint32_t*>(ExeFile.getHeaderData());
		    audioendhedptr = audiostarthedptr + ExeFile.getExeDataSize()/sizeof(uint32_t);
		    
		    uint32_t *audiohedptr = audiostarthedptr;
		    uint32_t number_of_audiorecs = 0;
		    for( ; audiohedptr < audioendhedptr ; audiohedptr++ )
		    {
			if(*audiohedptr == audiofilecompsize)
			{
				for( const uint32_t *startptr = (uint32_t*) (void*) ExeFile.getHeaderData() ;
						audiohedptr > startptr ; audiohedptr-- )
				{
					// Get the number of Audio files we have
					number_of_audiorecs++;
					if(*audiohedptr == 0x0)
						break;
				}
				break;
			}
		    }
		    
		    for(size_t i=0 ; i<number_of_audiorecs ; i++)
		    {
			audiohed.push_back(*audiohedptr);
			audiohedptr++;
		    }
		}	


		if(audiohed.empty())
		{
			g_pLogFile->textOut("CAudioGalaxy::LoadFromAudioCK(): No audio was found in that file! It seems to be empty.");
			delete [] AudioCompFileData;
			return false;
		}


		// Find the start of the embedded IMF files
		uint32_t outsize = 0;
		uint32_t al_snd_start = 0;
		uint32_t number_of_total_sounds = 0;

		for(  ; number_of_total_sounds<audiohed.size() ; number_of_total_sounds++ )
		{
			const uint32_t audio_start = audiohed[number_of_total_sounds];
			const uint32_t audio_end = audiohed[number_of_total_sounds+1];

			if(audio_start == audio_end)
			{
				al_snd_start = number_of_total_sounds/2;
				break;
			}
		}

		m_soundslot.assign(number_of_total_sounds, CSoundSlot());
		
		for( unsigned int snd=0 ; snd<number_of_total_sounds ; snd++ )
		{
			/// Now we have all the data we need.
			// decompress every file of AUDIO.CK? using huffman decompression algorithm
			const uint32_t audio_start = audiohed[snd];
			const uint32_t audio_end = audiohed[snd+1];

			const uint32_t audio_comp_data_start = audio_start+sizeof(uint32_t);
			if( audio_comp_data_start < audio_end )
			{				
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
