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

#include "../../StringUtils.h"
#include "../../CGameControl.h"
#include "../../CLogFile.h"
#include "../../sdl/CTimer.h"
#include "../../sdl/CSettings.h"
#include "../../sdl/sound/CSound.h"
#include "../../sdl/CVideoDriver.h"

#define SELMOVE_SPD         3

CMenuVorticon::CMenuVorticon( char menu_mode, std::string &GamePath,
			 char &Episode, CMap &Map, CSavedGame &SavedGame,
			 stOption *pOption ) :
CMenu(menu_mode, GamePath, Episode, SavedGame, pOption, DLG_THEME_VORTICON),
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
	case F1:
		initF1Menu(); processPtr = &CMenuVorticon::processF1Menu; break;
	default:
		processPtr = NULL;
		break;
	}
	return true;
}


////
// Process Routines
////
void CMenuVorticon::processSpecific()
{
	// Information Mode?
	if(!mp_InfoScene) // show a normal menu
	{
		if( g_pInput->getPressedCommand(IC_HELP) )
		{
			cleanup();
			init(F1);
		}
		// Process the Menu Type logic.
		CMenu::process();
		processMainMenu();
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
	if( m_selection != NO_SELECTION)
	{
		if( m_selection == 3 ) // Show Highscores
		{
			m_hideobjects = true;
			m_Map.m_animation_enabled = false;
			mp_InfoScene = new CHighScores(m_Episode, m_GamePath, false);
			m_selection = NO_SELECTION;
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
