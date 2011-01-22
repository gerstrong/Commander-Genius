/*
 * CAudioVorticon.cpp
 *
 *  Created on: 22.01.2011
 *      Author: gerstrong
 */

#include "CAudioVorticon.h"
#include "CLogFile.h"
#include "FindFile.h"
#include "fileio/ResourceMgmt.h"
#include <fstream>

/**
	\brief This will load the Audio into the buffer from vorticon based keen games
	\param buffer_size  Size of the buffer which will be. Other functions need it to determine later when to stop mapping

	\return Returns the pointer the allocated and streamed buffer or NULL if nothing could be read at all
 */
Uint8* CAudioVorticon::loadSoundStream(Uint32 &buffer_size, Uint8* exedata)
{
	Uint8 *buffer = NULL;
	buffer_size = 0;

	const std::string soundfile = "sounds.ck" + itoa(m_Episode);
	g_pLogFile->ftextOut("sound_load_all(): trying to open the game audio...<br>");

	std::ifstream file;
	if(!OpenGameFileR(file, getResourceFilename(soundfile,m_DataDirectory,false,true), std::ios::binary))
	{
		Uint32 sounds_start, sounds_end;

		// if not available try to extract it
		if (m_Episode == 2)
		{
		    sounds_start  = 0x12730;
		    sounds_end    = 0x14BDA;
		}
		else if (m_Episode == 3)
		{
		    sounds_start  = 0x13A70;
		    sounds_end    = 0x164D4;
		}
		else
		{
			g_pLogFile->ftextOut("Warning: I cannot extract sounds from that game. Please provide a \"sounds.ck%d\" for that game.", m_Episode);
		    return NULL;
		}

		buffer_size = sounds_end-sounds_start;
		buffer = new Uint8[buffer_size];
		memcpy(buffer, exedata+sounds_start, buffer_size);

		return buffer;
	}
	else
	{
		file.seekg(0, std::ios::end);
		buffer_size = file.tellg();
		file.seekg(0, std::ios::beg);
		buffer = new Uint8[buffer_size];
		file.read((char*)buffer, buffer_size*sizeof(Uint8) );
		file.close();
		return buffer;
	}
}

bool CAudioVorticon::loadSoundData(CExeFile &ExeFile)
{
	if(!m_active) return false;

	bool ok = true;
	const std::string soundfile = "sounds.ck" + itoa(m_Episode);

	for(int i=0 ; i<MAX_SOUNDS ; i++)
	{
		m_soundslot[i].setpAudioSpec(&AudioSpec);
		m_soundslot[i].m_gamepath = m_DataDirectory;
	}

	g_pLogFile->ftextOut("sound_load_all(): loading all sounds...<br>");

	Uint32 buffer_size;
	Uint8 *buffer = loadSoundStream( buffer_size, ExeFile.getRawData() );

	ok  = m_soundslot[SOUND_KEEN_WALK].loadSound(buffer, buffer_size, m_DataDirectory, "KEENWALKSND", SOUND_KEEN_WALK);
	ok &= m_soundslot[SOUND_KEEN_WALK2].loadSound(buffer, buffer_size, m_DataDirectory, "KEENWLK2SND", SOUND_KEEN_WALK2);
	ok &= m_soundslot[SOUND_KEEN_JUMP].loadSound(buffer, buffer_size, m_DataDirectory, "KEENJUMPSND", SOUND_KEEN_JUMP);
	ok &= m_soundslot[SOUND_KEEN_POGO].loadSound(buffer, buffer_size, m_DataDirectory, "KEENPOGOSND", SOUND_KEEN_POGO);
	ok &= m_soundslot[SOUND_KEEN_LAND].loadSound(buffer, buffer_size, m_DataDirectory, "KEENLANDSND", SOUND_KEEN_LAND);
	ok &= m_soundslot[SOUND_KEEN_BLOK].loadSound(buffer, buffer_size, m_DataDirectory, "KEENBLOKSND", SOUND_KEEN_BLOK);
	ok &= m_soundslot[SOUND_KEEN_DIE].loadSound(buffer, buffer_size, m_DataDirectory, "KEENDIESND", SOUND_KEEN_DIE);
	ok &= m_soundslot[SOUND_KEEN_FALL].loadSound(buffer, buffer_size, m_DataDirectory, "PLUMMETSND", SOUND_KEEN_FALL);
	ok &= m_soundslot[SOUND_KEEN_BUMPHEAD].loadSound(buffer, buffer_size, m_DataDirectory, "BUMPHEADSND", SOUND_KEEN_BUMPHEAD);
	ok &= m_soundslot[SOUND_ENTER_LEVEL].loadSound(buffer, buffer_size, m_DataDirectory, "WLDENTERSND", SOUND_ENTER_LEVEL);
	ok &= m_soundslot[SOUND_KEENSLEFT].loadSound(buffer, buffer_size, m_DataDirectory, "keensleft", SOUND_KEENSLEFT);

	ok &= m_soundslot[SOUND_KEEN_FIRE].loadSound(buffer, buffer_size, m_DataDirectory, "KEENFIRESND", SOUND_KEEN_FIRE);
	ok &= m_soundslot[SOUND_GUN_CLICK].loadSound(buffer, buffer_size, m_DataDirectory, "GUNCLICK", SOUND_GUN_CLICK);
	ok &= m_soundslot[SOUND_SHOT_HIT].loadSound(buffer, buffer_size, m_DataDirectory, "SHOTHIT", SOUND_SHOT_HIT);

	ok &= m_soundslot[SOUND_GET_ITEM].loadSound(buffer, buffer_size, m_DataDirectory, "GOTITEMSND", SOUND_GET_ITEM);
	ok &= m_soundslot[SOUND_GET_BONUS].loadSound(buffer, buffer_size, m_DataDirectory, "GOTBONUSSND", SOUND_GET_BONUS);
	ok &= m_soundslot[SOUND_GET_PART].loadSound(buffer, buffer_size, m_DataDirectory, "GOTPARTSND", SOUND_GET_PART);
	ok &= m_soundslot[SOUND_LEVEL_DONE].loadSound(buffer, buffer_size, m_DataDirectory, "LVLDONESND", SOUND_LEVEL_DONE);
	ok &= m_soundslot[SOUND_GAME_OVER].loadSound(buffer, buffer_size, m_DataDirectory, "GAMEOVERSND", SOUND_GAME_OVER);
	ok &= m_soundslot[SOUND_TELEPORT].loadSound(buffer, buffer_size, m_DataDirectory, "TELEPORTSND", SOUND_TELEPORT);
	ok &= m_soundslot[SOUND_EXTRA_LIFE].loadSound(buffer, buffer_size, m_DataDirectory, "EXTRAMANSND", SOUND_EXTRA_LIFE);
	ok &= m_soundslot[SOUND_CANNONFIRE].loadSound(buffer, buffer_size, m_DataDirectory, "CANNONFIRE", SOUND_CANNONFIRE);
	ok &= m_soundslot[SOUND_CHUNKSMASH].loadSound(buffer, buffer_size, m_DataDirectory, "CHUNKSMASH", SOUND_CHUNKSMASH);
	ok &= m_soundslot[SOUND_GOINDOOR].loadSound(buffer, buffer_size, m_DataDirectory, "GOINDOORSND", SOUND_GOINDOOR);
	ok &= m_soundslot[SOUND_GET_CARD].loadSound(buffer, buffer_size, m_DataDirectory, "GETCARDSND", SOUND_GET_CARD);
	ok &= m_soundslot[SOUND_USE_KEY].loadSound(buffer, buffer_size, m_DataDirectory, "USEKEYSND", SOUND_USE_KEY);
	ok &= m_soundslot[SOUND_SWITCH_TOGGLE].loadSound(buffer, buffer_size, m_DataDirectory, "CLICKSND", SOUND_SWITCH_TOGGLE);
	ok &= m_soundslot[SOUND_DOOR_OPEN].loadSound(buffer, buffer_size, m_DataDirectory, "DOOROPENSND", SOUND_DOOR_OPEN);

	ok &= m_soundslot[SOUND_YORP_BUMP].loadSound(buffer, buffer_size, m_DataDirectory, "YORPBUMPSND", SOUND_YORP_BUMP);
	ok &= m_soundslot[SOUND_YORP_STUN].loadSound(buffer, buffer_size, m_DataDirectory, "YORPBOPSND", SOUND_YORP_STUN);
	ok &= m_soundslot[SOUND_YORP_DIE].loadSound(buffer, buffer_size, m_DataDirectory, "YORPSCREAM", SOUND_YORP_DIE);
	ok &= m_soundslot[SOUND_GARG_DIE].loadSound(buffer, buffer_size, m_DataDirectory, "GARGSCREAM", SOUND_GARG_DIE);
	ok &= m_soundslot[SOUND_VORT_DIE].loadSound(buffer, buffer_size, m_DataDirectory, "vortscream", SOUND_VORT_DIE);
	ok &= m_soundslot[SOUND_TANK_FIRE].loadSound(buffer, buffer_size, m_DataDirectory, "TANKFIRE", SOUND_TANK_FIRE);

	if (m_Episode == 2)
	{
		ok &= m_soundslot[SOUND_KEEN_BLOK].loadSound(buffer, buffer_size, m_DataDirectory, "EARTHPOW", SOUND_EARTHPOW);
	}
	else if (m_Episode == 3)
	{
		ok &= m_soundslot[SOUND_MEEP].loadSound(buffer, buffer_size, m_DataDirectory, "MEEP", SOUND_MEEP);
		ok &= m_soundslot[SOUND_ANKH].loadSound(buffer, buffer_size, m_DataDirectory, "ANKH", SOUND_ANKH);
		ok &= m_soundslot[SOUND_MORTIMER].loadSound(buffer, buffer_size, m_DataDirectory, "MORTIMER", SOUND_MORTIMER);
		ok &= m_soundslot[SOUND_FOOTSLAM].loadSound(buffer, buffer_size, m_DataDirectory, "FOOTSLAM", SOUND_FOOTSLAM);
	}

	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
		snd_chnl->setSoundSlotPtr(m_soundslot);

	if(buffer)
		delete [] buffer;

	return ok;
}

void CAudioVorticon::unloadSound()
{
	for(int slot=0 ; slot<MAX_SOUNDS ; slot++ )
		m_soundslot[slot].unload();
}
