/*
 * CGameMain.cpp
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include "CGameMain.h"
#include "common/CBehaviorEngine.h"
#include "common/Menu/CMenuController.h"
#include "mode/CGamePassiveMode.h"
#include "mode/CGamePlayMode.h"
#include "engine/infoscenes/CHighScores.h"
#include "sdl/CVideoDriver.h"

void CGameMain::init()
{

}


void CGameMain::process()
{
	// process any triggered Game Main related event
	CEventContainer &EventContainer = g_pBehaviorEngine->EventList();

	if( !EventContainer.empty() )
	{

		if( GMSwitchToPassiveMode* p_Passive = EventContainer.occurredEvent<GMSwitchToPassiveMode>() )
		{
			mpGameMode = new CGamePassiveMode( p_Passive->m_DataDirectory, p_Passive->m_Episode );
			mpGameMode->init();
			EventContainer.pop_Event();
		}
		else if( GMSwitchToPlayGameMode* p_PlayGame = EventContainer.occurredEvent<GMSwitchToPlayGameMode>() )
		{
			mpGameMode = new CGamePlayMode( p_PlayGame->m_Episode, p_PlayGame->m_Numplayers,
					p_PlayGame->m_Difficulty, p_PlayGame->m_DataDirectory,
					p_PlayGame->m_SavedGame, p_PlayGame->m_startlevel);
			mpGameMode->init();
			EventContainer.pop_Event();
			EventContainer.add( new CloseAllMenusEvent() );
		}
		else if( StartInfoSceneEvent *scene = EventContainer.occurredEvent<StartInfoSceneEvent>() )
		{
			mpInfoScene = scene->mpScene;
			mpInfoScene->init();

			EventContainer.pop_Event();
			EventContainer.add( new CloseAllMenusEvent() );
			return;
		}


	}

	if( !mpInfoScene.empty() )
	{
		mpInfoScene->process();
		if( mpInfoScene->destroyed() )
		{
			mpInfoScene->teardown();
			mpInfoScene.tryDeleteData();
		}

	}
	else
	{
		// Process the game mode object
		mpGameMode->process();
	}

	// TODO: At this point some dialogs and menus should be drawn here!

}
