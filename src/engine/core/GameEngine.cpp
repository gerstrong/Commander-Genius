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
#include "mode/Scene.h"


GameEngine::~GameEngine()
{

}

void GameEngine::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    if(mpScene) // Otherwise send to the existing created mGameMode Object
    {
        mpScene->pumpEvent(evPtr);
    }
}


void GameEngine::ponder(const float deltaT)
{
    if(mEngineLoader.isRunning())
    {
        mEngineLoader.run(deltaT);
        return;
    }

    if(!mpScene)
        return;

    // Process the game mode object
    mpScene->ponder(deltaT);
}

void GameEngine::render()
{
    if(mEngineLoader.isRunning())
    {
        mEngineLoader.render();
        return;
    }

    if(!mpScene)
        return;

    // Render the game mode object
    mpScene->render();

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

        if(!mFPSSurface)
        {
            mFPSSurface.createRGBSurface(rect);
        }

        std::string tempbuf = "FPS: " + ftoa(gTimer.LastFPS());

        mFPSSurface.fillRGBA(0x88, 0x88, 0x88, 0x88);

        gGraphics.getFontLegacy(1).drawFont(mFPSSurface, tempbuf, 1, 1, false);

        GsWeakSurface blitWeak(gVideoDriver.getBlitSurface());
        mFPSSurface.blitTo(blitWeak, rect);
    }
}
