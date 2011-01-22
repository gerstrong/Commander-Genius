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


/*  char extractOfExeFile(const std::string& inputpath, int episode)

 Utility to extract the embedded sounds from Commander Keen
 Episode 2 (keen2.exe) and Episode 3 (keen3.exe)

 Implemented by Gerhard Stein  <gerstrong@gmail.com> (2008,2009)
 Copyright (C) 2007 Hans de Goede  <j.w.r.degoede@hhs.nl>

 Many thanks to Mitugu(Kou) Kurizono for the decompression algorithm.

 Modification by Gerstrong 2009
 The new version uses a class named CExeFile which is able to manage compressed or
 uncompressed files. The function simply let the class do its job and then copy
 the sound data.
 */

char CAudioVorticon::extractOfExeFile(CExeFile &ExeFile)
{
	std::string outputfname;
	int bit_count;
	int pos, sounds_start, sounds_end, ret = 0;
	std::string buffer;
	std::string inputpath = ExeFile.getDataDirectory();
	char episode = ExeFile.getEpisode();


	pos = 0;
	bit_count = 0;

	// Set Offsets. Episode 1 already provides this
	if (episode == 2)
	{
	    outputfname = "sounds.ck2";
	    sounds_start  = 0x12730;
	    sounds_end    = 0x14BDA;
	}
	else if (episode == 3)
	{
	    outputfname = "sounds.ck3";
	    sounds_start  = 0x13A70;
	    sounds_end    = 0x164D4;
	}
	else
	{
		g_pLogFile->ftextOut("Error: Unknown episode: %d<br>", episode);
	    return 1;
	}

	FILE *fout;

	buffer = inputpath;
	if( *(buffer.end()) != '/') buffer  += "/";
	buffer += outputfname;

	if(!(fout = OpenGameFile(buffer.c_str(),"wb"))) ret = 1;
	else
	{
		fwrite( ExeFile.getRawData()+sounds_start, 1, (sounds_end-sounds_start), fout);
		g_pLogFile->ftextOut(GREEN,"Sounds extraction completed successfully<br>");
		fclose(fout);
	}

	return ret;
}

bool CAudioVorticon::loadSoundData(CExeFile &ExeFile)
{
	if(!m_active) return false;

	bool ok = true;
	const std::string soundfile = "sounds.ck" + itoa(m_Episode);

	if(!m_soundslot.empty())
		m_soundslot.clear();

	for(int i=0 ; i<MAX_SOUNDS ; i++)
	{
		m_soundslot[i].setpAudioSpec(&AudioSpec);
		m_soundslot[i].m_gamepath = m_DataDirectory;
	}

	g_pLogFile->ftextOut("sound_load_all(): loading all sounds...<br>");

	std::ifstream file;
	if(!OpenGameFileR(file, getResourceFilename(soundfile,m_DataDirectory,false,true), std::ios::binary))
	{
		std::string exename = "keen" + itoa(m_Episode) + ".exe";
		g_pLogFile->textOut("sound_load_all(): \"" + soundfile + "\" was not found in the data directory. Looking for \""+ exename +"\" in \"" + m_DataDirectory + "\" and trying to extract this file<br>");
		extractOfExeFile(ExeFile);
	}
	else
		file.close();

	ok  = m_soundslot[SOUND_KEEN_WALK].loadSound(soundfile, m_DataDirectory, "KEENWALKSND", SOUND_KEEN_WALK);
	ok &= m_soundslot[SOUND_KEEN_WALK2].loadSound(soundfile, m_DataDirectory, "KEENWLK2SND", SOUND_KEEN_WALK2);
	ok &= m_soundslot[SOUND_KEEN_JUMP].loadSound(soundfile, m_DataDirectory, "KEENJUMPSND", SOUND_KEEN_JUMP);
	ok &= m_soundslot[SOUND_KEEN_POGO].loadSound(soundfile, m_DataDirectory, "KEENPOGOSND", SOUND_KEEN_POGO);
	ok &= m_soundslot[SOUND_KEEN_LAND].loadSound(soundfile, m_DataDirectory, "KEENLANDSND", SOUND_KEEN_LAND);
	ok &= m_soundslot[SOUND_KEEN_BLOK].loadSound(soundfile, m_DataDirectory, "KEENBLOKSND", SOUND_KEEN_BLOK);
	ok &= m_soundslot[SOUND_KEEN_DIE].loadSound(soundfile, m_DataDirectory, "KEENDIESND", SOUND_KEEN_DIE);
	ok &= m_soundslot[SOUND_KEEN_FALL].loadSound(soundfile, m_DataDirectory, "PLUMMETSND", SOUND_KEEN_FALL);
	ok &= m_soundslot[SOUND_KEEN_BUMPHEAD].loadSound(soundfile, m_DataDirectory, "BUMPHEADSND", SOUND_KEEN_BUMPHEAD);
	ok &= m_soundslot[SOUND_ENTER_LEVEL].loadSound(soundfile, m_DataDirectory, "WLDENTERSND", SOUND_ENTER_LEVEL);
	ok &= m_soundslot[SOUND_KEENSLEFT].loadSound(soundfile, m_DataDirectory, "keensleft", SOUND_KEENSLEFT);

	ok &= m_soundslot[SOUND_KEEN_FIRE].loadSound(soundfile, m_DataDirectory, "KEENFIRESND", SOUND_KEEN_FIRE);
	ok &= m_soundslot[SOUND_GUN_CLICK].loadSound(soundfile, m_DataDirectory, "GUNCLICK", SOUND_GUN_CLICK);
	ok &= m_soundslot[SOUND_SHOT_HIT].loadSound(soundfile, m_DataDirectory, "SHOTHIT", SOUND_SHOT_HIT);

	ok &= m_soundslot[SOUND_GET_ITEM].loadSound(soundfile, m_DataDirectory, "GOTITEMSND", SOUND_GET_ITEM);
	ok &= m_soundslot[SOUND_GET_BONUS].loadSound(soundfile, m_DataDirectory, "GOTBONUSSND", SOUND_GET_BONUS);
	ok &= m_soundslot[SOUND_GET_PART].loadSound(soundfile, m_DataDirectory, "GOTPARTSND", SOUND_GET_PART);
	ok &= m_soundslot[SOUND_LEVEL_DONE].loadSound(soundfile, m_DataDirectory, "LVLDONESND", SOUND_LEVEL_DONE);
	ok &= m_soundslot[SOUND_GAME_OVER].loadSound(soundfile, m_DataDirectory, "GAMEOVERSND", SOUND_GAME_OVER);
	ok &= m_soundslot[SOUND_TELEPORT].loadSound(soundfile, m_DataDirectory, "TELEPORTSND", SOUND_TELEPORT);
	ok &= m_soundslot[SOUND_EXTRA_LIFE].loadSound(soundfile, m_DataDirectory, "EXTRAMANSND", SOUND_EXTRA_LIFE);
	ok &= m_soundslot[SOUND_CANNONFIRE].loadSound(soundfile, m_DataDirectory, "CANNONFIRE", SOUND_CANNONFIRE);
	ok &= m_soundslot[SOUND_CHUNKSMASH].loadSound(soundfile, m_DataDirectory, "CHUNKSMASH", SOUND_CHUNKSMASH);
	ok &= m_soundslot[SOUND_GOINDOOR].loadSound(soundfile, m_DataDirectory, "GOINDOORSND", SOUND_GOINDOOR);
	ok &= m_soundslot[SOUND_GET_CARD].loadSound(soundfile, m_DataDirectory, "GETCARDSND", SOUND_GET_CARD);
	ok &= m_soundslot[SOUND_USE_KEY].loadSound(soundfile, m_DataDirectory, "USEKEYSND", SOUND_USE_KEY);
	ok &= m_soundslot[SOUND_SWITCH_TOGGLE].loadSound(soundfile, m_DataDirectory, "CLICKSND", SOUND_SWITCH_TOGGLE);
	ok &= m_soundslot[SOUND_DOOR_OPEN].loadSound(soundfile, m_DataDirectory, "DOOROPENSND", SOUND_DOOR_OPEN);

	ok &= m_soundslot[SOUND_YORP_BUMP].loadSound(soundfile, m_DataDirectory, "YORPBUMPSND", SOUND_YORP_BUMP);
	ok &= m_soundslot[SOUND_YORP_STUN].loadSound(soundfile, m_DataDirectory, "YORPBOPSND", SOUND_YORP_STUN);
	ok &= m_soundslot[SOUND_YORP_DIE].loadSound(soundfile, m_DataDirectory, "YORPSCREAM", SOUND_YORP_DIE);
	ok &= m_soundslot[SOUND_GARG_DIE].loadSound(soundfile, m_DataDirectory, "GARGSCREAM", SOUND_GARG_DIE);
	ok &= m_soundslot[SOUND_VORT_DIE].loadSound(soundfile, m_DataDirectory, "vortscream", SOUND_VORT_DIE);
	ok &= m_soundslot[SOUND_TANK_FIRE].loadSound(soundfile, m_DataDirectory, "TANKFIRE", SOUND_TANK_FIRE);

	if (m_Episode == 2)
	{
		ok &= m_soundslot[SOUND_KEEN_BLOK].loadSound(soundfile, m_DataDirectory, "EARTHPOW", SOUND_EARTHPOW);
	}
	else if (m_Episode == 3)
	{
		ok &= m_soundslot[SOUND_MEEP].loadSound(soundfile, m_DataDirectory, "MEEP", SOUND_MEEP);
		ok &= m_soundslot[SOUND_ANKH].loadSound(soundfile, m_DataDirectory, "ANKH", SOUND_ANKH);
		ok &= m_soundslot[SOUND_MORTIMER].loadSound(soundfile, m_DataDirectory, "MORTIMER", SOUND_MORTIMER);
		ok &= m_soundslot[SOUND_FOOTSLAM].loadSound(soundfile, m_DataDirectory, "FOOTSLAM", SOUND_FOOTSLAM);
	}

	std::vector<CSoundChannel>::iterator snd_chnl = m_soundchannel.begin();
	for( ; snd_chnl != m_soundchannel.end() ; snd_chnl++)
		snd_chnl->setSoundSlotPtr(m_soundslot);

	return ok;
}
