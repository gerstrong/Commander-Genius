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

CAudioGalaxy::CAudioGalaxy(const CExeFile &ExeFile, const SDL_AudioSpec &AudioSpec) :
CAudioResources(AudioSpec),
m_ExeFile(ExeFile)
{}

/**
 * Main load function for the galaxy audio
 */
bool CAudioGalaxy::loadSoundData()
{
	// Read the embedded AUDIODICT
	//const int episode = m_ExeFile.getEpisode();
	//const std::string datadir = m_ExeFile.getDataDirectory();

	//Uint8* AudioDict_Ptr = NULL;
	//Uint8* AudioHed_Ptr = NULL;
	//Uint8* AudioData_Ptr = NULL;

/*	if( episode == 4 )
	{

	}
	else*/
	{
		g_pLogFile->textOut("CAudioGalaxy::loadSoundData(): The function cannot read Audio of that game");
		return false;
	}


	// TODO: Read the AUDIOHED

	// TODO: Open the Audio File and read the data for it into the sound-slots

	// TODO: Free allocated Stuff

	return false;
}

/**
 * Will free the resources of the audio
 */
void CAudioGalaxy::unloadSound()
{

}
