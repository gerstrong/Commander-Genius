/*
 * CAudioGalaxy.cpp
 *
 *  Created on: 23.01.2011
 *      Author: gerstrong
 */

#include "CAudioGalaxy.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/compression/CHuffman.h"
#include "fileio/KeenFiles.h"
#include "engine/core/CBehaviorEngine.h"

#include <engine/keen/GameSound.h>
#include <base/GsLogging.h>
#include <base/interface/FindFile.h>
#include <base/audio/Audio.h>
#include <fstream>

/**
 * Caution: This is Galaxy only and will be replaced some time
 * This function loads the PC Speaker sounds to CG (Galaxy Version, similar to Vorticon Version but not equal.)
 */
bool CAudioGalaxy::readPCSpeakerSoundintoWaveForm(CSoundSlot &soundslot,
                                                  const gs_byte *pcsdata,
                                                  const Uint8 formatsize)
{
    const auto PC_Speaker_Volume = gAudio.getPCSpeakerVol();

    gs_byte *pcsdata_ptr = const_cast<gs_byte*>(pcsdata);
	const longword size = READLONGWORD(pcsdata_ptr);
	soundslot.priority = READWORD(pcsdata_ptr);

    const SDL_AudioSpec &audioSpec = gAudio.getAudioSpec();

	std::vector<Sint16> waveform;
	// TODO:  There should be a better way of determining if sound is signed or not...
	int AMP;
    if ((audioSpec.format == AUDIO_S8) || (audioSpec.format == AUDIO_S16))
		AMP = ((((1<<(formatsize*8))>>2)-1)*PC_Speaker_Volume)/100;
	else
		AMP = ((((1<<(formatsize*8))>>1)-1)*PC_Speaker_Volume)/100;

    const unsigned int wavetime = (audioSpec.freq*1000)/140026;

    /** Effective number of samples is actually size-1.
     * Reason: The vanilla way, right after beginning the very last sample output,
     * it's stopped. (That should be validated in some way...)
     */
    // Allocate the required memory for the Wave
    const int numBeeps = size-1;

    if(numBeeps >= 1)
    {
        waveform.assign(audioSpec.channels*wavetime*numBeeps, audioSpec.silence);

        generateWave((gs_byte*)waveform.data(), sizeof(Sint16), wavetime, pcsdata_ptr, numBeeps, false, AMP, audioSpec);

        if(formatsize == 1)
        {
            std::vector<Uint8> wave8;
            std::vector<Sint16>::iterator it = waveform.begin();
            for( ; it != waveform.end(); it++ )
            {
                wave8.push_back(*it);
            }

            soundslot.setupWaveForm((Uint8*)&wave8[0], wave8.size()*sizeof(Uint8));
        }
        else
        {
            soundslot.setupWaveForm((Uint8*)&waveform[0], waveform.size()*sizeof(Sint16));
        }

    }

    return true;
}


/**
 *  \brief Sets up the Map assignments for the Sounds used in Keen Galaxy
 */
void CAudioGalaxy::setupAudioMap()
{
    const auto episode = gBehaviorEngine.getEpisode();

    auto mapIt = [&](const int ep, const GameSound snd, const int value) {
        sndSlotMapGalaxy[ep][int(snd)] = value;
    };

    // Episode 4
    mapIt(4, GameSound::KEEN_WALK, 0);
    mapIt(4, GameSound::KEEN_WALK2, 1);
    mapIt(4, GameSound::KEEN_JUMP, 2);
    mapIt(4, GameSound::KEEN_LAND, 3);
    mapIt(4, GameSound::KEEN_FIRE, 4);
    mapIt(4, GameSound::WORMOUTH_STRIKE, 5);
    //mapIt(4, ???, 6);
    mapIt(4, GameSound::KEEN_POGO, 7);
    mapIt(4, GameSound::GET_BONUS, 8);
    mapIt(4, GameSound::GET_AMMO, 9);
    mapIt(4, GameSound::GET_DROP, 10);
    mapIt(4, GameSound::GET_ITEM, 11);
    mapIt(4, GameSound::ENTER_LEVEL, 12);
    mapIt(4, GameSound::LEVEL_DONE, 13);
    mapIt(4, GameSound::CANT_DO, 14);
    mapIt(4, GameSound::KEEN_BUMPHEAD, 15);
    mapIt(4, GameSound::BOUNCE_HIGH, 16);
    mapIt(4, GameSound::EXTRA_LIFE, 17);
    mapIt(4, GameSound::DOOR_OPEN, 18);
    mapIt(4, GameSound::GET_GEM, 19);
    mapIt(4, GameSound::KEEN_FALL, 20);
    mapIt(4, GameSound::GUN_CLICK, 21);
    mapIt(4, GameSound::SQUISH_SKYPEST, 22);
    mapIt(4, GameSound::KEEN_DIE, 23);
    //mapIt(4, , 24);
    mapIt(4, GameSound::SHOT_HIT, 25);
    mapIt(4, GameSound::KEEN_SWIM, 26);
    mapIt(4, GameSound::KEEN_SWIM_TO_LAND, 27);
    mapIt(4, GameSound::BOUNCE_LOW, 28);
    //mapIt(4, , 29);
    mapIt(4, GameSound::TRESURE_STEALER_TELEPORT, 30);
    mapIt(4, GameSound::RESCUE_COUNCIL_MEMBER, 31);
    mapIt(4, GameSound::LICK_FIREBREATH, 32);
    mapIt(4, GameSound::BERKELOID_WINDUP, 33);
    mapIt(4, GameSound::STATUS_SLIDE_IN, 34);
    mapIt(4, GameSound::STATUS_SLIDE_OUT, 35);
    mapIt(4, GameSound::BUBBLE, 36);
    mapIt(4, GameSound::MINE_EXPLOSION, 37);
    mapIt(4, GameSound::SPRITE_SHOT, 38);
    mapIt(4, GameSound::THUNDERCLOUD_STRIKE, 39);
    mapIt(4, GameSound::BERKELOID_FIREBALL_LANDING, 40);
    mapIt(4, GameSound::DARTGUN_SHOOT, 41);
    mapIt(4, GameSound::DOPEFISH_BURP, 42);
    mapIt(4, GameSound::FLAG_APPEAR, 43);
    mapIt(4, GameSound::FLAG_LAND, 44);
    mapIt(4, GameSound::GET_WETSUIT, 45);
    mapIt(4, GameSound::SLUG_DEFECATE, 46);
    mapIt(4, GameSound::PLAYER_PADDLE, 47);
    mapIt(4, GameSound::COMPUTER_PADDLE, 48);
    mapIt(4, GameSound::HIT_SIDEWALL, 49);
    mapIt(4, GameSound::COMPUTER_POINT, 50);
    mapIt(4, GameSound::PLAYER_POINT, 51);

    if(episode == 5) // Episode 5
    {
        mapIt(5, GameSound::KEEN_WALK, 0);
        mapIt(5, GameSound::KEEN_WALK2, 1);
        mapIt(5, GameSound::KEEN_JUMP, 2);
        mapIt(5, GameSound::KEEN_LAND, 3);
        mapIt(5, GameSound::KEEN_FIRE, 4);
        mapIt(5, GameSound::MINEEXPLODE, 5);
        mapIt(5, GameSound::SLICEBUMP, 6);
        mapIt(5, GameSound::KEEN_POGO, 7);
        mapIt(5, GameSound::GET_BONUS, 8);
        mapIt(5, GameSound::GET_AMMO, 9);
        mapIt(5, GameSound::GET_DROP, 10);
        mapIt(5, GameSound::GET_ITEM, 11);
        mapIt(5, GameSound::ENTER_LEVEL, 12);
        mapIt(5, GameSound::LEVEL_DONE, 13);
        mapIt(5, GameSound::CANT_DO, 14);
        mapIt(5, GameSound::KEEN_BUMPHEAD, 15);
        mapIt(5, GameSound::SPINDREDFLYUP, 16);
        mapIt(5, GameSound::EXTRA_LIFE, 17);
        mapIt(5, GameSound::OPEN_EXIT_DOOR, 18);
        mapIt(5, GameSound::GET_GEM, 19);
        mapIt(5, GameSound::KEEN_FALL, 20);
        mapIt(5, GameSound::GUN_CLICK, 21);
        mapIt(5, GameSound::SCREAM_LOUD, 22);
        mapIt(5, GameSound::KEEN_DIE, 23);
        mapIt(5, GameSound::SCREAM_LESS, 24);
        mapIt(5, GameSound::SHOT_HIT, 25);
        mapIt(5, GameSound::SCRAMBLE, 26);
        mapIt(5, GameSound::SPIROGRIP, 27);
        mapIt(5, GameSound::SPINDREDSLAM, 28);
        mapIt(5, GameSound::ROBORED_SHOOT, 29);
        mapIt(5, GameSound::ROBORED_SHOOT2, 30);

      // TODO: I think this came from Keen 9. Needs some rework.
      //memcpy(&holder, ptr + 0x112DE, 1 );
        //mapIt(5, GameSound::AMPTONWALK0, holder);
        mapIt(5, GameSound::AMPTONWALK0, 31);

      // TODO: I think this came from Keen 9. Needs some rework.
      //memcpy(&holder, ptr + 0x112F3, 1 );
        //mapIt(5, GameSound::AMPTONWALK1, holder);
        mapIt(5, GameSound::AMPTONWALK1, 32);

        mapIt(5, GameSound::ROBO_STUN, 33);
        mapIt(5, GameSound::STATUS_SLIDE_IN, 34);
        mapIt(5, GameSound::STATUS_SLIDE_OUT, 35);
        mapIt(5, GameSound::SPARKY_CHARGE, 36);

      // TODO: I think this came from Keen 9. Needs some rework.
      //memcpy(&holder, ptr + 0x10F93, 1 );
        //mapIt(5, GameSound::SPINDREDFLYDOWN, holder);
        mapIt(5, GameSound::SPINDREDFLYDOWN, 37);
        mapIt(5, GameSound::MASTERSHOT, 38);
        mapIt(5, GameSound::MASTERTELE, 39);
        mapIt(5, GameSound::POLEZAP, 40);
        mapIt(5, GameSound::TELEPORT, 41);
        mapIt(5, GameSound::SHOCKSUNDBARK, 42);
        mapIt(5, GameSound::FLAG_APPEAR, 43);
        mapIt(5, GameSound::FLAG_LAND, 44);

        //mapIt(5, ACTION_BARKSHOTDIE0, 45);
        //mapIt(5, GameSound::PLAYER_PADDLE, 46);
        mapIt(5, GameSound::COMPUTER_PADDLE, 47);
        mapIt(5, GameSound::HIT_SIDEWALL, 48);
        mapIt(5, GameSound::COMPUTER_POINT, 49);
        mapIt(5, GameSound::PLAYER_POINT, 50);
        //mapIt(5, ?, 51);
        mapIt(5, GameSound::FUSE_BREAK, 52);
        mapIt(5, GameSound::BIG_GAMEOVER, 53);
        //mapIt(5, ?, 54);
        mapIt(5, GameSound::GET_CARD, 55);

      // TODO: I think this came from Keen 9. Needs some rework.
      //memcpy(&holder, ptr + 0x13784, 1 );
        //mapIt(5, GameSound::ELEVATING, holder);
        mapIt(5, GameSound::ELEVATING, 56);
        //mapIt(5, GameSound::ELEVATOR_OPEN, 57);
        //mapIt(5, GameSound::ELEVATOR_OPEN, 58);
        mapIt(5, GameSound::SPHEREFULCEILING, 59);
        mapIt(5, GameSound::DOOR_OPEN, 60);
        mapIt(5, GameSound::SPIROFLY, 61);
        //mapIt(5, ?, 62);
        mapIt(5, GameSound::ELEVATOR_OPEN, 63);
    }

    // Episode 6
    mapIt(6, GameSound::KEEN_WALK, 0);
    mapIt(6, GameSound::KEEN_WALK2, 1);
    mapIt(6, GameSound::KEEN_JUMP, 2);
    mapIt(6, GameSound::KEEN_LAND, 3);
    mapIt(6, GameSound::KEEN_FIRE, 4);
    mapIt(6, GameSound::JUMPED_GEM, 5);
    mapIt(6, GameSound::KEEN_LAND_ALT, 6);
    mapIt(6, GameSound::KEEN_POGO, 7);
    mapIt(6, GameSound::GET_BONUS, 8);
    mapIt(6, GameSound::GET_AMMO, 9);
    mapIt(6, GameSound::GET_DROP, 10);
    mapIt(6, GameSound::GET_ITEM, 11);
    mapIt(6, GameSound::ENTER_LEVEL, 12);
    mapIt(6, GameSound::LEVEL_DONE, 13);
    mapIt(6, GameSound::CANT_DO, 14);
    mapIt(6, GameSound::KEEN_BUMPHEAD, 15);
    mapIt(6, GameSound::BOUNCE_HIGH, 16);
    mapIt(6, GameSound::EXTRA_LIFE, 17);
    mapIt(6, GameSound::DOOR_OPEN, 18);
    mapIt(6, GameSound::GET_GEM, 19);
    mapIt(6, GameSound::KEEN_FALL, 20);
    mapIt(6, GameSound::GUN_CLICK, 21);
    mapIt(6, GameSound::SQUISH, 22);
    mapIt(6, GameSound::KEEN_DIE, 23);
    mapIt(6, GameSound::BIP_SHIP_CRASH, 24);
    mapIt(6, GameSound::SHOT_HIT, 25);
    mapIt(6, GameSound::CEILICK_LICK, 26);
    mapIt(6, GameSound::BABOBBA_JUMP, 27);
    mapIt(6, GameSound::BABOBBA_LAND, 28);
    mapIt(6, GameSound::AUTOGUN, 29);
    //mapIt(6, ?, 30);
    mapIt(6, GameSound::BOBBA_LAND, 31);
    mapIt(6, GameSound::BOBBA_SHOOT, 32);
    mapIt(6, GameSound::SATELITE_MOUNT, 33);
    mapIt(6, GameSound::STATUS_SLIDE_IN, 34);
    mapIt(6, GameSound::STATUS_SLIDE_OUT, 35);
    //mapIt(6, ?, 36);
    //mapIt(6, ?, 37);
    mapIt(6, GameSound::ORBATRIX_BUMP, 38);
//    mapIt(6, GameSound::LASER_SHOT, 39);
//    mapIt(6, GameSound::BIG_LASER_SHOT, 40);
    mapIt(6, GameSound::TELEPORT, 41);
//    mapIt(6, GameSound::SPRING, 42);
    mapIt(6, GameSound::FLAG_APPEAR, 43);
    mapIt(6, GameSound::FLAG_LAND, 44);
    mapIt(6, GameSound::GET_SPECIAL_ITEM, 45);
    mapIt(6, GameSound::PLAYER_PADDLE, 46);
    mapIt(6, GameSound::COMPUTER_PADDLE, 47);
    mapIt(6, GameSound::HIT_SIDEWALL, 48);
    mapIt(6, GameSound::COMPUTER_POINT, 49);
    mapIt(6, GameSound::PLAYER_POINT, 50);
    mapIt(6, GameSound::GRABBITER_SLEEP, 51);
    mapIt(6, GameSound::KEEN_STUNNED, 52);
    mapIt(6, GameSound::ROPE_THROW, 53);
    mapIt(6, GameSound::ROCKET_DRIVE, 54);
    mapIt(6, GameSound::CEILICK_LAUGH, 55);
    mapIt(6, GameSound::ROCKET_LAUNCH, 56);
    mapIt(6, GameSound::GRABBITER_HUNGRY, 57);
    mapIt(6, GameSound::BLOOGGUARD_STUB, 58);
    mapIt(6, GameSound::BABOBBA_CINDER, 59);

    // Keen Dreams. Refkeen has predefined placeholder, so we just set counter for now...
    for(int i=0 ; i<28 ; i++)
    {
        mapIt(7, GameSound(i), i);
    }

    // Lua script. If you have a snd/sndmap this one will be taken and overwrite
    // some mappings
    auto fullFName = JoinPaths(gKeenFiles.gameDir ,"snd");
    fullFName = JoinPaths(fullFName, "sndmap.lua");

    bool ok = mLua.loadFile( fullFName );

    if(!ok)
        return;

    auto &slotmap = sndSlotMapGalaxy[episode];

    for(auto &slot : slotmap)
    {
        const auto curSnd = int(slot.first);
        int snd = slot.second;
        mLua.runFunctionRetOneInt("getSoundMapping", curSnd, snd);
        if(snd >= 0)
        {
            slot.second = snd;
        }
    }
}


/**
 * \brief 	This function will load the sounds using other dictionaries which are embedded in the Exe File.
 * 			Only galaxy supports that feature, and the original games will read two files from the EXE-file
 * 			AUDIOHED and AUDIODICT to get the sounds addresses from there.
 * 			Caution: CMusic Class has a function which is similar but only loads the music from one level.
 */
bool CAudioGalaxy::LoadFromAudioCK(const unsigned int dictOffset)
{
    const CExeFile &ExeFile = gKeenFiles.exeFile;

    gLogging.ftextOut("CAudioGalaxy::LoadFromAudioCK(): Setting up Audio map... <br>");
    setupAudioMap();

    const SDL_AudioSpec &audioSpec = gAudio.getAudioSpec();

    if(audioSpec.format == 0)
    {
        gLogging.textOut("CAudioGalaxy::LoadFromAudioCK(): Wrong format of the audio device. Cannot load the audio!");
        return false;
    }

    // Open the Huffman dictionary and get AUDIODICT
    gLogging.ftextOut("CAudioGalaxy::LoadFromAudioCK(): Preparing Huffman decompression...<br>");
    CHuffman Huffman;

    std::string audioDictfilename = getResourceFilename( gKeenFiles.audioDictFilename, gKeenFiles.gameDir, false, false);

    if(audioDictfilename.empty())
        Huffman.readDictionaryNumber( ExeFile, 0, dictOffset );
    else
        Huffman.readDictionaryFromFile( audioDictfilename );

    /// First get the size of the AUDIO.CK? File.
    std::string init_audiofilename = gKeenFiles.audioFilename;

    std::string audiofilename = getResourceFilename( init_audiofilename, gKeenFiles.gameDir, true, false);

    if( audiofilename == "" )
    {
        gLogging.textOut("CAudioGalaxy::LoadFromAudioCK(): Audio File not found!");
        return false;
    }

    std::ifstream AudioFile;
    OpenGameFileR(AudioFile, audiofilename);
    gLogging.ftextOut("CAudioGalaxy::LoadFromAudioCK(): Reading %s ...<br>", audiofilename.c_str());

    // Read File Size and allocate memory so we can read it
    AudioFile.seekg( 0, std::ios::end );
    const auto audiofilecompsize = uint32_t(AudioFile.tellg());
    AudioFile.seekg( 0, std::ios::beg );

    // create memory so we can store the Audio.ck there and use it later for extraction
    //uint8_t *AudioCompFileData = new uint8_t[audiofilecompsize];

    std::vector<uint8_t> AudioCompFileData;
    AudioCompFileData.resize(audiofilecompsize, 0);

    AudioFile.read(reinterpret_cast<char*>(AudioCompFileData.data()),
                   audiofilecompsize);
    AudioFile.close();

    // Open the AUDIOHED so we know where to decompress the audio

    std::string audiohedfilename = gKeenFiles.audioHedFilename;
    audiohedfilename =
            getResourceFilename( audiohedfilename,
                                 gKeenFiles.gameDir, false, false);

    uint32_t *audioendhedptr;

    std::vector<uint32_t> audiohed;

    if(audiohedfilename != "")
    {
        gLogging.ftextOut("CAudioGalaxy::LoadFromAudioCK(): Using external %s ...<br>", audiohedfilename.c_str());
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
        gLogging.ftextOut("CAudioGalaxy::LoadFromAudioCK(): No header file. Looking for internal audio pointers ...<br>");

        uint32_t *audiostarthedptr = reinterpret_cast<uint32_t*>(ExeFile.getHeaderData());
        audioendhedptr = audiostarthedptr + ExeFile.getExeDataSize()/sizeof(uint32_t);

        uint32_t *audiohedptr = audiostarthedptr;
        uint32_t number_of_audiorecs = 0;
        for( ; audiohedptr < audioendhedptr ; audiohedptr++ )
        {
            if(*audiohedptr == audiofilecompsize)
            {
                for( const uint32_t *startptr = audiostarthedptr ;
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

        // PATCH: Keen Dreams Plus. For some reason the second slot has an invalid offset
        if(audiohed[1] == 0xFF)
        {
            for(unsigned int i=1 ; i<number_of_audiorecs ; i++)
            {
                audiohed[i] = audiohed[i+1];
            }

            // And since we have one slot less, reduce the total amount
            number_of_audiorecs--;
            audiohed.resize(number_of_audiorecs);
        }

    }


    if(audiohed.empty())
    {
        gLogging.textOut("CAudioGalaxy::LoadFromAudioCK(): No audio was found in that file! It seems to be empty.");
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

    gLogging.ftextOut("CAudioGalaxy::LoadFromAudioCK(): Found a total of %d sound effects ...<br>", number_of_total_sounds);
    m_soundslot.assign(number_of_total_sounds, CSoundSlot());


    bool mustDehuffman = false;

    // Test, if we need to apply hufmann decompression. If out and in sizes are the same,
    // it won't be the case. This happens usually with mods.
    for( unsigned int snd=0 ; snd<number_of_total_sounds ; snd++ )
    {
        const uint32_t audio_start = audiohed[snd];
        const uint32_t audio_end = audiohed[snd+1];

        // Why this strange offset by 4 bytes?
        const uint32_t audio_comp_data_start = audio_start+sizeof(uint32_t);

        if( audio_comp_data_start < audio_end )
        {
            const uint32_t *AudioCompFileData32 = reinterpret_cast<uint32_t*>(
                        reinterpret_cast<void*>(AudioCompFileData.data() + audio_start));

            outsize = *AudioCompFileData32;

            const unsigned long insize = audio_end-audio_comp_data_start;

            if(outsize != insize)
            {
                mustDehuffman = true;
                break;
            }
        }
    }

    int succReadSounds = 0;

    for( unsigned int snd=0 ; snd<number_of_total_sounds ; snd++ )
    {
        /// Now we have all the data we need.
        // decompress every file of AUDIO.CK? using huffman decompression algorithm
        const uint32_t audio_start = audiohed[snd];
        const uint32_t audio_end = audiohed[snd+1];

        const uint32_t audio_comp_data_start = audio_start+sizeof(uint32_t); // Why this strange offset by 4 bytes?

        std::vector<gs_byte> imfdata;        

        if( audio_comp_data_start < audio_end )
        {
            const uint32_t *AudioCompFileData32 = reinterpret_cast<uint32_t*>(
                        reinterpret_cast<void*>(AudioCompFileData.data() + audio_start));

            outsize = *AudioCompFileData32;
            imfdata.resize(outsize);

            gs_byte *imfdataPtr = reinterpret_cast<gs_byte *>(imfdata.data());

            const unsigned long insize = audio_end-audio_comp_data_start;

            // Same size, just copy then
            if(!mustDehuffman)
            {
                memcpy(imfdataPtr,
                       reinterpret_cast<gs_byte*>(&AudioCompFileData[audio_comp_data_start]),
                       insize);
            }
            else // uncompress!
            {
                Huffman.expand( reinterpret_cast<gs_byte*>(&AudioCompFileData[audio_comp_data_start]),
                                imfdataPtr,
                                insize,
                                outsize);
            }


            if(snd>=al_snd_start)
            {
                const bool ok = readISFintoWaveForm( m_soundslot[snd],
                                                     imfdataPtr,
                                                     (audioSpec.format == AUDIO_S16) ? 2 : 1 );
                if(!ok)
                    gLogging << "Sound " << snd << " could not be read!<br>";
                else
                    succReadSounds++;
            }
            else
            {
                const bool ok = readPCSpeakerSoundintoWaveForm( m_soundslot[snd],
                                                                imfdataPtr,
                                                                (audioSpec.format == AUDIO_S16) ? 2 : 1 );

                if(!ok)
                    gLogging << "Sound " << snd << " could not be read!<br>";
                else
                    succReadSounds++;
            }
        }
    }

    gLogging.ftextOut("CAudioGalaxy::LoadFromAudioCK(): Finished caching %d "
                      "sound effect(s) from "
                      "OPL and PC Speaker Emulator...<br>", succReadSounds);

    return true;
}

/**
 * Main load function for the galaxy audio
 */
bool CAudioGalaxy::loadSoundData(const unsigned int dictOffset)
{       
    COPLEmulator &OPLEmulator = gAudio.getOPLEmulatorRef();

    OPLEmulator.shutdown();
    gLogging.ftextOut("CAudioGalaxy::loadSoundData(): Initializing OPL Emulator... <br>");
    OPLEmulator.init();

    gLogging.ftextOut("CAudioGalaxy::loadSoundData(): Initializing OPL Emulator... <br>");
    const bool ok = LoadFromAudioCK(dictOffset);

	if(!ok)
	{
		gLogging.textOut("CAudioGalaxy::loadSoundData(): The function cannot read audio of that game");
		return false;
	}

	return true;
}

