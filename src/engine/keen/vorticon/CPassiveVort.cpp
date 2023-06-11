/*
 * CPassiveVort.cpp
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#include "CPassiveVort.h"

#include "CVorticonMapLoader.h"
#include "engine/CGameLauncher.h"

#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <engine/core/GameEngine.h>
#include <engine/core/mode/CGameMode.h>
#include <engine/core/VGamepads/vgamepadsimple.h>

namespace vorticon
{

bool CPassiveVort::init()
{
    mTextSfc.createRGBSurface(gVideoDriver.getGameResolution().SDLRect());
    mTextSfc.makeBlitCompatible();

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
    {
        return false;
    }

#ifdef USE_VIRTUALPAD
    gInput.mpVirtPad.reset(new VirtualKeenControl);
    gInput.mpVirtPad->init();
#endif

    gInput.flushAll();

    return true;
}

void CPassiveVort::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    if( std::dynamic_pointer_cast<const ResetScrollSurface>(evPtr) )
    {
        if(mpMap)
        {
            mpMap->drawAll();
            const auto scroll = mpMap->getMainScrollCoords();
            gVideoDriver.updateScrollBuffer( scroll.x, scroll.y );
            return;
        }
    }
    else
    {
        CPassive::pumpEvent(evPtr);
    }
}

void CPassiveVort::redrawMap()
{
    mpMap->drawAll();
}

void CPassiveVort::ponder(const float)
{
    if(!mBackToGameLauncher)
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
    }

    if(goBackToLauncherSet())
    {
        // TODO: Some of game resources are still not cleaned up here!
        gEventManager.add( new GMSwitchToGameLauncher() );
        return;
    }

}

void CPassiveVort::render()
{
    // Process Drawing related stuff
    // Animate the tiles
    mpMap->animateAllTiles();

    // Blit the background
    gVideoDriver.blitScrollSurfaces();

    GsWeakSurface weakBlit(gVideoDriver.getBlitSurface());
    mTextSfc.blitTo(weakBlit);

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

