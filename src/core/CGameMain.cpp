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
			mpGameMode = new CGamePassiveMode();
			mpGameMode->init();
			mOpenedGamePlay = false;
			EventContainer.pop_Event();
		}
		else if( GMSwitchToPlayGameMode* p_PlayGame = EventContainer.occurredEvent<GMSwitchToPlayGameMode>() )
		{
			mpGameMode = new CGamePlayMode( p_PlayGame->m_Episode, p_PlayGame->m_Numplayers,
					p_PlayGame->m_DataDirectory, p_PlayGame->m_startlevel);
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

		else if( EventContainer.occurredEvent<LoadGameEvent>() &&
				 (dynamic_cast<CGamePlayMode*>(mpGameMode.get()) == NULL) ) // If GamePlayMode is not running but loading is requested...
		{
			// In this case let's pop this event and add the same one, the way the loading is finished within the playgame object
			EventContainer.pop_Event();

			switchToGamePlayMode();

			// The same caught event is pushed again but this time it will be polled by the GamePlay object which in the next cycles will be running!
			EventContainer.add( new LoadGameEvent() );
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

	// TODO: At this point some dialogs and menus should be drawn here!

}
