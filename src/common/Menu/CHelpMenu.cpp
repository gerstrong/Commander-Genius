/*
 * CHelpMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CHelpMenu.h"

#include "engine/infoscenes/CHighScores.h"
#include "engine/infoscenes/CStory.h"
#include "engine/infoscenes/CCredits.h"
#include "engine/infoscenes/COrderingInfo.h"
#include "engine/infoscenes/CAbout.h"
#include "engine/infoscenes/CHelp.h"
#include "engine/infoscenes/CPreviews.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CHelpMenu::CHelpMenu(CInfoScene *&p_InfoScene, Uint8 dlg_theme,
		CExeFile &ExeFile,
		bool &hideobjects) :
CBaseMenu(dlg_theme),
m_ExeFile(ExeFile),
mp_InfoScene(p_InfoScene),
m_Episode(ExeFile.getEpisode()),
m_GamePath(ExeFile.getDataDirectory()),
m_hideobjects(hideobjects)
{
	/*mp_Dialog = new CDialog(18, 10, INPUT_MODE_UP_DOWN,m_dlg_theme);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "The Menu");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "The Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "The Story");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Ordering Info");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "About ID");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "About CG");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Credits");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Previews!");*/
}

void CHelpMenu::processSpecific()
{
	/*if( m_selection != NO_SELECTION)
	{
		//m_Map.m_animation_enabled = false;
		// no cleanups here, because later we return back to that menu
		switch(m_selection)
		{
		case 0:
			mp_InfoScene = new CHelp(m_ExeFile, "Menu");
			break;
		case 1:
			mp_InfoScene = new CHelp(m_ExeFile, "Game");
			break;
		case 2:
			mp_InfoScene = new CStory(m_ExeFile);
			break;
		case 3:
			mp_InfoScene = new COrderingInfo(m_ExeFile);
			break;
		case 4:
			mp_InfoScene = new CAbout(m_ExeFile, "ID");
			break;
		case 5:
			mp_InfoScene = new CAbout(m_ExeFile, "CG");
			break;
		case 6:
			mp_InfoScene = new CCredits(m_ExeFile.getDataDirectory(),
										m_ExeFile.getEpisode());
			break;
		case 7:
			mp_InfoScene = new CPreviews(m_ExeFile);
			break;
		}
		m_hideobjects = true;
		m_selection = NO_SELECTION;
		m_mustclose = true;
	}*/
}

CHelpMenu::~CHelpMenu()
{
	//SAFE_DELETE(mp_Dialog);
}
