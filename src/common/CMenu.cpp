/*
 * CMenu.cpp
 *
 *  Created on: 22.04.2010
 *      Author: gerstrong
 */

#include "CMenu.h"

CMenu::CMenu( char menu_mode, std::string &GamePath,
		 char &Episode, CSavedGame &SavedGame,
		 stOption *pOption ) :
m_Episode(Episode),
m_GamePath(GamePath),
m_SavedGame(SavedGame),
m_RestartVideo(false),
mp_option(pOption),
m_menu_mode(menu_mode),
m_choosegame(false),
m_overwrite(false),
m_goback(false),
m_goback2(false),
m_Endgame(false),
m_quit(false),
m_selection(-1),
m_lastselect(-1),
m_menu_type(MAIN),
m_NumPlayers(0),
m_Difficulty(-1),
m_saveslot(0)
{
	// TODO Auto-generated constructor stub
}

CMenu::~CMenu()
{
	// TODO Auto-generated destructor stub
}
