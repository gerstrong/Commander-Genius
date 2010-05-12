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
processPtr(NULL),
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

void CMenu::initMainMenu()
{
	mp_Dialog = new CDialog(17, 10);

	m_menumap.clear();
	// When in Intro, Title, Demo mode
	if( m_menu_mode == PASSIVE )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "New Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Load Game");
		mp_Dialog->addObject(DLG_OBJ_DISABLED, 1, 3, "Save Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "High Scores");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Configure");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "Back to Demo");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Choose Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Quit");
	}
	// When Player is playing
	// TODO: This still must be adapted to ingame situation
	if( m_menu_mode == ACTIVE )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "New Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Load Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT,  1, 3, "Save Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT,  1, 4, "High Scores");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Configure");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT,  1, 6, "Back to Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "End Game");
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 8, "Quit");
		m_menumap[6] = ENDGAME;
	}
	m_menumap[0] = NEW;
	m_menumap[1] = LOAD;
	m_menumap[2] = SAVE;
	m_menumap[4] = CONFIGURE;
	m_menumap[7] = QUIT;
}

void CMenu::initNumPlayersMenu()
{
	mp_Dialog = new CDialog(13, MAX_PLAYERS+2);
	int i;

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "1 Player");
	for(i=2;i<=MAX_PLAYERS;i++)
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, itoa(i)+" Player");
	}
	m_menumap.clear();
	m_menumap[0] = DIFFICULTY;
	m_menumap[1] = DIFFICULTY;
	m_menumap[2] = DIFFICULTY;
	m_menumap[3] = DIFFICULTY;
}

void CMenu::initDebugMenu()
{
	mp_Dialog = new CDialog(18, 5);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "<O==========>");
	mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 0;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "<O==========>");
	mp_Dialog->m_dlgobject.at(1)->m_Option->m_value = 0;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "<O==========>");
	mp_Dialog->m_dlgobject.at(2)->m_Option->m_value = 0;

	mp_Dialog->m_key = 's';
}

void CMenu::initModMenu()
{
	mp_Dialog = new CDialog(18, 5);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "<O==========>");
	mp_Dialog->m_dlgobject.at(0)->m_Option->m_value = 0;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "<O==========>");
	mp_Dialog->m_dlgobject.at(1)->m_Option->m_value = 0;
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "<O==========>");
	mp_Dialog->m_dlgobject.at(2)->m_Option->m_value = 0;

	mp_Dialog->m_key = 's';
}

void CMenu::initConfigureMenu()
{
	mp_Dialog = new CDialog(13, 6);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Graphics");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Audio");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Options");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Controls");

	m_menumap.clear();
	m_menumap[0] = GRAPHICS;
	m_menumap[1] = AUDIO;
	m_menumap[2] = OPTIONS;
	m_menumap[3] = CONTROLPLAYERS;
}

void CMenu::initNumControlMenu()
{
	mp_Dialog = new CDialog(13, MAX_PLAYERS+2);
	int i;

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Player 1");
	for(i=2;i<=MAX_PLAYERS;i++)
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, "Player "+itoa(i));
	}
	m_menumap.clear();
	m_menumap[0] = CONTROLS;
	m_menumap[1] = CONTROLS;
	m_menumap[2] = CONTROLS;
	m_menumap[3] = CONTROLS;
}

void CMenu::initF1Menu()
{
	mp_Dialog = new CDialog(18, 9);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "The Menu");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "The Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "The Story");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Ordering Info");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "About ID");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "About CG");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Credits");

	// In the Help system let's hide all objects like Bitmaps, players, enemies, etc.
}

void CMenu::initConfirmMenu(std::string confirmtext)
{
	mp_Dialog = new CDialog(0, 0, 22, 5, 'l');

	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 1, confirmtext);
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Yes");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 15, 3, "No");
}

void CMenu::initDifficultyMenu()
{
	mp_Dialog = new CDialog(11, 5);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Easy");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Normal");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Hard");

	mp_Dialog->processInput(1);
}

void CMenu::initSaveMenu()
{
	std::string text;
	mp_Dialog = new CDialog(0, 0, 22, 22, 'u');

	// Load the state-file list
	m_StateFileList = m_SavedGame.getSlotList();

	for(Uint32 i=1;i<=20;i++)
	{
		text = "";
		if(i <= m_StateFileList.size())
		{
			text = m_StateFileList.at(i-1);
			mp_Dialog->m_name = text;
		}
		if(text == "")
			text = "     EMPTY       ";
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, text);
	}
}

////
// Process
////
void CMenu::process()
{
	if(processPtr != NULL)
		(this->*processPtr)();
}

void CMenu::processNumPlayersMenu()
{
	if( m_selection != -1)
	{
		cleanup();
		if( m_selection < MAX_PLAYERS )
		{
			m_NumPlayers = m_selection + 1;
		}
	}
	return;
}

void CMenu::processDifficultyMenu()
{
	if( m_selection != -1)
	{
		cleanup();
			m_Difficulty = m_selection;
	}
	return;
}

void CMenu::processNumControlMenu()
{
	if( m_selection != -1)
	{
		if( m_selection < MAX_PLAYERS )
		{
			m_NumPlayers = m_selection + 1;
		}
	}
	return;
}

void CMenu::processDebugMenu()
{
	return;
}

void CMenu::processModMenu()
{
	return;
}

void CMenu::processSaveMenu()
{
	std::string text;
	if( m_selection != -1)
	{
		if(mp_Dialog->m_key == 'u')
		{
			if(mp_Dialog->m_name == "     EMPTY       ")
			{
				mp_Dialog->m_name = "";
				mp_Dialog->m_key = 't';
				m_selection = -1;
			}
			else if(m_overwrite == true)
			{
				mp_Dialog->processInput(int(m_lastselect));
				mp_Dialog->m_name = "";
				mp_Dialog->m_key = 't';
				m_selection = -1;
				m_lastselect = -1;
				m_overwrite = false;
			}
			else if(mp_Dialog->m_name != "")
			{
				m_lastselect = m_selection;
				init(OVERWRITE);
			}
			else
			{
				mp_Dialog->m_key = 't';
				m_selection = -1;
			}
			mp_Dialog->m_length = 15;
		}
		else if (mp_Dialog->m_key == 't')
		{
			if(mp_Dialog->m_name == "")
			{
				mp_Dialog->setObjectText(m_selection, "Untitled");
				mp_Dialog->m_name = "Untitled";
				m_saveslot = int(m_selection) + 1;
				m_SavedGame.prepareSaveGame(m_saveslot, mp_Dialog->m_name);
			}
			else
			{
				mp_Dialog->setObjectText(m_selection, mp_Dialog->m_name);
				m_saveslot = int(m_selection) + 1;
				m_SavedGame.prepareSaveGame(m_saveslot, mp_Dialog->m_name);
			}
			mp_Dialog->m_key = 'u';
			m_goback2 = true;
		}
	}
	else
	{
		m_selection = m_lastselect;
	}

	if(m_goback2)
	{
		cleanup();
		init(MAIN);
		m_goback = true;
	}
	return;
}

void CMenu::processLoadMenu()
{
	if( m_selection != -1)
	{
		if(mp_Dialog->m_name == "     EMPTY       ")
		{
			//TODO: Message saying can't load, it is empty.  Also, we need to add a check to see if it is corrupt, or something to prevent old saves from crashing due to incompatibility.
		}
		else
		{
			m_saveslot = int(m_selection) + 1;
			m_SavedGame.prepareLoadGame(m_saveslot);
			m_goback2 = true;
		}
	}

	if(m_goback2)
	{
		cleanup();
		init(MAIN);
		m_goback = true;
	}
	return;
}

void CMenu::processOverwriteMenu()
{
	if( m_selection != -1)
	{
		if ( m_selection == 1 )
		{
			m_overwrite = true;
			m_goback = true;
		}
		else if ( m_selection == 2 )
		{
			m_overwrite = false;
			m_goback = true;
			m_lastselect = -1;
		}
	}
	return;
}

void CMenu::processQuitMenu()
{
	if( m_selection != -1)
	{
		if ( m_selection == 1 )
		{
			cleanup();
			m_quit = true;
		}
		else if ( m_selection == 2 )
		{
			m_goback = true;
		}
	}
	return;
}

void CMenu::processEndGameMenu()
{
	if( m_selection != -1)
	{
		if ( m_selection == 1 )
		{
			cleanup();
			m_Endgame = true;
		}
		else if ( m_selection == 2 )
		{
			m_goback = true;
		}
	}
	return;
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
