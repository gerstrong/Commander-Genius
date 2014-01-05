/*
 * GameEngine.cpp
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include "GameEngine.h"
#include "common/CBehaviorEngine.h"
#include "mode/CGamePassiveMode.h"
#include "mode/CGamePlayMode.h"
#include "core/mode/CGameMode.h"
#include <widgets/CMenuController.h>
#include "common/Menu/CSelectionMenu.h"
#include <base/CInput.h>

#include "common/Menu/CMainMenu.h"
#include "common/Menu/CHelpMenu.h"





void GameEngine::switchToGamePlayMode()
{
	const int episode = g_pBehaviorEngine->getEpisode();

	// If you get here, you always have at least one player
	if(g_pBehaviorEngine->mPlayers <= 0)
		g_pBehaviorEngine->mPlayers = 1;

	const int Numplayers = g_pBehaviorEngine->mPlayers;
	std::string DataDirectory = g_pBehaviorEngine->m_ExeFile.getDataDirectory();
    gEventManager.add( new GMSwitchToPlayGameMode( episode, Numplayers, DataDirectory ) );
}


void GameEngine::pumpEvent(const CEvent *evPtr)
{
    // process any triggered Game Main related event
    auto &EventContainer = g_pBehaviorEngine->EventList();

    if( dynamic_cast<const GMSwitchToPassiveMode*>(evPtr) )
    {
        std::unique_ptr<CGamePassiveMode> passive(new CGamePassiveMode());
        mpGameMode = move(passive);
        mpGameMode->init();
        mOpenedGamePlay = false;
        gMenuController.emptyMenuStack();
    }
    else if( const GMSwitchToPlayGameMode* p_PlayGame = dynamic_cast<const GMSwitchToPlayGameMode*>(evPtr) )
    {
        // TODO: This const_cast must be removed. So adapt the rest of the structure to make it more secure
        GMSwitchToPlayGameMode *playGame = const_cast<GMSwitchToPlayGameMode*>(p_PlayGame);
        std::unique_ptr<CGamePlayMode> gameplay( new CGamePlayMode(*playGame) );
        mpGameMode = move(gameplay);
        mpGameMode->init();
        mOpenedGamePlay = true;
        EventContainer.add( new CloseAllMenusEvent() );
    }
    else if( const StartInfoSceneEvent *scene = dynamic_cast<const StartInfoSceneEvent*>(evPtr) )
    {
        gMenuController.lock(true);
        gMenuController.hide(true);
        mpInfoScene = scene->mpScene;
        mpInfoScene->init();

        return;
    }
    else if( const NewGamePlayersEvent* pNewGame = dynamic_cast<const NewGamePlayersEvent*>(evPtr) )
    {
        g_pBehaviorEngine->mPlayers = pNewGame->mSelection;
        EventContainer.add( new OpenMenuEvent(new CDifficultySelection) );
        return;
    }

    else if( const StartNewGameEvent* pStart = dynamic_cast<const StartNewGameEvent*>(evPtr) )
    {
        g_pBehaviorEngine->mDifficulty = pStart->mDifficulty;
        switchToGamePlayMode();
        return;
    }

    else if( dynamic_cast<const LoadGameEvent*>(evPtr) ) // If GamePlayMode is not running but loading is requested...
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
    }
    else // Otherwise send to the existing created mGameMode Object
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

	if( mpInfoScene )
	{
        mpInfoScene->ponder();
		if( mpInfoScene->destroyed() )
		{		    
			mpInfoScene->teardown();
			mpInfoScene = nullptr;
			gInput.flushAll();
            gMenuController.lock(false);
            gMenuController.hide(false);
		}
	}
	else
	{
		// Process the game mode object
        mpGameMode->ponder(deltaT);
	}
}

void GameEngine::render()
{
    if(mEngineLoader.isRunning())
    {
        mEngineLoader.render();
        return;
    }

    if( !mpInfoScene )
    {
        // Render the game mode object
        mpGameMode->render();
    }
    else
    {
        mpInfoScene->render();
    }

}
