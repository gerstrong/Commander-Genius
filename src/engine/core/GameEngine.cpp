/*
 * GameEngine.cpp
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include <base/CInput.h>
#include <base/GsTimer.h>
#include <base/video/CVideoDriver.h>
#include <widgets/GsMenuController.h>
#include <graphics/GsGraphics.h>


#include "GameEngine.h"
#include "CBehaviorEngine.h"
#include "mode/CGamePlayMode.h"
#include "mode/CGameMode.h"


void GameEngine::pumpEvent(const CEvent *evPtr)
{
    if(mpGameMode) // Otherwise send to the existing created mGameMode Object
    {
        mpGameMode->pumpEvent(evPtr);
    }
}


void GameEngine::ponder(const float deltaT)
{
    if(mEngineLoader.isRunning())
    {
        mEngineLoader.run(deltaT);
        return;
    }

    if(!mpGameMode)
        return;

    // Process the game mode object
    mpGameMode->ponder(deltaT);
}

void GameEngine::render()
{
    if(mEngineLoader.isRunning())
    {
        mEngineLoader.render();
        return;
    }

    // Render the game mode object
    mpGameMode->render();

    // FPS Display
    renderFPSDisplay();
}




void GameEngine::renderFPSDisplay()
{
    const auto optFPS = gBehaviorEngine.mOptions[GameOption::SHOWFPS];

    if(optFPS.value)
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

        gGraphics.getFont(1).drawFont(mpFPSSurface.get(), tempbuf, 1, 1, false);

        BlitSurface(mpFPSSurface.get(), NULL, gVideoDriver.getBlitSurface(), &rect);
    }
}
