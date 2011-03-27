/*
 * CGameMode.cpp
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#include "CGameMode.h"

#include "engine/vorticon/CPassiveVort.h"
#include "engine/vorticon/CEGAGraphicsVort.h"
#include "engine/vorticon/CAudioVorticon.h"
#include "engine/vorticon/playgame/CPlayGameVorticon.h"

#include "engine/galaxy/CEGAGraphicsGalaxy.h"
#include "engine/galaxy/CPassive.h"
#include "engine/galaxy/CPlayGameGalaxy.h"


CGameMode::CGameMode(Uint8& Episode, Uint8& Numplayers,
		Uint8& Difficulty, std::string& DataDirectory):
mp_GameLauncher(NULL),
mp_PassiveMode(NULL),
mp_PlayGame(NULL),
m_Episode(Episode),
m_Numplayers(Numplayers),
m_Difficulty(Difficulty),
m_DataDirectory(DataDirectory)
{
	// TODO Auto-generated constructor stub
	m_endgame = false;
}

CGameMode::~CGameMode() {
	// TODO Auto-generated destructor stub
}
