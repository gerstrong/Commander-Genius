/*
 * GameEngine.cpp
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include <base/CInput.h>
#include <widgets/GsMenuController.h>


#include "GameEngine.h"
#include "CBehaviorEngine.h"
#include "mode/CGamePlayMode.h"
#include "mode/CGameMode.h"
/*#include "common/Menu/CSelectionMenu.h"
#include "common/Menu/CMainMenu.h"
#include "common/Menu/CHelpMenu.h"*/



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
}
