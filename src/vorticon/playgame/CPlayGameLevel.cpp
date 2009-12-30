/*
 * CPlayGameLevel.cpp
 *
 *  Created on: 05.11.2009
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "../ai/se.h"
#include "../../common/objenums.h"
#include "../../sdl/sound/CSound.h"
#include "../finale/CTantalusRay.h"

void CPlayGame::processInLevel()
{
	if(!m_gameover)
	{
		// Perform player Objects...
		for( int i=0 ; i<m_NumPlayers ; i++ )
		{
			// check if someone has lifes
			if(m_Player[i].inventory.lives==0 && m_Player[i].pdie==PDIE_DEAD)
				continue;

			// Process the other stuff like, items, jump, etc.
			m_Player[i].processInLevel(mp_ObjectAI->getPlatMoving());

			// If the player touched did in which we have to show a Message, do it so
			std::string hinttext;
			if( (hinttext=m_Player[i].pollHintMessage()) != "")
				m_MessageBoxes.push_back(new CMessageBox(getstring(hinttext)));

			// Process the falling physics of the player here.
			// We need to know the objects and tiles which could hinder the fall.
			// decide if player should fall
			if (!m_Player[i].inhibitfall) processPlayerfallings(&m_Player[i]);
			else
			{
				m_Player[i].psupportingtile = 145;
				m_Player[i].psupportingobject = 0;
			}

			// Check Collisions and only move player, if it is not blocked
			checkPlayerCollisions(&m_Player[i]);

			// Check if the first player is dead, and if the others also are...
			if(i==0) m_alldead = (m_Player[i].pdie == PDIE_DEAD);
			else m_alldead &= (m_Player[i].pdie == PDIE_DEAD);

			// Now draw the player to the screen
			m_Player[i].SelectFrame();

			// If Player has toggled a switch for platform extend it!
			int trigger = m_Player[i].pollLevelTrigger();
			if( trigger != LVLTRIG_NONE )
			{
				processLevelTrigger(trigger);
			}

			// finished the level
			if(m_Player[i].level_done == LEVEL_COMPLETE)
			{
				mp_level_completed[m_Level] = true;
				goBacktoMap();
				break;
			}
			else if(m_Player[i].level_done == LEVEL_TELEPORTER)
			{	// This happens, when keen used the inlevel teleporter...
				CTeleporter Teleporter( m_TeleporterTable, m_Episode);
				goBacktoMap();
				Teleporter.teleportPlayerFromLevel(*mp_Map, m_Object, m_Player[i], m_checkpoint_x, m_checkpoint_y);
				break;
			}
		}

		// Check if all players are dead. In that case, go back to map
		if(m_alldead)
		{
			m_gameover = true; // proof contrary case
			for( int i=0 ; i<m_NumPlayers ; i++ )
				m_gameover &= ( m_Player[i].inventory.lives < 0 );

			if(!m_gameover) // Check if no player has lifes left and must go in game over mode.
				goBacktoMap();
			m_dark = false;
			g_pGfxEngine->Palette.setdark(m_dark);
		}
	}
}

// called when a switch is flipped. mx,my is the pixel coords of the switch,
// relative to the upper-left corner of the map.
// TODO: Should be part of an object
void CPlayGame::processLevelTrigger(int trigger)
{
	if (trigger == LVLTRIG_TANTALUS_RAY)
	{
		m_dark = false;
		g_pGfxEngine->Palette.setdark(m_dark);
		mp_Finale = new CTantalusRay(*mp_Map, m_Object, *mp_ObjectAI);
		m_gameover = true;
	}
	else if (trigger == LVLTRIG_BRIDGE)
	{	// it's a moving platform switch--don't allow player to hit it again while
		// the plat is still moving as this will glitch
		if (!mp_ObjectAI->getPlatMoving())
		{
			mp_ObjectAI->triggerPlat(true);
		}
		// The spawning of the plat extension is defined in the CPlayer class
	}
	else if (trigger == LVLTRIG_LIGHT)
	{
		m_dark = !m_dark;
		g_pGfxEngine->Palette.setdark(m_dark);
	}
}

