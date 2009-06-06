/*
 * CGame.cpp
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#include <stdlib.h>

#include "CGame.h"
#include "fileio.h"
#include "keen.h"
#include "sdl/sound/CSound.h"

CGame::CGame() {
	m_Episode = 0;
	memset(m_DataDirectory,0,256);
	Latch = new CLatch();
}

CGame::~CGame() {
	if(Latch){ delete Latch; Latch = NULL;}
}

void CGame::runCycle(void)
{
	// TODO: Code here!!
}

int CGame::loadResources(unsigned short Episode, char *DataDirectory)
{
	m_Episode = Episode;
	memcpy(m_DataDirectory, DataDirectory, 256);

	// Decode the graphics for the game (EGALATCH, EGASPRIT)
	if (Latch->loadGraphics(Episode, DataDirectory)) return 1;

	loadstrings("strings.dat");

	// Load the sound data
	int ok;
	ok = g_pSound->loadSoundData(m_Episode, DataDirectory);
	if( ok ) return ok;
	return 0;
}

CLatch *CGame::getLatch(void)
{	return Latch;	}
