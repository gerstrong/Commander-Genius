/*
 * CHelpMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CHelpMenu.h"

CHelpMenu::CHelpMenu(CInfoScene *&p_InfoScene, Uint8 dlg_theme,
		const char &Episode, const std::string &GamePath,
		bool &hideobjects) :
CBaseMenu(dlg_theme),
mp_InfoScene(p_InfoScene),
m_Episode(Episode),
m_GamePath(GamePath),
m_hideobjects(hideobjects)
{
	mp_Dialog = new CDialog(18, 9, INPUT_MODE_UP_DOWN,m_dlg_theme);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "The Menu");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "The Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "The Story");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Ordering Info");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "About ID");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "About CG");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Credits");
}

void CHelpMenu::processSpecific()
{
	if( m_selection != NO_SELECTION)
	{
		//m_Map.m_animation_enabled = false;
		// no cleanups here, because later we return back to that menu
		switch(m_selection)
		{
		case 0:
			mp_InfoScene = new CHelp(m_GamePath, m_Episode, "Menu");
			break;
		case 1:
			mp_InfoScene = new CHelp(m_GamePath, m_Episode, "Game");
			break;
		case 2:
			mp_InfoScene = new CStory(m_GamePath, m_Episode);
			break;
		case 3:
			mp_InfoScene = new COrderingInfo(m_GamePath, m_Episode);
			break;
		case 4:
			mp_InfoScene = new CAbout(m_GamePath, m_Episode, "ID");
			break;
		case 5:
			mp_InfoScene = new CAbout(m_GamePath, m_Episode, "CG");
			break;
		case 6:
			mp_InfoScene = new CCredits(m_GamePath, m_Episode);
			break;
		}
		m_hideobjects = true;
		m_selection = NO_SELECTION;
		m_mustclose = true;
	}
}

CHelpMenu::~CHelpMenu()
{
	// TODO Auto-generated destructor stub
}
