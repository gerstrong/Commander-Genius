/*
 * CGameLauncher.cpp
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#include "CGameLauncher.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"
#include "../graphics/CGfxEngine.h"
#include <dirent.h>

CGameLauncher::CGameLauncher() {
	m_mustquit = false;
	m_hasbeenchosen = false;
	m_numGames = 0;
	m_chosenGame = 0;
}

CGameLauncher::~CGameLauncher() {
	// TODO Auto-generated destructor stub
}

////
// Initialization Routine
////
bool CGameLauncher::init()
{
	// Load the map for the background
	mp_map = new CMap(g_pVideoDriver->getScrollSurface(), g_pGfxEngine->Tilemap);

	if(!mp_map->loadMap(1, 90, "games")) return false;

	mp_map->gotoPos(32,32);

	// Initialize the menu
	mp_LaunchMenu = new CDialog(g_pVideoDriver->FGLayerSurface, 36, 20);

	// Use the standard Menu-Frame used in the old DOS-Games
	mp_LaunchMenu->setFrameTheme( DLG_THEME_OLDSCHOOL );

	// Scan for games...
	if( (m_numGames = scanDirectories()) != 0)
	{
		int i=0;
		// TODO: Check consistency of the games.

		for( i=0 ; i < m_numGames ; i++ )
			mp_LaunchMenu->addObject(DLG_OBJ_OPTION_TEXT,1,i+1, m_DirList[i]);

		mp_LaunchMenu->addObject(DLG_OBJ_OPTION_TEXT,1,i+1, "Quit");
	}
	else
	{
		mp_LaunchMenu->addObject(DLG_OBJ_DISABLED,1,1, "No games were found!");
		mp_LaunchMenu->addObject(DLG_OBJ_OPTION_TEXT,1,2, "Quit");
	}

	// Draw Background. This is only needed once, since Scrollsurface
	// won't be cleared every update screen
	mp_map->drawAll();

	// You also should be able to exit!

	return true;
}


Uint8 CGameLauncher::scanDirectories()
{
	// TODO: This implementation must be adapted to the searchpath system!!
	DIR* games_root;
	std::string buffer;

	games_root = opendir("games");

	struct dirent *dp;

	if(!games_root)
		return 0;

	while( (dp = readdir(games_root)) != NULL )
	{
		if(dp->d_type == DT_DIR)
		{
			buffer = dp->d_name;
			if(buffer[0] != '.') // no ., .., or hidden files please
				m_DirList.push_back(buffer);
		}
	}

	closedir(games_root);
	return m_DirList.size();
}

////
// Process Routine
////
void CGameLauncher::process()
{
	// Gather input states
	if( g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedKey(KENTER) )
	{
		unsigned char selection = mp_LaunchMenu->getSelection();
		if( selection >= m_numGames )
		{
			// outside the number of games, that exist. This means exit was triggered.
			m_mustquit = true;
		}
		else
		{
			m_hasbeenchosen = true;
			m_chosenGame = selection;
		}

	}

	// Did the use press (X)
	if( g_pInput->getExitEvent() )
		m_mustquit = true;

	// Process Menu Input
	mp_LaunchMenu->processInput();

	// Animate the tiles of the map
	g_pGfxEngine->Tilemap->animateAllTiles(g_pVideoDriver->ScrollSurface);

	// Blit the background
	g_pVideoDriver->blitScrollSurface(mp_map->m_scrollx_buf, mp_map->m_scrolly_buf);

	// Draw the Start Menu to the Foreground-layer
	mp_LaunchMenu->draw();
}

// When the game is chosen, read the episode number, by looking which exe is present
Uint8 CGameLauncher::retrievetEpisode(short chosengame)
{
	// TODO: implement search-path here!
	FILE *fp;
	std::string buffer;

	// Detect the right Episode
	buffer = "games/" + m_DirList[chosengame] + "/keen1.exe";
	if((fp = fopen(buffer.c_str(),"rb")) != NULL)
	{
		return 1;
	}

	buffer = "games/" + m_DirList[chosengame] + "/keen2.exe";
	if((fp = fopen(buffer.c_str(),"rb")) != NULL)
	{
		return 2;
	}

	buffer = "games/" + m_DirList[chosengame] + "/keen3.exe";
	if((fp = fopen(buffer.c_str(),"rb")) != NULL)
	{
		return 3;
	}

	return 0;
}


////
// Cleanup Routine
////
void CGameLauncher::cleanup()
{
	// destroy the map
	delete mp_map;

	// destroy the menu
	delete mp_LaunchMenu;
}

