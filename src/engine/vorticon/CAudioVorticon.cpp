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

CAudioVorticon::CAudioVorticon(const CExeFile &ExeFile, const SDL_AudioSpec &AudioSpec) :
CAudioResources(AudioSpec),
m_ExeFile(ExeFile)
{}

/**
	\brief This will load the Audio into the buffer from vorticon based keen games
	\param buffer_size  Size of the buffer which will be. Other functions need it to determine later when to stop mapping

	\return Returns the pointer the allocated and streamed buffer or NULL if nothing could be read at all
 */
Uint8* CAudioVorticon::loadSoundStream(Uint32 &buffer_size, Uint8* exedata)
{
	Uint8 *buffer = NULL;
	buffer_size = 0;
	const std::string gamepath = m_ExeFile.getDataDirectory();

	for(int i=0 ; i<MAX_SOUNDS ; i++)
		m_soundslot[i].m_gamepath=gamepath;

	const std::string soundfile = "sounds.ck" + itoa(m_ExeFile.getEpisode());
	g_pLogFile->ftextOut("sound_load_all(): trying to open the game audio...<br>");

	std::ifstream file;
	if(!OpenGameFileR(file, getResourceFilename(soundfile, gamepath, false, true), std::ios::binary))
	{
		Uint32 sounds_start, sounds_end;

		// if not available try to extract it
		if (m_ExeFile.getEpisode() == 2)
		{
		    sounds_start  = 0x12730;
		    sounds_end    = 0x14BDA;
		}
		else if (m_ExeFile.getEpisode() == 3)
		{
		    sounds_start  = 0x13A70;
		    sounds_end    = 0x164D4;
		}
		else
		{
			g_pLogFile->ftextOut("Warning: I cannot extract sounds from that game. Please provide a \"sounds.ck%d\" for that game.", m_ExeFile.getEpisode());
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

bool CAudioVorticon::loadSoundData()
{
	bool ok = true;
	const int episode = m_ExeFile.getEpisode();
	const std::string soundfile = "sounds.ck" + itoa(episode);
	const std::string DataDirectory = m_ExeFile.getDataDirectory();

	g_pLogFile->ftextOut("sound_load_all(): loading all sounds...<br>");

	Uint32 buffer_size;
	Uint8 *buffer = loadSoundStream( buffer_size, m_ExeFile.getRawData() );

	ok  = m_soundslot[SOUND_KEEN_WALK].loadSound(buffer, buffer_size, DataDirectory, "KEENWALKSND", SOUND_KEEN_WALK);
	ok &= m_soundslot[SOUND_KEEN_WALK2].loadSound(buffer, buffer_size, DataDirectory, "KEENWLK2SND", SOUND_KEEN_WALK2);
	ok &= m_soundslot[SOUND_KEEN_JUMP].loadSound(buffer, buffer_size, DataDirectory, "KEENJUMPSND", SOUND_KEEN_JUMP);
	ok &= m_soundslot[SOUND_KEEN_POGO].loadSound(buffer, buffer_size, DataDirectory, "KEENPOGOSND", SOUND_KEEN_POGO);
	ok &= m_soundslot[SOUND_KEEN_LAND].loadSound(buffer, buffer_size, DataDirectory, "KEENLANDSND", SOUND_KEEN_LAND);
	ok &= m_soundslot[SOUND_KEEN_BLOK].loadSound(buffer, buffer_size, DataDirectory, "KEENBLOKSND", SOUND_KEEN_BLOK);
	ok &= m_soundslot[SOUND_KEEN_DIE].loadSound(buffer, buffer_size, DataDirectory, "KEENDIESND", SOUND_KEEN_DIE);
	ok &= m_soundslot[SOUND_KEEN_FALL].loadSound(buffer, buffer_size, DataDirectory, "PLUMMETSND", SOUND_KEEN_FALL);
	ok &= m_soundslot[SOUND_KEEN_BUMPHEAD].loadSound(buffer, buffer_size, DataDirectory, "BUMPHEADSND", SOUND_KEEN_BUMPHEAD);
	ok &= m_soundslot[SOUND_ENTER_LEVEL].loadSound(buffer, buffer_size, DataDirectory, "WLDENTERSND", SOUND_ENTER_LEVEL);
	ok &= m_soundslot[SOUND_KEENSLEFT].loadSound(buffer, buffer_size, DataDirectory, "keensleft", SOUND_KEENSLEFT);

	ok &= m_soundslot[SOUND_KEEN_FIRE].loadSound(buffer, buffer_size, DataDirectory, "KEENFIRESND", SOUND_KEEN_FIRE);
	ok &= m_soundslot[SOUND_GUN_CLICK].loadSound(buffer, buffer_size, DataDirectory, "GUNCLICK", SOUND_GUN_CLICK);
	ok &= m_soundslot[SOUND_SHOT_HIT].loadSound(buffer, buffer_size, DataDirectory, "SHOTHIT", SOUND_SHOT_HIT);

	ok &= m_soundslot[SOUND_GET_ITEM].loadSound(buffer, buffer_size, DataDirectory, "GOTITEMSND", SOUND_GET_ITEM);
	ok &= m_soundslot[SOUND_GET_BONUS].loadSound(buffer, buffer_size, DataDirectory, "GOTBONUSSND", SOUND_GET_BONUS);
	ok &= m_soundslot[SOUND_GET_PART].loadSound(buffer, buffer_size, DataDirectory, "GOTPARTSND", SOUND_GET_PART);
	ok &= m_soundslot[SOUND_LEVEL_DONE].loadSound(buffer, buffer_size, DataDirectory, "LVLDONESND", SOUND_LEVEL_DONE);
	ok &= m_soundslot[SOUND_GAME_OVER].loadSound(buffer, buffer_size, DataDirectory, "GAMEOVERSND", SOUND_GAME_OVER);
	ok &= m_soundslot[SOUND_TELEPORT].loadSound(buffer, buffer_size, DataDirectory, "TELEPORTSND", SOUND_TELEPORT);
	ok &= m_soundslot[SOUND_EXTRA_LIFE].loadSound(buffer, buffer_size, DataDirectory, "EXTRAMANSND", SOUND_EXTRA_LIFE);
	ok &= m_soundslot[SOUND_CANNONFIRE].loadSound(buffer, buffer_size, DataDirectory, "CANNONFIRE", SOUND_CANNONFIRE);
	ok &= m_soundslot[SOUND_CHUNKSMASH].loadSound(buffer, buffer_size, DataDirectory, "CHUNKSMASH", SOUND_CHUNKSMASH);
	ok &= m_soundslot[SOUND_GOINDOOR].loadSound(buffer, buffer_size, DataDirectory, "GOINDOORSND", SOUND_GOINDOOR);
	ok &= m_soundslot[SOUND_GET_CARD].loadSound(buffer, buffer_size, DataDirectory, "GETCARDSND", SOUND_GET_CARD);
	ok &= m_soundslot[SOUND_USE_KEY].loadSound(buffer, buffer_size, DataDirectory, "USEKEYSND", SOUND_USE_KEY);
	ok &= m_soundslot[SOUND_SWITCH_TOGGLE].loadSound(buffer, buffer_size, DataDirectory, "CLICKSND", SOUND_SWITCH_TOGGLE);
	ok &= m_soundslot[SOUND_DOOR_OPEN].loadSound(buffer, buffer_size, DataDirectory, "DOOROPENSND", SOUND_DOOR_OPEN);

	ok &= m_soundslot[SOUND_YORP_BUMP].loadSound(buffer, buffer_size, DataDirectory, "YORPBUMPSND", SOUND_YORP_BUMP);
	ok &= m_soundslot[SOUND_YORP_STUN].loadSound(buffer, buffer_size, DataDirectory, "YORPBOPSND", SOUND_YORP_STUN);
	ok &= m_soundslot[SOUND_YORP_DIE].loadSound(buffer, buffer_size, DataDirectory, "YORPSCREAM", SOUND_YORP_DIE);
	ok &= m_soundslot[SOUND_GARG_DIE].loadSound(buffer, buffer_size, DataDirectory, "GARGSCREAM", SOUND_GARG_DIE);
	ok &= m_soundslot[SOUND_VORT_DIE].loadSound(buffer, buffer_size, DataDirectory, "vortscream", SOUND_VORT_DIE);
	ok &= m_soundslot[SOUND_TANK_FIRE].loadSound(buffer, buffer_size, DataDirectory, "TANKFIRE", SOUND_TANK_FIRE);

	if (episode == 2)
	{
		ok &= m_soundslot[SOUND_KEEN_BLOK].loadSound(buffer, buffer_size, DataDirectory, "EARTHPOW", SOUND_EARTHPOW);
	}
	else if (episode == 3)
	{
		ok &= m_soundslot[SOUND_MEEP].loadSound(buffer, buffer_size, DataDirectory, "MEEP", SOUND_MEEP);
		ok &= m_soundslot[SOUND_ANKH].loadSound(buffer, buffer_size, DataDirectory, "ANKH", SOUND_ANKH);
		ok &= m_soundslot[SOUND_MORTIMER].loadSound(buffer, buffer_size, DataDirectory, "MORTIMER", SOUND_MORTIMER);
		ok &= m_soundslot[SOUND_FOOTSLAM].loadSound(buffer, buffer_size, DataDirectory, "FOOTSLAM", SOUND_FOOTSLAM);
	}

	if(buffer)
		delete [] buffer;

	return ok;
}

void CAudioVorticon::unloadSound()
{
	for(int slot=0 ; slot<MAX_SOUNDS ; slot++ )
		m_soundslot[slot].unload();
}
