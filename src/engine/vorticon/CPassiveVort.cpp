/*
 * CPassiveVort.cpp
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#include "CPassiveVort.h"

#include "graphics/CGfxEngine.h"
#include "common/CVorticonMapLoader.h"
#include "common/CTileProperties.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "sdl/extensions.h"
#include "core/mode/CGameMode.h"

namespace vorticon
{

bool CPassiveVort::init(char mode)
{
	m_mode = mode;


	SDL_Surface *temp = CG_CreateRGBSurface( g_pVideoDriver->getGameResolution().SDLRect() );
//#if SDL_VERSION_ATLEAST(2, 0, 0)
    
//#else
    mpTextSfc.reset(g_pVideoDriver->convertThroughBlitSfc(temp), &SDL_FreeSurface);
//#endif
	SDL_FreeSurface(temp);

	if( m_mode == INTRO )
	{
		mpIntroScreen.reset(new CIntro());
		mpMap.reset(new CMap);
		CVorticonMapLoaderBase MapLoader( mpMap );
		MapLoader.load( m_Episode, 90, m_DataDirectory);
		mpMap->gotoPos( 64+5*320, 32); // Coordinates of star sky
		mpMap->drawAll();
		mpIntroScreen->init();
		mpMap->changeTileArrayY(8, 15, 2, 560);
	}
	else if( m_mode == TITLE )
	{
		mpMap.reset(new CMap);
		CVorticonMapLoaderBase MapLoader( mpMap );
		MapLoader.load( m_Episode, 90, m_DataDirectory);
		mpMap->gotoPos( 32, 32 ); // Coordinates of title screen
		mpMap->drawAll();
		mpTitleScreen.reset( new CTitle( *mpMap.get() ) );
		mpTitleScreen->init(m_Episode);
	}
	else if( m_mode == DEMO )
	{
		// TODO: Setup the demo environment
	}
	else
		return false;
	return true;
}

void CPassiveVort::ponder()
{

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	if(!EventContainer.empty())
	{
		if( EventContainer.occurredEvent<ResetScrollSurface>() )
		{
		    g_pVideoDriver->updateScrollBuffer( mpMap );
		    EventContainer.pop_Event();
		    return;
		}
		
		else if( EventContainer.occurredEvent<EventEndGamePlay>() )
		{
		    EventContainer.pop_Event();
		    m_modeg = true;
		}		
	}


	// Modes. We have three: Intro, Main-tile and Demos. We could add more.
	if( m_mode == INTRO )
	{
		// Intro code goes here!
        mpIntroScreen->ponder();

		if( mpIntroScreen->isFinished() )
		{
			// Shutdown mp_IntroScreen and show load Title Screen
			init(TITLE);
		}
	}
	else if( m_mode == TITLE )
	{
        mpTitleScreen->ponder();
	}
	else if( m_mode == DEMO )
	{
		// TODO: Demo modes are processed here!
		// TODO: Implement Demos here!
		init(TITLE);
	}

}

void CPassiveVort::render()
{
    // Process Drawing related stuff
    // Animate the tiles
    mpMap->animateAllTiles();

    // Blit the background
    g_pVideoDriver->blitScrollSurface();

    SDL_BlitSurface( mpTextSfc.get(),
                     nullptr,
                     g_pVideoDriver->getBlitSurface(),
                     nullptr );


    // Modes. We have three: Intro, Main-tile and Demos. We could add more.
    if( m_mode == INTRO )
    {
        // Intro code goes here!
        mpIntroScreen->render();
    }
    else if( m_mode == TITLE )
    {
        mpTitleScreen->render();
    }

}

}

