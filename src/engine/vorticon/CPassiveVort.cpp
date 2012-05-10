/*
 * CPassiveVort.cpp
 *
 *  Created on: 07.03.2010
 *      Author: gerstrong
 */

#include "CPassiveVort.h"

#include "graphics/CGfxEngine.h"
#include "common/CMapLoader.h"
#include "common/CTileProperties.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "core/mode/CGameMode.h"

namespace vorticon
{

CPassiveVort::CPassiveVort() :
mp_Option(g_pBehaviorEngine->m_option),
m_RestartVideo(false)
{
	mp_IntroScreen = NULL;
	mp_TitleScreen = NULL;
	m_GoDemo = false;
	m_textsize = 0;
}

bool CPassiveVort::init(char mode)
{
	mp_Scrollsurface = g_pVideoDriver->mp_VideoEngine->getScrollSurface();
	m_mode = mode;

	if( m_mode == INTRO )
	{
		mp_IntroScreen = new CIntro();
		mpMap = new CMap;
		CMapLoader MapLoader( mpMap );
		MapLoader.load( m_Episode, 90, m_DataDirectory);
		mpMap->gotoPos( 64+5*320, 32); // Coordinates of star sky
		mpMap->drawAll();
		mp_IntroScreen->init();
		mpMap->changeTileArrayY(8, 15, 2, 560);
	}
	else if( m_mode == TITLE )
	{
		mpMap = new CMap;
		CMapLoader MapLoader( mpMap );
		MapLoader.load( m_Episode, 90, m_DataDirectory);
		mpMap->gotoPos( 32, 32 ); // Coordinates of title screen
		mpMap->drawAll();
		mp_TitleScreen = new CTitle( *mpMap.get() );
		mp_TitleScreen->init(m_Episode);
	}
	else if( m_mode == DEMO )
	{
		// TODO: Setup the demo environment
	}
	else
		return false;
	return true;
}

void CPassiveVort::process()
{
	// Process Drawing related stuff
	// Animate the tiles
	mpMap->animateAllTiles();

	// Blit the background
	g_pVideoDriver->mDrawTasks.add( new BlitScrollSurfaceTask() );

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	if(!EventContainer.empty())
	{
		if( EventContainer.occurredEvent<ResetScrollSurface>() )
		{
			g_pVideoDriver->updateScrollBuffer( mpMap );
			EventContainer.pop_Event();
			return;
		}
	}


	// Modes. We have three: Intro, Main-tile and Demos. We could add more.
	if( m_mode == INTRO )
	{
		// Intro code goes here!
		mp_IntroScreen->process();

		if( mp_IntroScreen->isFinished() )
		{
			// Shutdown mp_IntroScreen and show load Title Screen
			cleanup();
			init(TITLE);
		}
	}
	else if( m_mode == TITLE )
	{
		mp_TitleScreen->process();
	}
	else if( m_mode == DEMO )
	{
		// TODO: Demo modes are processed here!
		// TODO: Implement Demos here!
		cleanup();
		init(TITLE);
	}
}

void CPassiveVort::cleanup()
{
	/*if(!m_object.empty())
	{
		for(std::vector<CObject*>::iterator obj = m_object.begin() ; obj!=m_object.end() ; obj++)
			SAFE_DELETE(*obj);
		m_object.clear();
	}*/

	SAFE_DELETE(mp_IntroScreen);
	SAFE_DELETE(mp_TitleScreen);
}

}

