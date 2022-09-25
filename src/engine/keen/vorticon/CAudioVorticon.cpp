/*
 * CAudioVorticon.cpp
 *
 *  Created on: 22.01.2011
 *      Author: gerstrong
 */

#include "CAudioVorticon.h"
#include "fileio/ResourceMgmt.h"
#include "engine/keen/GameSound.h"

#include <base/GsLogging.h>
#include <base/utils/FindFile.h>
#include <fileio/KeenFiles.h>
#include <fstream>
#include <cmath>


/**
 * This variables describes the possible maximum number of sounds the vorticon keen can have
 * As the mapping in vorticon is not as flexible we need that in order to be able to switch between hq sounds and
 * pc speaker sounds.
 */
const unsigned int MAX_NUM_SOUNDS = 50;


/**
	\brief This will load the Audio into the buffer from vorticon based keen games
	\param buffer_size  Size of the buffer which will be. Other functions need it to determine later when to stop mapping

	\return Returns the pointer the allocated and streamed buffer or NULL if nothing could be read at all
 */
void CAudioVorticon::loadSoundStream(Uint8* exedata)
{
    const std::string gamepath = gKeenFiles.gameDir;
    const std::string soundfile = "sounds.ck" + itoa(gKeenFiles.exeFile.getEpisode());
	gLogging.ftextOut("loadSoundStream(): trying to open the game audio...<br>");

	std::ifstream file;
	if(!OpenGameFileR(file, getResourceFilename(soundfile, gamepath, false, true), std::ios::binary))
	{
        const int ep = gKeenFiles.exeFile.getEpisode();
        Uint32 sounds_start=0, sounds_end=0;

        // if not available try to extract it.
        // In Episode 2 and 3 this an usual case.
        if (ep == 2)
		{
		    sounds_start  = 0x12730;
		    sounds_end    = 0x14BDA;
		}
        else if (ep == 3)
		{
		    sounds_start  = 0x13A70;
		    sounds_end    = 0x164D4;
		}
		else
		{
            gLogging.ftextOut("Warning: I cannot extract sounds from that game. Please provide a \"sounds.ck%d\" for that game.", gKeenFiles.exeFile.getEpisode());
		}

        const Uint32 buffer_size = sounds_end-sounds_start;
        mFileBuffer.resize(buffer_size);

        memcpy((char*)mFileBuffer.data(), exedata+sounds_start, buffer_size);
	}
	else
	{
		file.seekg(0, std::ios::end);
        const Uint32 buffer_size = file.tellg();
		file.seekg(0, std::ios::beg);

        mFileBuffer.resize(buffer_size);

        file.read((char*)mFileBuffer.data(), buffer_size*sizeof(Uint8) );
	}
}

/**
 * Load the PC Speaker sound and pass it as waveform directly.
 */
template <typename T>
bool CAudioVorticon::loadPCSpeakerSound(std::vector<T> &waveform, const std::string& searchname,
									bool IsSigned, Uint16& Priority)
{
    const auto PC_Speaker_Volume = gAudio.getPCSpeakerVol();
	int curheader = 0x10;
	word offset;
	//int garbage,
	int	nr_of_sounds;
	char name[12];

	memset(name,0,12);
    const Uint32 buf_size = mFileBuffer.size();
    gs_byte *buffer = mFileBuffer.data();
    gs_byte *buf_ptr = buffer+0x6;

    const auto &audioSpec = gAudio.getAudioSpec();

	nr_of_sounds = READWORD(buf_ptr);

	bool mayContinue=true;

	for(int j=0; mayContinue ; j++)
	{
		buf_ptr = buffer+curheader;
		offset = READWORD(buf_ptr);
		Priority = *buf_ptr++;
		buf_ptr++;

        for(int i=0;i<12;i++)
            name[i] = *buf_ptr++;

        const std::string nameStr = name;

        if (searchname == nameStr)
		{
			buf_ptr = buffer+offset;
            auto *beekseekPtr = buf_ptr;

            // Discover how many beeps exist
            int numBeeps = 0;
            while(1)
            {
                const int test = READWORD(beekseekPtr);

                if(test == 0xffff)
                    break;

                numBeeps++;
            }

			const int AMP = ((IsSigned ? ((1<<(sizeof(T)*8))>>2)-1 : (1<<(sizeof(T)*8)>>1)-1)*PC_Speaker_Volume)/100;
            const unsigned int wavetime = (audioSpec.freq*1000)/145575;

            // Allocate the required memory for the Wave
            waveform.assign(audioSpec.channels*wavetime*numBeeps, audioSpec.silence);

            generateWave((gs_byte*)waveform.data(), sizeof(T), wavetime, buf_ptr, numBeeps, true, AMP, audioSpec);
            gLogging.ftextOut("CAudioVorticon::loadSound: loaded sound %s into the waveform.<br>", searchname.c_str());

			return true;
		}
		curheader += 0x10;

		mayContinue = j<nr_of_sounds;
		const Uint32 buffDiff = buf_ptr-buffer;
		mayContinue |= (buffDiff < buf_size);
	}
	// sound could not be found
    gLogging.ftextOut("CAudioVorticon::loadSound: sound \"%s\" could not be found.<br>", searchname.c_str());

	return false;
}

// loads sound searchname from file fname, into sounds[] entry loadnum
// return value is false on failure
bool CAudioVorticon::loadSound( const std::string& path,
                                const std::string& searchname,
                                const GameSound loadnum)
{
    CSoundSlot &current_snd_slot = m_soundslot[int(loadnum)];
    CSoundSlot &current_hq_snd_slot = m_soundslot[int(loadnum)+MAX_NUM_SOUNDS];

	current_snd_slot.unload();
	current_hq_snd_slot.unload();

	// If a high quality sound file is available, try to open it.
	// Otherwise open the classic sounds from the original data files
    Uint8* buf = nullptr;
	bool ok = false;

    const Uint16 format = gAudio.getAudioSpec().format;

    if( format == AUDIO_S8 )
	{
		std::vector<Sint8> waveform;
        ok = loadPCSpeakerSound(waveform, searchname, false, current_snd_slot.priority);
		buf = (Uint8*)&waveform[0];
		current_snd_slot.setupWaveForm( buf, waveform.size()*sizeof(Sint8) );
	}
    else if( format == AUDIO_U8 )
	{
		std::vector<Uint8> waveform;
        ok = loadPCSpeakerSound(waveform, searchname, true, current_snd_slot.priority);
		buf = (Uint8*)&waveform[0];
		current_snd_slot.setupWaveForm( buf, waveform.size()*sizeof(Uint8) );
	}
    else if( format == AUDIO_U16 )
	{
		std::vector<Uint16> waveform;
        ok = loadPCSpeakerSound(waveform, searchname, false, current_snd_slot.priority);
		buf = (Uint8*)&waveform[0];
		current_snd_slot.setupWaveForm( buf, waveform.size()*sizeof(Uint16) );
	}
    else if( format == AUDIO_S16 )
	{
		std::vector<Sint16> waveform;
        ok = loadPCSpeakerSound(waveform, searchname, true, current_snd_slot.priority);
		buf = (Uint8*)&waveform[0];
		current_snd_slot.setupWaveForm( buf,  waveform.size()*sizeof(Sint16) );
	}

    current_hq_snd_slot.HQSndLoad(path,
                                  searchname);

	return ok;

}

/**
 *  \brief Sets up the Map assignments for the Sounds used in Keen Vorticon
 */
void CAudioVorticon::setupAudioMap()
{ 
    auto mapIt = [&](const GameSound snd, const int value) {
        sndSlotMap[int(snd)] = value;
    };

    mapIt(GameSound::KEEN_WALK,0);

    mapIt(GameSound::KEEN_WALK2,1);
    mapIt(GameSound::KEEN_JUMP,2);
    mapIt(GameSound::KEEN_POGO,3);
    mapIt(GameSound::KEEN_DIE,4);
    mapIt(GameSound::KEEN_FALL,5);
    mapIt(GameSound::KEEN_BUMPHEAD,6);
    mapIt(GameSound::KEENSLEFT,7);
    mapIt(GameSound::KEEN_FIRE,8);
    mapIt(GameSound::GUN_CLICK,9);
    mapIt(GameSound::SHOT_HIT,10);
    mapIt(GameSound::GET_ITEM,11);
    mapIt(GameSound::GET_PART,12);
    mapIt(GameSound::LEVEL_DONE,13);
    mapIt(GameSound::GAME_OVER,14);
    mapIt(GameSound::TELEPORT,15);
    mapIt(GameSound::EXTRA_LIFE,16);
//    mapIt(?,17);
    mapIt(GameSound::CANNONFIRE,18);
    mapIt(GameSound::CHUNKSMASH,19);
//    mapIt(?,20);
    mapIt(GameSound::GET_CARD,21);
//    mapIt(?,22);
    mapIt(GameSound::DOOR_OPEN,23);
    mapIt(GameSound::YORP_BUMP,24);
    mapIt(GameSound::YORP_STUN,25);
    mapIt(GameSound::YORP_DIE,26);
    mapIt(GameSound::GARG_DIE,27);
    mapIt(GameSound::VORT_DIE,28);
    mapIt(GameSound::KEEN_LAND,29);
    mapIt(GameSound::GET_BONUS,30);
    mapIt(GameSound::ENTER_LEVEL,31);
    mapIt(GameSound::SWITCH_TOGGLE,32);
    mapIt(GameSound::EARTHPOW,33);
    mapIt(GameSound::TANK_FIRE,34);
    mapIt(GameSound::KEEN_BLOK,35);
    mapIt(GameSound::MEEP,36);
    mapIt(GameSound::ANKH,37);
    mapIt(GameSound::MORTIMER,38);
    mapIt(GameSound::FOOTSLAM,39);
}


bool CAudioVorticon::loadSoundData(const unsigned int dictOffset)
{
    setupAudioMap();

	bool ok = true;
    const int episode = gKeenFiles.exeFile.getEpisode();
    const std::string datadir = gKeenFiles.gameDir;

	gLogging.ftextOut("loadSoundData(): loading all sounds...<br>");

    loadSoundStream( gKeenFiles.exeFile.getRawData() );
	CSoundSlot zeroslot;
	m_soundslot.assign(2*MAX_NUM_SOUNDS, zeroslot);

    ok  = loadSound( datadir, "WLDWALKSND", GameSound::WLD_WALK);
    ok &= loadSound( datadir, "KEENWALKSND", GameSound::KEEN_WALK);
    ok &= loadSound( datadir, "KEENWLK2SND", GameSound::KEEN_WALK2);
    ok &= loadSound( datadir, "KEENJUMPSND", GameSound::KEEN_JUMP);
    ok &= loadSound( datadir, "KEENPOGOSND", GameSound::KEEN_POGO);
    ok &= loadSound( datadir, "KEENLANDSND", GameSound::KEEN_LAND);
    ok &= loadSound( datadir, "KEENBLOKSND", GameSound::KEEN_BLOK);
    ok &= loadSound( datadir, "KEENDIESND", GameSound::KEEN_DIE);
    ok &= loadSound( datadir, "PLUMMETSND", GameSound::KEEN_FALL);
    ok &= loadSound( datadir, "BUMPHEADSND", GameSound::KEEN_BUMPHEAD);
    ok &= loadSound( datadir, "WLDENTERSND", GameSound::ENTER_LEVEL);
    ok &= loadSound( datadir, "keensleft", GameSound::KEENSLEFT);

    ok &= loadSound( datadir, "KEENFIRESND", GameSound::KEEN_FIRE);
    ok &= loadSound( datadir, "GUNCLICK", GameSound::GUN_CLICK);
    ok &= loadSound( datadir, "SHOTHIT", GameSound::SHOT_HIT);

    ok &= loadSound( datadir, "GOTITEMSND", GameSound::GET_ITEM);
    ok &= loadSound( datadir, "GOTBONUSSND", GameSound::GET_BONUS);
    ok &= loadSound( datadir, "GOTPARTSND", GameSound::GET_PART);
    ok &= loadSound( datadir, "LVLDONESND", GameSound::LEVEL_DONE);
    ok &= loadSound( datadir, "GAMEOVERSND", GameSound::GAME_OVER);
    ok &= loadSound( datadir, "TELEPORTSND", GameSound::TELEPORT);
    ok &= loadSound( datadir, "EXTRAMANSND", GameSound::EXTRA_LIFE);
    ok &= loadSound( datadir, "CANNONFIRE", GameSound::CANNONFIRE);
    ok &= loadSound( datadir, "CHUNKSMASH", GameSound::CHUNKSMASH);
    ok &= loadSound( datadir, "GOINDOORSND", GameSound::GOINDOOR);
    ok &= loadSound( datadir, "GETCARDSND", GameSound::GET_CARD);
    ok &= loadSound( datadir, "USEKEYSND", GameSound::USE_KEY);
    ok &= loadSound( datadir, "CLICKSND", GameSound::SWITCH_TOGGLE);
    ok &= loadSound( datadir, "DOOROPENSND", GameSound::DOOR_OPEN);

    ok &= loadSound( datadir, "YORPBUMPSND", GameSound::YORP_BUMP);
    ok &= loadSound( datadir, "YORPBOPSND", GameSound::YORP_STUN);
    ok &= loadSound( datadir, "YORPSCREAM", GameSound::YORP_DIE);
    ok &= loadSound( datadir, "GARGSCREAM", GameSound::GARG_DIE);
    ok &= loadSound( datadir, "vortscream", GameSound::VORT_DIE);
    ok &= loadSound( datadir, "TANKFIRE", GameSound::TANK_FIRE);

	if (episode == 2)
	{
        ok &= loadSound( datadir, "EARTHPOW", GameSound::EARTHPOW);
	}
	else if (episode == 3)
	{
        ok &= loadSound( datadir, "MEEP", GameSound::MEEP);
        ok &= loadSound( datadir, "ANKH", GameSound::ANKH);
        ok &= loadSound( datadir, "MORTIMER", GameSound::MORTIMER);
        ok &= loadSound( datadir, "FOOTSLAM", GameSound::FOOTSLAM);
	}

    if(!mFileBuffer.empty())
    {
        mFileBuffer.clear();
    }

	return ok;
}

void CAudioVorticon::unloadSound()
{
	if(m_soundslot.empty())
		return;
	std::vector<CSoundSlot>::iterator it = m_soundslot.begin();
	for( ; it != m_soundslot.end() ; it++ )
		it->unload();
	m_soundslot.clear();
}
