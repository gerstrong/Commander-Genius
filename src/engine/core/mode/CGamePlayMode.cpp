/*
 * CGamePlayMode.cpp
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#include "CGamePlayMode.h"
#include "graphics/effects/CColorMerge.h"
#include "sdl/audio/music/CMusicPlayer.h"
#include <base/GsTimer.h>
#include <base/video/CVideoDriver.h>
#include <base/GsApp.h>
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
        EventContainer.add( new GMSwitchToPlayGameMode(m_Episode, g_pBehaviorEngine->mPlayers, m_DataDirectory) );
	}
	else if( mp_PlayGame->getExitEvent() )
	{
		EventContainer.add( new GMQuit() );
	}

}

void CGamePlayMode::render()
{
    mp_PlayGame->render();

    if(g_pBehaviorEngine->m_option[OPT_SHOWFPS].value)
    {
        SDL_Rect rect;
        rect.x = 5;
        rect.y = 5;
        rect.w = 150;
        rect.h = 10;

        if(!mpFPSSurface)
        {
            auto *blit = gVideoDriver.getBlitSurface();
            SDL_PixelFormat *format = blit->format;

            mpFPSSurface.reset( SDL_CreateRGBSurface( SDL_SWSURFACE,
                        rect.w,
                        rect.h,
                        RES_BPP,
                        format->Rmask,
                        format->Gmask,
                        format->Bmask,
                        format->Amask ), &SDL_FreeSurface );
        }

        std::string tempbuf = "FPS: " + ftoa(gTimer.LastFPS());
        SDL_FillRect(mpFPSSurface.get(),NULL,0x88888888);
        //gGraphics.getFont(1).drawFont(mpFPSSurface.get(), tempbuf, 1, 1, false);

        BlitSurface(mpFPSSurface.get(), NULL, gVideoDriver.getBlitSurface(), &rect);
    }
}

