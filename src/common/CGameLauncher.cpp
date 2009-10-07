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
#include <iostream>
#include <fstream>
#include "StringUtils.h"
#include "FindFile.h"

CGameLauncher::CGameLauncher() {
	m_mustquit = false;
	m_hasbeenchosen = false;
	m_numGames = 0;
	m_chosenGame = 0;
	mp_map = NULL;
	mp_LaunchMenu = NULL;
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

	if(!mp_map->loadMap(1, 90, "games/EP1")) return false;

	mp_map->gotoPos(32,32);

	// Initialize the menu
	mp_LaunchMenu = new CDialog(g_pVideoDriver->FGLayerSurface, 36, 20);

	// Use the standard Menu-Frame used in the old DOS-Games
	mp_LaunchMenu->setFrameTheme( DLG_THEME_OLDSCHOOL );

	// Scan for games...
	if( (m_numGames = scanDirectories()) != 0)
	{
		std::string line;
		std::string dir;
		std::string name;
		
		std::ifstream gamescfg; OpenGameFileR(gamescfg, "games/games.cfg");
		int i=0;
		bool both=false;
		// TODO: Check consistency of the games.

		for( i=0 ; i < m_numGames ; i++ )
		{
			if (gamescfg.is_open())
			{
				while ( !both )
				{
					getline (gamescfg,line);
			
					if(strncmp(line.c_str(),"",strlen("")) == 0)
						both=false;
				if(strncmp(line.c_str(),"&Dir=",strlen("&Dir=")) == 0)
				{
					dir = line.substr(strlen("&Dir="));
					both = false;
				}
					if(strncmp(line.c_str(),"/Name=",strlen("/Name=")) == 0)
					{
						name = line.substr(strlen("/Name="));
						both = true;
					}
				}
			}
			
			if (both == true)
			{
				if (m_DirList[i] == dir)
				{
				mp_LaunchMenu->addObject(DLG_OBJ_OPTION_TEXT,1,i+1, name);
					both = false;
				}
				else
			mp_LaunchMenu->addObject(DLG_OBJ_OPTION_TEXT,1,i+1, m_DirList[i]);
			}
		}
		gamescfg.close();
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

	return true;
}


Uint8 CGameLauncher::scanDirectories()
{
	// TODO: This implementation must be adapted to the searchpath system!!
	DIR* games_root;
	std::string buffer;

	#if defined(__APPLE__)
	games_root = opendir("Commander Genius.app/Contents/Resources/data/games");
	#else
	games_root = opendir("games");
	#endif
	
	struct dirent *dp;

	if(!games_root)
		return 0;

	while( (dp = readdir(games_root)) != NULL )
	{
		if(dp->d_type == DT_DIR)
		{
			buffer = dp->d_name;
			if(buffer[0] != '.') // no ".", "..", or hidden files, please!
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

	// Did the user press (X)?
	if( g_pInput->getExitEvent() )
		m_mustquit = true;

	// Process Menu Input
	mp_LaunchMenu->processInput();

	// Animate the tiles of the map
	g_pGfxEngine->Tilemap->animateAllTiles(g_pVideoDriver->ScrollSurface);

	// Blit the background
	g_pVideoDriver->blitScrollSurface(mp_map->m_scrollx_buf, mp_map->m_scrolly_buf);

	// Draw the Start-Menu
	mp_LaunchMenu->draw();
}

// When the game is chosen, read the episode number by looking which exe file is present
Uint8 CGameLauncher::retrievetEpisode(short chosengame)
{
	// TODO: implement search-path here!
	FILE *fp;
	std::string buffer;

	// Detect the right Episode
#if defined(__APPLE__)
	buffer = "Commander Genius.app/Contents/Resources/data/games/" + m_DirList.at(chosengame) + "/keen1.exe";
	if((fp = fopen(buffer.c_str(),"rb")) != NULL)
	{
		return 1;
	}
	
	buffer = "Commander Genius.app/Contents/Resources/data/games/" + m_DirList[chosengame] + "/keen2.exe";
	if((fp = fopen(buffer.c_str(),"rb")) != NULL)
	{
		return 2;
	}
	
	buffer = "Commander Genius.app/Contents/Resources/data/games/" + m_DirList[chosengame] + "/keen3.exe";
	if((fp = fopen(buffer.c_str(),"rb")) != NULL)
	{
		return 3;
	}
#else
	buffer = "games/" + m_DirList.at(chosengame) + "/keen1.exe";
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
#endif

	return 0;
}


////
// Cleanup Routine
////
void CGameLauncher::cleanup()
{
	// destroy the map
	if (mp_map) delete mp_map, mp_map = NULL;

	// destroy the menu
	if (mp_LaunchMenu) delete mp_LaunchMenu, mp_LaunchMenu = NULL;
}

