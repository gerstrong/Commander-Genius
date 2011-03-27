/*
 * CMenu.cpp
 *
 *  Created on: 22.04.2010
 *      Author: gerstrong
 */

#include "CMenu.h"

#include "Menu/CSettingsMenu.h"
#include "Menu/CConfirmMenu.h"
#include "Menu/CSaveMenu.h"
#include "Menu/CLoadMenu.h"
#include "Menu/CSelectionMenu.h"

CMenu::CMenu(char menu_mode,
		 CSavedGame &SavedGame,
		 Uint8 DlgTheme,
		 bool &restartVideo) :
CBaseMenu(DlgTheme),
m_demoback(false),
mp_Dialog(NULL),
m_ExeFile(g_pBehaviorEngine->m_ExeFile),
m_Episode(m_ExeFile.getEpisode()),
m_GamePath(m_ExeFile.getDataDirectory()),
m_SavedGame(SavedGame),
m_menu_mode(menu_mode),
m_choosegame(false),
m_overwrite(false),
m_Endgame(false),
m_quit(false),
m_selection(NO_SELECTION),
m_lastselect(-1),
m_menu_type(MAIN),
m_NumPlayers(0),
m_Difficulty(0),
m_saveslot(0),
m_restartVideo(restartVideo)
{}

void CMenu::init( menutypes menu_type )
{
	m_menu_type = menu_type;
	m_selection = NO_SELECTION; // Nothing has been selected yet.

	switch(m_menu_type)
	{
	case QUIT:
		m_SubMenus.push_back(new CConfirmMenu("   Quit the game?   ", m_quit, m_dlg_theme)); break;
	case ENDGAME:
		m_SubMenus.push_back(new CConfirmMenu("   End your game?   ", m_quit, m_Endgame)); break;
	case NEW:
	{
		// you want to start a new game. Well, I need to know the amount of players and difficulty
		std::list<std::string> players_list;
		for(size_t i=1 ; i<=MAX_PLAYERS ; i++)
			players_list.push_back(itoa(i) + " Player");

		std::list<std::string> diff_list;
		diff_list.push_back("Easy");
		diff_list.push_back("Normal");
		diff_list.push_back("Hard");

		m_SubMenus.push_back(new CSelectionMenu<Uint8>( m_NumPlayers, players_list, m_dlg_theme) );
		if(!g_pBehaviorEngine->m_is_a_mod)
			m_SubMenus.push_back(new CSelectionMenu<Uint8>( m_Difficulty, diff_list, m_dlg_theme, 1 ) );
		else
			m_Difficulty = 2;
	}
	break;
	case CONFIGURE:
		m_SubMenus.push_back(new CSettingsMenu(m_dlg_theme, m_ExeFile, m_restartVideo)); break;
	case AUDIO:
		m_SubMenus.push_back(new CSettingsMenu(m_dlg_theme, m_ExeFile, m_restartVideo, 1)); break;
	case CONTROLS:
		m_SubMenus.push_back(new CSettingsMenu(m_dlg_theme, m_ExeFile, m_restartVideo, 3)); break;
	case SAVE:
		m_SubMenus.push_back(new CSaveMenu(m_dlg_theme, m_SavedGame)); break;
	case LOAD:
		m_SubMenus.push_back(new CLoadMenu(m_dlg_theme, m_SavedGame)); break;
	default:
		break;
	}
}

void CMenu::initMainMenu()
{
	mp_Dialog = new CDialog(17, 10, INPUT_MODE_UP_DOWN,m_dlg_theme);

	// Being at Intro, Title or Demo mode
	if( m_menu_mode == PASSIVE )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "New Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Load Game");
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 3, 	"Save Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "High Scores");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Configure");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Back to Demo");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Choose Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Quit");
	}
	// When Player is playing
	if( m_menu_mode == ACTIVE )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "New Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Load Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Save Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "High Scores");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Configure");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Back to Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "End Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Quit");
	}
}

////
// Process
////
void CMenu::process()
{
	if( g_pInput->getHoldedKey(KM) && g_pInput->getHoldedKey(KO) && g_pInput->getHoldedKey(KD) )
	{
		cleanup();
		init(MODCONF);
	}

	if( !m_SubMenus.empty() )
	{
		CBaseMenu *SubMenu = m_SubMenus.front();
		SubMenu->processCommon();
		SubMenu->processSpecific();
		SubMenu->postProcess();

		if(SubMenu->mustClose())
		{
			SAFE_DELETE(SubMenu);
			m_SubMenus.pop_front();
		}
	}
	else
	{
		if(mp_Dialog)
		{
			// Get Input for selection
			if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) )
			{
				m_selection = mp_Dialog->getSelection();
			}

			if( mp_Dialog->getInputMode(INPUT_MODE_LEFT_RIGHT) )
			{
				if( g_pInput->getPressedKey(KY) )
				{
					m_selection = 1;
				}
				else if( g_pInput->getPressedKey(KN) )
				{
					m_selection = 2;
				}
			}

			if( g_pInput->getPressedCommand(IC_QUIT) )
			{
				if(m_menu_mode == PASSIVE)
					m_SubMenus.push_back(new CConfirmMenu("   Quit the game?   ", m_quit, m_dlg_theme));
				else
					m_mustclose = true;
			}
			mp_Dialog->processInput();

			// Draw the menu
			mp_Dialog->draw();
		}

		processMainMenu();
	}
}

void CMenu::processMainMenu()
{
	if( m_selection != NO_SELECTION )
	{
		if( m_menu_mode == PASSIVE )
		{
			switch(m_selection)
			{
			case 0: init(NEW); break;
			case 1: init(LOAD); break;
			case 4: init(CONFIGURE);break;
			case 5: m_demoback = true; break;
			case 6: m_choosegame = true;break;
			case 7: init(QUIT);break;
			}
		}
		else if( m_menu_mode == ACTIVE )
		{
			switch(m_selection)
			{
			case 0: init(NEW); break;
			case 1: init(LOAD); break;
			case 2: init(SAVE);break;
			case 4: init(CONFIGURE);break;
			case 5: m_mustclose = true; break;
			case 6: m_SubMenus.push_back(new CConfirmMenu("Back to Titlescreen?", m_Endgame, m_dlg_theme));
					m_selection = NO_SELECTION;
					break;
			case 7: init(QUIT);break;
			}
		}
	}
}

////
// Cleanup Routines
////
void CMenu::cleanup()
{
	// Close the old menu
	while(!m_SubMenus.empty())
	{
		delete m_SubMenus.front();
		m_SubMenus.pop_back();
	}
	SAFE_DELETE(mp_Dialog);
}


CMenu::~CMenu()
{
	cleanup();
}
