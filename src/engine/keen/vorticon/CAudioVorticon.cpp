/*
 * CAudioVorticon.cpp
 *
 *  Created on: 22.01.2011
 *      Author: gerstrong
 */

#include "CAudioVorticon.h"
#include <base/GsLogging.h>
#include <base/utils/FindFile.h>
#include "fileio/ResourceMgmt.h"
#include <fstream>
#include <math.h>

#include <fileio/KeenFiles.h>

/**
 * This variables describes the possible maximum number of sounds the vorticon keen can have
 * As the mapping in vorticon is not as flexible we need that in order to be able to switch between hq sounds and
 * pc speaker sounds.
 */
const unsigned int MAX_NUM_SOUNDS = 50;

CAudioVorticon::CAudioVorticon(const CExeFile &ExeFile) :
m_ExeFile(ExeFile)
{}

/**
	\brief This will load the Audio into the buffer from vorticon based keen games
	\param buffer_size  Size of the buffer which will be. Other functions need it to determine later when to stop mapping

	\return Returns the pointer the allocated and streamed buffer or NULL if nothing could be read at all
 */
void CAudioVorticon::loadSoundStream(Uint8* exedata)
{
    const std::string gamepath = gKeenFiles.gameDir;
	const std::string soundfile = "sounds.ck" + itoa(m_ExeFile.getEpisode());
	gLogging.ftextOut("loadSoundStream(): trying to open the game audio...<br>");

	std::ifstream file;
	if(!OpenGameFileR(file, getResourceFilename(soundfile, gamepath, false, true), std::ios::binary))
	{
        const int ep = m_ExeFile.getEpisode();
		Uint32 sounds_start, sounds_end;

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
			gLogging.ftextOut("Warning: I cannot extract sounds from that game. Please provide a \"sounds.ck%d\" for that game.", m_ExeFile.getEpisode());
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
	int curheader = 0x10;
	word offset;
	//int garbage,
	int	nr_of_sounds;
	char name[12];

	memset(name,0,12);
    const Uint32 buf_size = mFileBuffer.size();
    byte *buffer = mFileBuffer.data();
    byte *buf_ptr = buffer+0x6;

	nr_of_sounds = READWORD(buf_ptr);

	bool mayContinue=true;

	for(int j=0; mayContinue ; j++)
	{
		buf_ptr = buffer+curheader;
		offset = READWORD(buf_ptr);
		Priority = *buf_ptr++;
		buf_ptr++;
		//garbage = *buf_ptr++;

        for(int i=0;i<12;i++)
            name[i] = *buf_ptr++;

        const std::string nameStr = name;

        if (searchname == nameStr)
		{
			buf_ptr = buffer+offset;

			const int AMP = ((IsSigned ? ((1<<(sizeof(T)*8))>>2)-1 : (1<<(sizeof(T)*8)>>1)-1)*PC_Speaker_Volume)/100;
            generateWave(waveform, buf_ptr, 0, true, AMP, g_pSound->getAudioSpec());
			gLogging.ftextOut("CAudioVorticon::loadSound : loaded sound %s into the waveform.<br>", searchname.c_str());

			return true;
		}
		curheader += 0x10;

		mayContinue = j<nr_of_sounds;
		const Uint32 buffDiff = buf_ptr-buffer;
		mayContinue |= (buffDiff < buf_size);
	}
	// sound could not be found
	gLogging.ftextOut("CAudioVorticon::loadSound : sound \"%s\" could not be found.<br>", searchname.c_str());

	return false;
}

// loads sound searchname from file fname, into sounds[] entry loadnum
// return value is false on failure
bool CAudioVorticon::loadSound( const std::string& path, const std::string& searchname, unsigned int loadnum)
{
	CSoundSlot &current_snd_slot = m_soundslot[loadnum];
	CSoundSlot &current_hq_snd_slot = m_soundslot[loadnum+MAX_NUM_SOUNDS];

	current_snd_slot.unload();
	current_hq_snd_slot.unload();

	// If a high quality sound file is available, try to open it.
	// Otherwise open the classic sounds from the original data files
    Uint8* buf = nullptr;
	bool ok = false;

    const Uint16 format = g_pSound->getAudioSpec().format;

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

	current_hq_snd_slot.HQSndLoad(path, searchname);

	return ok;

}

/**
 *  \brief Sets up the Map assignments for the Sounds used in Keen Vorticon
 */
void CAudioVorticon::setupAudioMap()
{
    sndSlotMap[SOUND_KEEN_WALK] = 0;
    sndSlotMap[SOUND_KEEN_WALK2] = 1;
    sndSlotMap[SOUND_KEEN_JUMP] = 2;
    sndSlotMap[SOUND_KEEN_POGO] = 3;
    sndSlotMap[SOUND_KEEN_DIE] = 4;
    sndSlotMap[SOUND_KEEN_FALL] = 5;
    sndSlotMap[SOUND_KEEN_BUMPHEAD] = 6;
    sndSlotMap[SOUND_KEENSLEFT] = 7;
    sndSlotMap[SOUND_KEEN_FIRE] = 8;
    sndSlotMap[SOUND_GUN_CLICK] = 9;
    sndSlotMap[SOUND_SHOT_HIT] = 10;
    sndSlotMap[SOUND_GET_ITEM] = 11;
    sndSlotMap[SOUND_GET_PART] = 12;
    sndSlotMap[SOUND_LEVEL_DONE] = 13;
    sndSlotMap[SOUND_GAME_OVER] = 14;
    sndSlotMap[SOUND_TELEPORT] = 15;
    sndSlotMap[SOUND_EXTRA_LIFE] = 16;
    //sndSlotMap[?] = 17;
    sndSlotMap[SOUND_CANNONFIRE] = 18;
    sndSlotMap[SOUND_CHUNKSMASH] = 19;
    //sndSlotMap[?] = 20; // unused!
    sndSlotMap[SOUND_GET_CARD] = 21;
    //sndSlotMap[?] = 22;
    sndSlotMap[SOUND_DOOR_OPEN] = 23;
    sndSlotMap[SOUND_YORP_BUMP] = 24;
    sndSlotMap[SOUND_YORP_STUN] = 25;
    sndSlotMap[SOUND_YORP_DIE] = 26;
    sndSlotMap[SOUND_GARG_DIE] = 27;
    sndSlotMap[SOUND_VORT_DIE] = 28;
    sndSlotMap[SOUND_KEEN_LAND] = 29;
    sndSlotMap[SOUND_GET_BONUS] = 30;
    sndSlotMap[SOUND_ENTER_LEVEL] = 31;
    sndSlotMap[SOUND_SWITCH_TOGGLE] = 32;
    sndSlotMap[SOUND_EARTHPOW] = 33;
    sndSlotMap[SOUND_TANK_FIRE] = 34;
    sndSlotMap[SOUND_KEEN_BLOK] = 35;
    sndSlotMap[SOUND_MEEP] = 36;
    sndSlotMap[SOUND_ANKH] = 37;
    sndSlotMap[SOUND_MORTIMER] = 38;
    sndSlotMap[SOUND_FOOTSLAM] = 39;
}


bool CAudioVorticon::loadSoundData()
{
    setupAudioMap();

	bool ok = true;
	const int episode = m_ExeFile.getEpisode();
    const std::string datadir = gKeenFiles.gameDir;

	gLogging.ftextOut("loadSoundData(): loading all sounds...<br>");

    loadSoundStream( m_ExeFile.getRawData() );
	CSoundSlot zeroslot;
	m_soundslot.assign(2*MAX_NUM_SOUNDS, zeroslot);

    ok  = loadSound( datadir, "WLDWALKSND", SOUND_WLD_WALK);
    //ok  &= loadSound( DataDirectory, "WLDBLOCKSND", SOUND_WLD_BLOCK);
    ok &= loadSound( datadir, "KEENWALKSND", SOUND_KEEN_WALK);
    ok &= loadSound( datadir, "KEENWLK2SND", SOUND_KEEN_WALK2);
    ok &= loadSound( datadir, "KEENJUMPSND", SOUND_KEEN_JUMP);
    ok &= loadSound( datadir, "KEENPOGOSND", SOUND_KEEN_POGO);
    ok &= loadSound( datadir, "KEENLANDSND", SOUND_KEEN_LAND);
    ok &= loadSound( datadir, "KEENBLOKSND", SOUND_KEEN_BLOK);
    ok &= loadSound( datadir, "KEENDIESND", SOUND_KEEN_DIE);
    ok &= loadSound( datadir, "PLUMMETSND", SOUND_KEEN_FALL);
    ok &= loadSound( datadir, "BUMPHEADSND", SOUND_KEEN_BUMPHEAD);
    ok &= loadSound( datadir, "WLDENTERSND", SOUND_ENTER_LEVEL);
    ok &= loadSound( datadir, "keensleft", SOUND_KEENSLEFT);

    ok &= loadSound( datadir, "KEENFIRESND", SOUND_KEEN_FIRE);
    ok &= loadSound( datadir, "GUNCLICK", SOUND_GUN_CLICK);
    ok &= loadSound( datadir, "SHOTHIT", SOUND_SHOT_HIT);

    ok &= loadSound( datadir, "GOTITEMSND", SOUND_GET_ITEM);
    ok &= loadSound( datadir, "GOTBONUSSND", SOUND_GET_BONUS);
    ok &= loadSound( datadir, "GOTPARTSND", SOUND_GET_PART);
    ok &= loadSound( datadir, "LVLDONESND", SOUND_LEVEL_DONE);
    ok &= loadSound( datadir, "GAMEOVERSND", SOUND_GAME_OVER);
    ok &= loadSound( datadir, "TELEPORTSND", SOUND_TELEPORT);
    ok &= loadSound( datadir, "EXTRAMANSND", SOUND_EXTRA_LIFE);
    ok &= loadSound( datadir, "CANNONFIRE", SOUND_CANNONFIRE);
    ok &= loadSound( datadir, "CHUNKSMASH", SOUND_CHUNKSMASH);
    ok &= loadSound( datadir, "GOINDOORSND", SOUND_GOINDOOR);
    ok &= loadSound( datadir, "GETCARDSND", SOUND_GET_CARD);
    ok &= loadSound( datadir, "USEKEYSND", SOUND_USE_KEY);
    ok &= loadSound( datadir, "CLICKSND", SOUND_SWITCH_TOGGLE);
    ok &= loadSound( datadir, "DOOROPENSND", SOUND_DOOR_OPEN);

    ok &= loadSound( datadir, "YORPBUMPSND", SOUND_YORP_BUMP);
    ok &= loadSound( datadir, "YORPBOPSND", SOUND_YORP_STUN);
    ok &= loadSound( datadir, "YORPSCREAM", SOUND_YORP_DIE);
    ok &= loadSound( datadir, "GARGSCREAM", SOUND_GARG_DIE);
    ok &= loadSound( datadir, "vortscream", SOUND_VORT_DIE);
    ok &= loadSound( datadir, "TANKFIRE", SOUND_TANK_FIRE);

	if (episode == 2)
	{
        ok &= loadSound( datadir, "EARTHPOW", SOUND_EARTHPOW);
	}
	else if (episode == 3)
	{
        ok &= loadSound( datadir, "MEEP", SOUND_MEEP);
        ok &= loadSound( datadir, "ANKH", SOUND_ANKH);
        ok &= loadSound( datadir, "MORTIMER", SOUND_MORTIMER);
        ok &= loadSound( datadir, "FOOTSLAM", SOUND_FOOTSLAM);
	}

    if(!mFileBuffer.empty())
        mFileBuffer.clear();

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
