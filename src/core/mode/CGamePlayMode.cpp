/*
 * CGamePlayMode.cpp
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#include "CGamePlayMode.h"
#include "engine/galaxy/CPlayGameGalaxy.h"
#include "engine/vorticon/playgame/CPlayGameVorticon.h"
#include "graphics/effects/CColorMerge.h"
#include "sdl/music/CMusicPlayer.h"
#include "sdl/CTimer.h"
#include <memory>

CGamePlayMode::CGamePlayMode(const int Episode,
        const std::string& DataDirectory,
        const int startLevel) :
m_startLevel(startLevel),
m_Episode(Episode),
m_DataDirectory(DataDirectory)
{}

CGamePlayMode::CGamePlayMode( GMSwitchToPlayGameMode &gpmode ) :
m_startLevel(gpmode.m_startlevel),
m_Episode(gpmode.m_Episode),
m_DataDirectory(gpmode.m_DataDirectory)
{}

void CGamePlayMode::init()
{
	CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	// If no level has been set or is out of bound, set it to map.
	if(m_startLevel > 100 || m_startLevel < 0 )
		m_startLevel = WORLD_MAP_LEVEL_VORTICON;

	bool ok = true;

	if(m_Episode >= 4)
	{
		if(m_startLevel == WORLD_MAP_LEVEL_VORTICON)
		{
		    m_startLevel = WORLD_MAP_LEVEL_GALAXY;
		}
        mp_PlayGame.reset( new galaxy::CPlayGameGalaxy( ExeFile, m_startLevel, m_SavedGame) );
	}
	else
	{
		if(m_startLevel == WORLD_MAP_LEVEL_GALAXY)
			m_startLevel = WORLD_MAP_LEVEL_VORTICON;
        mp_PlayGame.reset( new CPlayGameVorticon( ExeFile, m_startLevel, m_SavedGame) );
	}

	// Create the special merge effect (Fadeout)
	CColorMerge *pColorMergeFX = new CColorMerge(8);

	ok &= mp_PlayGame->init();

	g_pGfxEngine->setupEffect(pColorMergeFX);


	if(!ok)
	{
		EventContainer.add( new GMSwitchToPassiveMode(m_DataDirectory, m_Episode));
	}
}


void CGamePlayMode::loadGame()
{
    mp_PlayGame->ponder();

    if(mp_PlayGame->loadXMLGameState())
        return;

    mp_PlayGame->loadGameState();
}

void CGamePlayMode::pumpEvent(const CEvent *evPtr)
{
    mp_PlayGame->pumpEvent(evPtr);
}

void CGamePlayMode::ponder()
{
	// The player is playing the game. It also includes scenes like ending
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

    mp_PlayGame->ponder();
	
	if( EventContainer.occurredEvent<SaveGameEvent>() )
	{
        mp_PlayGame->saveXMLGameState();
        //mp_PlayGame->saveGameState();
        EventContainer.pop_Event();
	}


	if( mp_PlayGame->getEndGame() )
	{
		m_startLevel = 0;
		g_pMusicPlayer->stop();
		EventContainer.add( new GMSwitchToPassiveMode(m_DataDirectory, m_Episode) );
	}
	else if( mp_PlayGame->getStartGame() )
	{ // Start another new game
        EventContainer.add( new GMSwitchToPlayGameMode(m_Episode, g_pBehaviorEngine->mPlayers, m_DataDirectory) );
	}
	else if( mp_PlayGame->getExitEvent() )
	{
		EventContainer.add( new GMQuit() );
	}

}

void CGamePlayMode::render(const float deltaT)
{
    mp_PlayGame->render(deltaT);

    if(g_pBehaviorEngine->m_option[OPT_SHOWFPS].value)
    {
        SDL_Rect rect;
        rect.x = 5;
        rect.y = 5;
        rect.w = 150;
        rect.h = 10;

        if(!mpFPSSurface)
        {
            mpFPSSurface.reset(CG_CreateRGBSurface(rect), &SDL_FreeSurface);
        }

        std::string tempbuf = "FPS: " + ftoa(g_pTimer->LastFPS());
        SDL_FillRect(mpFPSSurface.get(),NULL,0x88888888);
        g_pGfxEngine->getFont(1).drawFont(mpFPSSurface.get(), tempbuf, 1, 1, false);

        SDL_BlitSurface(mpFPSSurface.get(), NULL, g_pVideoDriver->getBlitSurface(), &rect);
    }
}

