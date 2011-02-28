/*
 * CPassive.cpp
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 */

#include "CPassive.h"


CPassive::CPassive(CExeFile &Exefile,
					CSavedGame &SavedGame, Uint8& difficulty) :
	m_Difficulty(difficulty),
	m_SavedGame(SavedGame)
{
	mp_Menu = NULL;
	m_modeg = false;
	m_mode = INTRO;
	m_Episode = Exefile.getEpisode();
	m_DataDirectory = Exefile.getDataDirectory();
	m_NumPlayers = 0; // because no game chosen
}

