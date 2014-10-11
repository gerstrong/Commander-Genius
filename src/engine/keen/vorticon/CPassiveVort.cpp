/*
 * CPassiveVort.cpp
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#include "CPassiveVort.h"

#include "graphics/GsGraphics.h"
#include "CVorticonMapLoader.h"
#include "fileio/CTileProperties.h"
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include "engine/CGameLauncher.h"
#include "sdl/extensions.h"
#include "engine/core/mode/CGameMode.h"

namespace vorticon
{

bool CPassiveVort::init()
{    
    const GsRect<Uint16> gameRect(320, 200);
    gVideoDriver.setNativeResolution(gameRect);

	SDL_Surface *temp = CG_CreateRGBSurface( gVideoDriver.getGameResolution().SDLRect() );
    mpTextSfc.reset(gVideoDriver.convertThroughBlitSfc(temp), &SDL_FreeSurface);
	SDL_FreeSurface(temp);

	if( m_mode == INTRO )
	{
		mpIntroScreen.reset(new CIntro());
		mpMap.reset(new CMap);
		CVorticonMapLoaderBase MapLoader( mpMap );

        if(!MapLoader.load( m_Episode, 90, m_DataDirectory))
        {
            gEventManager.add(new EventEndGamePlay);
        }

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
        mpTitleScreen.reset( new Title( *mpMap.get() ) );
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

void CPassiveVort::pumpEvent(const CEvent *evPtr)
{
    if( dynamic_cast<const ResetScrollSurface*>(evPtr) )
    {
        if(mpMap)
        {
            mpMap->drawAll();
            gVideoDriver.updateScrollBuffer( mpMap->m_scrollx, mpMap->m_scrolly );
            return;
        }
    }
    else
    {
        CPassive::pumpEvent(evPtr);
    }
}

void CPassiveVort::ponder(const float deltaT)
{
	// Modes. We have three: Intro, Main-tile and Demos. We could add more.
	if( m_mode == INTRO )
	{
		// Intro code goes here!
        mpIntroScreen->ponder();

        if( mpIntroScreen->isFinished() )
		{
			// Shutdown mp_IntroScreen and show load Title Screen
            m_mode = TITLE;
            init();
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
        m_mode = TITLE;
        init();
	}

    if(getchooseGame())
    {
        // TODO: Some of game resources are still not cleaned up here!
        gEventManager.add( new GMSwitchToGameLauncher(-1, -1) );
        return;
    }

}

void CPassiveVort::render()
{
    // Process Drawing related stuff
    // Animate the tiles
    mpMap->animateAllTiles();

    // Blit the background
    gVideoDriver.blitScrollSurface();

    BlitSurface( mpTextSfc.get(),
                     nullptr,
                     gVideoDriver.getBlitSurface(),
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

