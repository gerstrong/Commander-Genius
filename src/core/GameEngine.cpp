/*
 * GameEngine.cpp
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include <base/CInput.h>
#include <widgets/GsMenuController.h>


#include "GameEngine.h"
#include "common/CBehaviorEngine.h"
#include "mode/CGamePlayMode.h"
#include "core/mode/CGameMode.h"
/*#include "common/Menu/CSelectionMenu.h"
#include "common/Menu/CMainMenu.h"
#include "common/Menu/CHelpMenu.h"*/



void GameEngine::pumpEvent(const CEvent *evPtr)
{
    // process any triggered Game Main related event
    //auto &EventContainer = gEventManager;

    /*if( dynamic_cast<const GMSwitchToPassiveMode*>(evPtr) )
    {
        std::unique_ptr<CGamePassiveMode> passive(new CGamePassiveMode());
        mpGameMode = move(passive);
        mpGameMode->init();
        mOpenedGamePlay = false;
        gMenuController.emptyMenuStack();
    }
    else *//*if( const GMSwitchToPlayGameMode* p_PlayGame = dynamic_cast<const GMSwitchToPlayGameMode*>(evPtr) )
    {
        // TODO: This const_cast must be removed. So adapt the rest of the structure to make it more secure
        GMSwitchToPlayGameMode *playGame = const_cast<GMSwitchToPlayGameMode*>(p_PlayGame);
        std::unique_ptr<CGamePlayMode> gameplay( new CGamePlayMode(*playGame) );
        mpGameMode = move(gameplay);
        mpGameMode->init();
        mOpenedGamePlay = true;
        EventContainer.add( new CloseAllMenusEvent() );
    }*/

    /*else if( dynamic_cast<const LoadGameEvent*>(evPtr) ) // If GamePlayMode is not running but loading is requested...
    {
        // TODO: we need to pass less arguments here! Make this code more pleasant

        const unsigned int ep = g_pBehaviorEngine->getEpisode();
        const std::string &dir = g_pBehaviorEngine->m_ExeFile.getDataDirectory();

        std::unique_ptr<CGamePlayMode> gameplay(new CGamePlayMode( ep, dir, 0));

        gameplay->init();

        gameplay->loadGame();

        mpGameMode = move(gameplay);

        mOpenedGamePlay = true;
        EventContainer.add( new CloseAllMenusEvent() );
    }*/
    /*else*/ if(mpGameMode) // Otherwise send to the existing created mGameMode Object
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
