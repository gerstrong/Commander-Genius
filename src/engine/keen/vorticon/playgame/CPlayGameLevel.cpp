/*
 * CPlayGameLevel.cpp
 *
 *  Created on: 05.11.2009
 *      Author: gerstrong
 */

#include "CPlayGameVorticon.h"
#include "../ai/CManglingMachine.h"
#include "engine/core/objenums.h"
#include "sdl/audio/Audio.h"
#include "../finale/CTantalusRay.h"

void CPlayGameVorticon::processInLevel()
{
	if(m_gameover)
		return;

    const int numPlayers = g_pBehaviorEngine->mPlayers;

	// Perform player Objects...
    for( int i=0 ; i<numPlayers ; i++ )
	{
		// check if someone has lives
		if(m_Player[i].inventory.lives==0 && m_Player[i].pdie==PDIE_DEAD)
			continue;

		// Process the other stuff like, items, jump, etc.
		m_Player[i].processInLevel();

		// If the player touched a hint trigger in which we have to show a Message, do it so
		std::string hinttext;
		if( (hinttext=m_Player[i].pollHintMessage()) != "")
		{
		    std::unique_ptr<CMessageBoxVort> msg( new CMessageBoxVort(g_pBehaviorEngine->getString(hinttext), false, true) );
		    mMessageBoxes.push_back( move(msg) );
		}

		// Check if the first player is dead, and if the others also are...
		if(i==0) m_alldead = (m_Player[i].pdie == PDIE_DEAD);
		else m_alldead &= (m_Player[i].pdie == PDIE_DEAD);

		// Now draw the player to the screen
		m_Player[i].SelectFrame();

		// If Player has toggled a switch for platform extend it!
		LEVEL_TRIGGER trigger = m_Player[i].pollLevelTrigger();
		if( trigger != LVLTRIG_NONE )
		{
			processLevelTrigger(trigger);
		}

		// finished the level
		if(m_Player[i].level_done == LEVEL_COMPLETE)
		{	
		    if(!mSpriteObjectContainer.empty())
			mSpriteObjectContainer.clear();
			mpLevelCompleted[m_Level] = true;
			goBacktoMap();
			break;
		}
		else if(m_Player[i].level_done == LEVEL_TELEPORTER)
		{	// This happens, when keen used the inlevel teleporter...
			goBacktoMap();
			teleportPlayerFromLevel(m_Player[i], m_checkpoint_x, m_checkpoint_y);
			break;
		}
	}

	// Check if all players are dead. In that case, go back to map
	if(m_alldead)
	{
		g_pMusicPlayer->stop();
		m_gameover = true; // proof contrary case
        for( int i=0 ; i<numPlayers ; i++ )
			m_gameover &= ( m_Player[i].inventory.lives < 0 );

		if(!m_gameover) // Check if no player has lives left and must go in game over mode.
			goBacktoMap();
	}
}

// called when a switch is flipped. mx,my is the pixel coords of the switch,
// relative to the upper-left corner of the map.
// TODO: Should be part of an object
void CPlayGameVorticon::processLevelTrigger(int trigger)
{
	if (trigger == LVLTRIG_TANTALUS_RAY)
	{
		mMap->m_Dark = false;
		g_pMusicPlayer->stop();
		gGraphics.Palette.setdark(mMap->m_Dark);
		mpFinale.reset( new CTantalusRay( mMessageBoxes, mMap, mSpriteObjectContainer, mpObjectAI ) );	

        std::shared_ptr<CMessageBoxVort> msg(new CMessageBoxVort("Uh-Oh"));
        mMessageBoxes.push_back( msg );
		
		m_Player[0].dontdraw = true;
		m_gameover = true;
	}
	else if (trigger == LVLTRIG_BRIDGE)
	{	// it's a moving platform switch--don't allow player to hit it again while
		// the plat is still moving as this will glitch

		// The spawning of the plat extension is defined in the CPlayer class
	}
	else if (trigger == LVLTRIG_LIGHT)
	{
		mMap->m_Dark = !mMap->m_Dark;
		gGraphics.Palette.setdark(mMap->m_Dark);
	}
}

