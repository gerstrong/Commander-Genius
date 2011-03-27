/*
 * CPassive.cpp
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 */

#include "CPassive.h"
#include "common/CBehaviorEngine.h"

CPassive::CPassive()
{
	CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
	mp_Menu = NULL;
	m_modeg = false;
	m_mode = INTRO;
	m_Episode = ExeFile.getEpisode();
	m_DataDirectory = ExeFile.getDataDirectory();
	m_NumPlayers = 0; // because no game chosen
}

