//MENU.C
//  The main menu, intro, and other such stuff.

#include "CMenu.h"
#include "CObject.h"

#include "../vorticon/infoscenes/CHighScores.h"
#include "../vorticon/infoscenes/CStory.h"
#include "../vorticon/infoscenes/CCredits.h"
#include "../vorticon/infoscenes/COrderingInfo.h"
#include "../vorticon/infoscenes/CAbout.h"
#include "../vorticon/infoscenes/CHelp.h"

#include "Menu/CVideoSettings.h"
#include "Menu/CAudioSettings.h"
#include "Menu/CControlsettings.h"
#include "Menu/COptions.h"

#include "../StringUtils.h"
#include "../CGameControl.h"
#include "../vorticon/CPassive.h"
#include "../sdl/CInput.h"
#include "../sdl/CTimer.h"
#include "../sdl/CSettings.h"
#include "../sdl/sound/CSound.h"
#include "../sdl/CVideoDriver.h"

#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }

#define SELMOVE_SPD         3

CMenu::CMenu( char menu_mode, std::string &GamePath,
			 char &Episode, CMap &Map, CSavedGame &SavedGame,
			 stOption *pOption ) :
m_Episode(Episode),
m_GamePath(GamePath),
m_Map(Map),
m_SavedGame(SavedGame),
mp_option(pOption),
m_RestartVideo(false)
{
	// Create the Main Menu
	mp_MenuSurface = g_pVideoDriver->FGLayerSurface;
	m_menu_mode = menu_mode;
	m_Difficulty = -1; // no difficulty chosen...
	m_NumPlayers = 0; // no player chosen...
	
	m_lastselect = -1;
	m_demoback = false;
	m_overwrite = false;
	m_quit = false;
	m_choosegame = false;
	m_goback = false;
	m_Endgame = false;
	mp_Dialog = NULL;
	mp_InfoScene = NULL;
	m_hideobjects = false;

	// Special Pointer used for Menus that are declared in separated classes
	mp_Menu = NULL;
}

////
// Initialization Routines
////
bool CMenu::init( char menu_type )
{
	m_menu_type = menu_type;
	m_goback = false;
	m_selection = -1; // Nothing has been selected yet.
	mp_MenuSurface = g_pVideoDriver->FGLayerSurface;
	if(mp_Dialog)
		mp_Dialog->setSDLSurface(mp_MenuSurface);

	if( m_menu_type == MAIN )
		initMainMenu();
	else if( m_menu_type == QUIT )
		initConfirmMenu();
	else if( m_menu_type == ENDGAME )
		initConfirmMenu();
	else if( m_menu_type == NEW )
		initNumPlayersMenu();
	else if( m_menu_type == DIFFICULTY )
		initDifficultyMenu();
	else if( m_menu_type == CONFIGURE )
		initConfigureMenu();
	else if( m_menu_type == CONTROLPLAYERS )
		initNumControlMenu();
	else if( m_menu_type == CONTROLS )
	{
		mp_Menu = new CControlsettings(m_menu_type, m_NumPlayers);
		return true;
	}
	else if( m_menu_type == F1 )
		initF1Menu();
	else if( m_menu_type == SAVE )
		initSaveMenu();
	else if( m_menu_type == LOAD )
		initSaveMenu();
	else if( m_menu_type == OVERWRITE )
		initConfirmMenu();
	else if( m_menu_type == GRAPHICS )
	{
		mp_Menu = new CVideoSettings(m_menu_type);
		return true;
	}
	else if( m_menu_type == OPTIONS )
	{
		mp_Menu = new COptions(m_menu_type, mp_option);
		return true;
	}
	else if( m_menu_type == AUDIO )
	{
		mp_Menu = new CAudioSettings(m_menu_type, m_GamePath, m_Episode);
		return true;
	}
	
	// Use the standard Menu-Frame used in the old DOS-Games
	mp_Dialog->setFrameTheme( DLG_THEME_OLDSCHOOL );
	
	return true;
}

void CMenu::initMainMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 17, 10);
	
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
	}
}

void CMenu::initNumPlayersMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 13, MAX_PLAYERS+2);
	int i;
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "1 Player");
	for(i=2;i<=MAX_PLAYERS;i++)
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, itoa(i)+" Player");
	}
}

void CMenu::initDifficultyMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 11, 5);
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Easy");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Normal");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Hard");
}

void CMenu::initConfigureMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 13, 6);
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Graphics");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Audio");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Options");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Controls");
}

void CMenu::initNumControlMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 13, MAX_PLAYERS+2);
	int i;
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Player 1");
	for(i=2;i<=MAX_PLAYERS;i++)
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, "Player "+itoa(i));
	}
}

void CMenu::initF1Menu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 18, 9);
	
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "The Menu");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "The Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "The Story");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Ordering Info");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "About ID");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 6, "About CG");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 7, "Credits");
	
	// In the Help system let's hide all objects like Bitmaps, players, enemies, etc.
}

void CMenu::initConfirmMenu()
{
	mp_Dialog = new CDialog(mp_MenuSurface, 0, 0, 22, 5, 'l');
	
	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, 1, "  Are you certain?  ");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Yes");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 15, 3, "No");
}

void CMenu::initSaveMenu()
{
	std::string text;
	mp_Dialog = new CDialog(mp_MenuSurface, 0, 0, 22, 12, 'u');
	
	// Load the state-file list
	m_StateFileList = m_SavedGame.getSlotList();
	
	for(Uint32 i=1;i<=10;i++)
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
// Process Routines
////
/// Main Process method fo the menu
void CMenu::process()
{
	// Information Mode?
	if(!mp_InfoScene) // show a normal menu
	{
		if( g_pInput->getPressedCommand(IC_HELP) )
		{
			cleanup();
			init(F1);
		}

		if( mp_Menu )
		{
			mp_Menu->processCommon();

			mp_Menu->processSpecific();

			mp_Menu->postProcess();

			if(mp_Menu->mustClose())
			{
				m_RestartVideo=mp_Menu->restartVideo();
				SAFE_DELETE(mp_Menu);
				init(m_menu_type);
			}
		}
		else
		{
			// Get Input for selection
			if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) )
			{
				m_selection = mp_Dialog->getSelection();
			}
			else if( g_pInput->getPressedCommand(IC_QUIT) )
			{
				m_goback = true;
			}
			mp_Dialog->processInput();

			// Process the Menu Type logic.
			// Which menu is open and what do we have to do?

			// NOTE: Some menus are now inherited classes by a base-class
			// Some variables must be hidden, and it can be achieved through that method
			// I (Gerstrong) would recommend making the others menu also this way.
			// You'd have more files but small ones and better to control than a big one, where
			// you have to seek and seek

			if( m_menu_type == MAIN ) processMainMenu();
			else if( m_menu_type == NEW ) processNumPlayersMenu();
			else if( m_menu_type == DIFFICULTY ) processDifficultyMenu();
			else if( m_menu_type == CONFIGURE ) processConfigureMenu();
			else if( m_menu_type == CONTROLPLAYERS ) processNumControlMenu();
			else if( m_menu_type == F1 ) processF1Menu();
			else if( m_menu_type == QUIT ) processQuitMenu();
			else if( m_menu_type == ENDGAME ) processEndGameMenu();
			else if( m_menu_type == SAVE ) processSaveMenu();
			else if( m_menu_type == LOAD ) processLoadMenu();
			else if( m_menu_type == OVERWRITE ) processOverwriteMenu();

			// Draw the menu
			if(!mp_Menu && mp_Dialog) mp_Dialog->draw();
		}
	}
	else // InfoScene is enabled! show it instead of the menu
	{
		mp_InfoScene->process();
		
		if(mp_InfoScene->destroyed())
		{
			SAFE_DELETE(mp_InfoScene); // Destroy the InfoScene and go back to the menu!!!
									   // Restore the old map, that was hidden behind the scene
			g_pInput->flushAll();
			g_pVideoDriver->setScrollBuffer(&m_Map.m_scrollx_buf, &m_Map.m_scrolly_buf);
			m_Map.drawAll();
			m_Map.m_animation_enabled = true;
			m_hideobjects = false;
		}
	}
}

void CMenu::processMainMenu()
{	
	if( m_selection != -1)
	{
		if( m_menu_mode == PASSIVE )
		{
			if( m_selection == 5 ) // Back to Demo
			{
				m_demoback = true;
			}
			if( m_selection == 6 ) // Choose Game
			{
				m_choosegame = true;
			}
		}
		else if( m_menu_mode == ACTIVE )
		{
			if( m_selection == 5 ) // Back to Game
			{
				m_goback = true;
			}
			if( m_selection == 6 ) // End Game
			{
				cleanup();
				init(ENDGAME);
			}
		}
		
		if( m_selection == 0 ) // Start Game
		{
			cleanup();
			init(NEW);
		}
		if( m_selection == 1 ) // Load Game
		{
			cleanup();
			init(LOAD);
		}
		if( m_selection == 2 ) // Save Game
		{
			cleanup();
			init(SAVE);
		}
		if( m_selection == 3 ) // Show Highscores
		{
			m_hideobjects = true;
			m_Map.m_animation_enabled = false;
			mp_InfoScene = new CHighScores(m_Episode, m_GamePath, false);
			m_selection = -1;
		}
		if( m_selection == 4 ) // Options
		{
			cleanup();
			init(CONFIGURE);
		}
		if( m_selection == 7 ) // Quit
		{
			cleanup();
			init(QUIT);
		}
	}
	
	if( m_menu_mode == PASSIVE )
	{
		if(m_goback)
		{
			cleanup();
			init(QUIT);
		}
	}
}

void CMenu::processNumPlayersMenu()
{
	if( m_selection != -1)
	{
		cleanup();
		if( m_selection < MAX_PLAYERS )
		{
			m_NumPlayers = m_selection + 1;	
			init(DIFFICULTY);
		}
		else
		{
			m_goback = true;
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
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
	
	if(m_goback)
	{
		cleanup();
		init(NEW);
	}
	return;
}

void CMenu::processConfigureMenu()
{
	if( m_selection != -1)
	{
		if ( m_selection == 0 )
		{
			cleanup();
			init(GRAPHICS);
		}
		else if ( m_selection == 1 )
		{
			cleanup();
			init(AUDIO);
		}
		else if ( m_selection == 2 )
		{
			cleanup();
			init(OPTIONS);
		}
		else if ( m_selection == 3 )
		{
			cleanup();
			init(CONTROLPLAYERS);
		}
		else
		{
			m_goback = true;	
		}
		m_selection = -1;
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
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
			cleanup();
			init(CONTROLS);
		}
		else
		{
			m_goback = true;	
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(CONFIGURE);
	}
	return;
}

void CMenu::processF1Menu()
{
	if( m_selection != -1)
	{
		m_Map.m_animation_enabled = false;
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
				m_hideobjects = true;
				mp_InfoScene = new CStory(m_GamePath, m_Episode);
				break;
			case 3:
				m_hideobjects = true;
				mp_InfoScene = new COrderingInfo(m_GamePath, m_Episode);
				break;
			case 4:
				m_hideobjects = true;
				mp_InfoScene = new CAbout(m_GamePath, m_Episode, "ID");
				break;
			case 5:
				m_hideobjects = true;
				mp_InfoScene = new CAbout(m_GamePath, m_Episode, "CG");
				break;
			case 6:
				m_hideobjects = true;
				mp_InfoScene = new CCredits(m_GamePath, m_Episode);
				break;
		}
		m_selection = -1;
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

void CMenu::processQuitMenu()
{
	mp_Dialog->setObjectText(0, "   Quit the game?   ");
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
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

void CMenu::processEndGameMenu()
{
	mp_Dialog->setObjectText(0, "   End your game?   ");
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
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

// TODO: PLease put more comments in order to understand what is supposed to be done.
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
				cleanup();
				init(OVERWRITE);
			}
			else
			{
				mp_Dialog->m_key = 't';
				m_selection = -1;
			}
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
			m_selection = -1;
		}
	}
	else
	{
		m_selection = m_lastselect;
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

void CMenu::processLoadMenu()
{
	if( m_selection != -1)
	{
		m_saveslot = int(m_selection) + 1;
		m_SavedGame.prepareLoadGame(m_saveslot);
		m_selection = -1;
	}
	
	if(m_goback)
	{
		cleanup();
		init(MAIN);
	}
	return;
}

// TODO: You really should see to get that dialog type (Yes/No) making it more universal and templatized.
// If you don't you may loose the your own insight into the code.
void CMenu::processOverwriteMenu()
{
	mp_Dialog->setObjectText(0, "Overwrite this save?");
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
		}
	}
	
	if(m_goback)
	{
		cleanup();
		init(SAVE);
	}
	return;
}

////
// Cleanup Routines
////
void CMenu::cleanup()
{
	m_Map.m_animation_enabled = true;
	m_hideobjects = false;
	// Close the old menu
	SAFE_DELETE(mp_Menu);
	SAFE_DELETE(mp_Dialog);
}

CMenu::~CMenu()
{
	SAFE_DELETE(mp_Menu);
	SAFE_DELETE(mp_InfoScene);
}
