/*
 * CPassive.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPassive.h"

#include "graphics/CGfxEngine.h"
#include "graphics/effects/CPixelate.h"
#include "graphics/effects/CColorMerge.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"


namespace galaxy
{

CPassiveGalaxy::CPassiveGalaxy() :
processMode(&CPassiveGalaxy::processIntro),
m_BackgroundBitmap(*g_pGfxEngine->getBitmap("TITLE")),
mp_Menu(new CMenuGalaxy(PASSIVE, m_SavedGame, m_restartVideo))
{ }

bool CPassiveGalaxy::init(char mode)
{
	// TODO: Coding has still to be done!!

	return true;
};

// This function is always called from the base class.
// Here it will execute the mode we are currently running
void CPassiveGalaxy::process()
{
	(this->*processMode)();
}

// This will show the animation of the intro you see in every galaxy game...
// Letters are big and scrolling around the screen...
void CPassiveGalaxy::processIntro()
{
	processMode = &CPassiveGalaxy::processTitle;

	m_BackgroundBitmap = *g_pGfxEngine->getBitmap("TITLE");

	g_pGfxEngine->pushEffectPtr(new CPixelate(2));
}

// Just show the title screen with the pixelation effect
void CPassiveGalaxy::processTitle()
{
	// draw the title bitmap here!
	m_BackgroundBitmap.draw(g_pVideoDriver->mp_VideoEngine->getBlitSurface(), 0, 0);
	
	if ( g_pInput->getPressedAnyKey() || g_pInput->getPressedAnyCommand() )
	{
		if(g_pGfxEngine->applyingEffects())
			g_pGfxEngine->killEffect();
		else
		{
			// Now we process the Galaxy Menu
			processMode = &CPassiveGalaxy::processMenu;
			m_BackgroundBitmap = *g_pGfxEngine->getBitmap("KEENSWATCH");
			g_pGfxEngine->pushEffectPtr(new CColorMerge(8));
		}
	}
}

// This will call the menu in Passive mode and let the player choose a game
// load one, changes options, etc.
void CPassiveGalaxy::processMenu()
{
	// draw the background!
	m_BackgroundBitmap.draw(g_pVideoDriver->mp_VideoEngine->getBlitSurface(), 0, 0);

	mp_Menu->process();

	if ( g_pInput->getPressedAnyKey() || g_pInput->getPressedAnyCommand() )
	{
		if(g_pGfxEngine->applyingEffects())
			g_pGfxEngine->killEffect();
	}

	if(mp_Menu->getExitEvent())
	{
		SAFE_DELETE(mp_Menu);
		cleanup();
		m_mode = SHUTDOWN;
	}
	else if(mp_Menu->getChooseGame())
	{
		SAFE_DELETE(mp_Menu);
		m_modeg = true;
	}
	else if(m_restartVideo) // When some video settings has been changed
	{
		m_restartVideo = false;
	}
	else if(mp_Menu->mustStartGame() || m_SavedGame.getCommand() == CSavedGame::LOAD) // Start a normal game
	{
		m_NumPlayers = mp_Menu->getNumPlayers();
		m_Difficulty = mp_Menu->getDifficulty();
		SAFE_DELETE(mp_Menu);
		m_mode = STARTGAME;
	}

}

}
