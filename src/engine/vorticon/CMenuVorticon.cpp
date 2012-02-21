// MENUVORTICON.C
//  The main menu, intro, and other such stuff.

#include "CMenuVorticon.h"

#include "engine/CPassive.h"

#include "StringUtils.h"
#include "core/CGameControl.h"
#include "CLogFile.h"
#include "sdl/CTimer.h"
#include "sdl/sound/CSound.h"
#include "sdl/CVideoDriver.h"
#include "common/Menu/CHelpMenu.h"

#include "engine/infoscenes/CHighScores.h"

#define SELMOVE_SPD         3

CMenuVorticon::CMenuVorticon( char menu_mode, CMap &Map, CSavedGame &SavedGame,
			 bool &restartVideo, bool &hideobjects ) :
CMenu(menu_mode, SavedGame,
	DLG_THEME_VORTICON, restartVideo),
m_Map(Map),
mp_InfoScene(NULL),
m_hideobjects(hideobjects)
{
	initMainMenu();
}

////
// Process Routines
////
void CMenuVorticon::processSpecific()
{
	// Information Mode?
	if(!mp_InfoScene) // show a normal menu
	{
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
