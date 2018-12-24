/*
 * CGamePlayMode.cpp
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#include "CGamePlayMode.h"
#include "sdl/audio/music/CMusicPlayer.h"
#include <base/GsTimer.h>
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>
#include <base/GsApp.h>
#include <base/GsEventContainer.h>
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


void CGamePlayMode::pumpEvent(const CEvent *evPtr)
{
    mp_PlayGame->pumpEvent(evPtr);
}

void CGamePlayMode::ponder(const float deltaT)
{
	// The player is playing the game. It also includes scenes like ending
    CEventContainer& EventContainer = gEventManager;

    mp_PlayGame->ponder(deltaT);

	if( mp_PlayGame->getEndGame() )
	{
		m_startLevel = 0;
        EventContainer.add( new GMSwitchToPassiveMode() );
	}
	else if( mp_PlayGame->getStartGame() )
	{ // Start another new game
        EventContainer.add( new GMSwitchToPlayGameMode(m_Episode,
                                                       m_DataDirectory) );
	}
	else if( mp_PlayGame->getExitEvent() )
	{
		EventContainer.add( new GMQuit() );
	}

}


void CGamePlayMode::render()
{
    mp_PlayGame->render();    

}
