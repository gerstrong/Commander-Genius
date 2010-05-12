// MENUVORTICON.C
//  The main menu, intro, and other such stuff.

#include "CMenuVorticon.h"

#include "../../engine/CPassive.h"
#include "../../engine/infoscenes/CHighScores.h"
#include "../../engine/infoscenes/CStory.h"
#include "../../engine/infoscenes/CCredits.h"
#include "../../engine/infoscenes/COrderingInfo.h"
#include "../../engine/infoscenes/CAbout.h"
#include "../../engine/infoscenes/CHelp.h"

#include "../../common/Menu/CVideoSettings.h"
#include "../../common/Menu/CAudioSettings.h"
#include "../../common/Menu/CControlsettings.h"
#include "../../common/Menu/COptions.h"

#include "../../StringUtils.h"
#include "../../CGameControl.h"
#include "../../CLogFile.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CSettings.h"
#include "../../sdl/sound/CSound.h"
#include "../../sdl/CVideoDriver.h"

#define SELMOVE_SPD         3

CMenuVorticon::CMenuVorticon( char menu_mode, std::string &GamePath,
			 char &Episode, CMap &Map, CSavedGame &SavedGame,
			 stOption *pOption ) :
CMenu(menu_mode, GamePath, Episode, SavedGame, pOption),
processPtr(NULL),
m_Map(Map),
mp_InfoScene(NULL)
{

}

////
// Initialization Routines
////
bool CMenuVorticon::init( char menu_type )
{
	cleanup();

	CMenu::init(menu_type);

	switch(m_menu_type)
	{
	case MAIN:
		initMainMenu(); processPtr = &CMenuVorticon::processMainMenu; break;
	case QUIT:
		initConfirmMenu("   Quit the game?   "); processPtr = &CMenuVorticon::processQuitMenu; break;
	case ENDGAME:
		initConfirmMenu("   End your game?   "); processPtr = &CMenuVorticon::processEndGameMenu; break;
	case OVERWRITE:
		initConfirmMenu("Overwrite this save?"); processPtr = &CMenuVorticon::processOverwriteMenu; break;
	case NEW:
		initNumPlayersMenu(); processPtr = &CMenuVorticon::processNumPlayersMenu; break;
	case DIFFICULTY:
		initDifficultyMenu(); processPtr = &CMenuVorticon::processDifficultyMenu; break;
	case CONFIGURE:
		initConfigureMenu(); processPtr = NULL; break;
	case CONTROLPLAYERS:
		initNumControlMenu(); processPtr = &CMenuVorticon::processNumControlMenu; break;
	case CONTROLS:
		mp_Menu = new CControlsettings(m_menu_type, m_NumPlayers);
		return true;
	case F1:
		initF1Menu(); processPtr = &CMenuVorticon::processF1Menu; break;
	case MENU_DEBUG:
		initDebugMenu(); processPtr = &CMenuVorticon::processDebugMenu; break;
	case MODCONF:
		initModMenu(); processPtr = &CMenuVorticon::processModMenu; break;
	case SAVE:
		initSaveMenu(); processPtr = &CMenuVorticon::processSaveMenu; break;
	case LOAD:
		initSaveMenu(); processPtr = &CMenuVorticon::processLoadMenu; break;
	case GRAPHICS:
	case BOUNDS:
		mp_Menu = new CVideoSettings(m_menu_type);
		return true;
	case OPTIONS:
		mp_Menu = new COptions(m_menu_type, mp_option);
		return true;
	case AUDIO:
	case VOLUME:
		mp_Menu = new CAudioSettings(m_menu_type, m_GamePath, m_Episode);
		return true;
	}
	
	// Use the standard Menu-Frame used in the old DOS-Games
	mp_Dialog->setFrameTheme( DLG_THEME_OLDSCHOOL );
	
	return true;
}


////
// Process Routines
////
/// Main Process method fo the menu
void CMenuVorticon::process()
{
	// Information Mode?
	if(!mp_InfoScene) // show a normal menu
	{
		if( g_pInput->getPressedCommand(IC_HELP) )
		{
			cleanup();
			init(F1);
		}

		if( g_pInput->getHoldedKey(KM) && g_pInput->getHoldedKey(KO) && g_pInput->getHoldedKey(KD) )
		{
			cleanup();
			init(MODCONF);
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
			if( mp_Dialog->m_key == 'l' )
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
				m_goback = true;
			}
			mp_Dialog->processInput();

			// Process the Menu Type logic.
			// Which menu is open and what do we have to do?

			// NOTE: Some menus are now inherited classes by a base-class
			// Some variables must be hidden, and it can be achieved through that method
			// I (Gerstrong) would recommend making the others menu also this way.
			// You'd have more files but small ones and better to control than a big one, where
			// you have to seek and sneek
			CMenu::process();

			if(processPtr != NULL)
				(this->*processPtr)();

			// Draw the menu
			if(!mp_Menu && mp_Dialog) mp_Dialog->draw();
			if(m_goback && m_menu_type != MAIN)
			{
				init(m_menuback[m_menu_type]);
			}
			for( std::map<int, int>::iterator iter = m_menumap.begin(); iter != m_menumap.end(); ++iter ) {
				if( m_selection == (*iter).first )
				{
					init((*iter).second);
					break;
				}
			}
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

void CMenuVorticon::processMainMenu()
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
		}
		if( m_selection == 3 ) // Show Highscores
		{
			m_hideobjects = true;
			m_Map.m_animation_enabled = false;
			mp_InfoScene = new CHighScores(m_Episode, m_GamePath, false);
			m_selection = -1;
		}
	}
	
	if( m_menu_mode == PASSIVE )
	{
		if(m_goback)
		{
			init(QUIT);
		}
	}
}


void CMenuVorticon::processF1Menu()
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
	return;
}

////
// Cleanup Routines
////
void CMenuVorticon::cleanup()
{
	CMenu::cleanup();
	m_Map.m_animation_enabled = true;
}

CMenuVorticon::~CMenuVorticon()
{
	cleanup();
	SAFE_DELETE(mp_InfoScene);
}
