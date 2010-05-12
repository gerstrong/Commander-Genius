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
m_demoback(false),
m_hideobjects(false),
mp_Dialog(NULL),
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
m_saveslot(0),
mp_Menu(NULL)
{
	m_menuback[1] = MAIN;
	m_menuback[2] = SAVE;
	m_menuback[3] = CONFIGURE;
	m_menuback[9] = MAIN;
	m_menuback[10] = MAIN;
	m_menuback[12] = NEW;
	m_menuback[13] = MAIN;
	m_menuback[18] = MAIN;
	m_menuback[20] = MAIN;
	m_menuback[21] = MAIN;
	m_menuback[24] = MAIN;
	m_menuback[25] = MAIN;
	m_menumap.clear();
}

void CMenu::init( char menu_type )
{
	m_menu_type = menu_type;
	m_goback = false;
	m_goback2 = false;
	m_selection = -1; // Nothing has been selected yet.
}

////
// Cleanup Routines
////
void CMenu::cleanup()
{
	m_hideobjects = false;
	// Close the old menu
	SAFE_DELETE(mp_Menu);
	SAFE_DELETE(mp_Dialog);
}


CMenu::~CMenu()
{
	// TODO Auto-generated destructor stub
}
