/*
 * CGameMain.cpp
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include "CGameMain.h"
#include "common/CBehaviorEngine.h"
#include "mode/CGamePassiveMode.h"
#include "mode/CGamePlayMode.h"
#include "core/mode/CGameMode.h"
#include "common/Menu/CMenuController.h"
#include "common/Menu/CSelectionMenu.h"
#include "sdl/CVideoDriver.h"


void CGameMain::switchToGamePlayMode()
{
	const int episode = g_pBehaviorEngine->getEpisode();

	// If you get here, you always have at least one player
	if(g_pBehaviorEngine->mPlayers <= 0)
		g_pBehaviorEngine->mPlayers = 1;

	const int Numplayers = g_pBehaviorEngine->mPlayers;
	std::string DataDirectory = g_pBehaviorEngine->m_ExeFile.getDataDirectory();
	g_pBehaviorEngine->m_EventList.add( new GMSwitchToPlayGameMode( episode, Numplayers, DataDirectory ) );
}



void CGameMain::init()
{}




void CGameMain::process()
{
	// process any triggered Game Main related event
	CEventContainer &EventContainer = g_pBehaviorEngine->EventList();

	if( !EventContainer.empty() )
	{

		if( EventContainer.occurredEvent<GMSwitchToPassiveMode>() )
		{
		    std::unique_ptr<CGamePassiveMode> passive(new CGamePassiveMode());
		    mpGameMode = move(passive);
		    mpGameMode->init();
		    mOpenedGamePlay = false;
		    EventContainer.pop_Event();
		}
		else if( GMSwitchToPlayGameMode* p_PlayGame = EventContainer.occurredEvent<GMSwitchToPlayGameMode>() )
		{
		    // TODO: we need to pass less arguments here! Make this code more pleasant
		    std::unique_ptr<CGamePlayMode> gameplay( new CGamePlayMode( p_PlayGame->m_Episode, p_PlayGame->m_Numplayers,
									   p_PlayGame->m_DataDirectory, p_PlayGame->m_startlevel) );			
		    mpGameMode = move(gameplay); 
		    mpGameMode->init();
		    mOpenedGamePlay = true;
		    EventContainer.pop_Event();
		    EventContainer.add( new CloseAllMenusEvent() );
		}
		else if( StartInfoSceneEvent *scene = EventContainer.occurredEvent<StartInfoSceneEvent>() )
		{
			gpMenuController->lock(true);
			mpInfoScene = scene->mpScene;
			mpInfoScene->init();

			EventContainer.pop_Event();
			EventContainer.add( new CloseAllMenusEvent() );
			return;
		}
		else if( NewGamePlayersEvent* pNewGame = EventContainer.occurredEvent<NewGamePlayersEvent>() )
		{
			EventContainer.pop_Event();
			g_pBehaviorEngine->mPlayers = pNewGame->mSelection;
			EventContainer.add( new OpenMenuEvent(new CDifficultySelection) );
			return;
		}

		else if( StartNewGameEvent* pStart = EventContainer.occurredEvent<StartNewGameEvent>() )
		{

			EventContainer.pop_Event();
			g_pBehaviorEngine->mDifficulty = pStart->mDifficulty;
			switchToGamePlayMode();
			return;
		}

		else if( EventContainer.occurredEvent<LoadGameEvent>() ) // If GamePlayMode is not running but loading is requested...
		{
		    // TODO: we need to pass less arguments here! Make this code more pleasant
		    std::unique_ptr<CGamePlayMode> gameplay(new CGamePlayMode( g_pBehaviorEngine->getEpisode(), 1, 
							g_pBehaviorEngine->m_ExeFile.getDataDirectory(),0));
			mpGameMode = move(gameplay);

			mpGameMode->init();

			CGamePlayMode *pCGM = dynamic_cast<CGamePlayMode*>(mpGameMode.get());

			pCGM->loadGame();

			mOpenedGamePlay = true;
			EventContainer.pop_Event();
			EventContainer.add( new CloseAllMenusEvent() );
		}


	}

	if( !mpInfoScene.empty() )
	{
		mpInfoScene->process();
		if( mpInfoScene->destroyed() )
		{
			mpInfoScene->teardown();
			mpInfoScene = NULL;
			gpMenuController->lock(false);
		}
	}
	else
	{
		// Process the game mode object
		mpGameMode->process();
	}
}
