/*
 * CPassive.cpp
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 */

#include "CPassive.h"


CPassive::CPassive(char Episode, std::string DataDirectory,
					CSavedGame &SavedGame, stOption *p_Option) :
	m_SavedGame(SavedGame)
{
	mp_Menu = NULL;
	m_modeg = false;
	m_mode = INTRO;
	m_Episode = Episode;
	m_DataDirectory = DataDirectory;
	m_NumPlayers = 0; // because no game chosen
}

CPassive::~CPassive() {
	// TODO Auto-generated destructor stub
}

