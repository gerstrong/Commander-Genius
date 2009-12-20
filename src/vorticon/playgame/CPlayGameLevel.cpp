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
			m_Player[i].processInLevel();

			// If the player touched did in which we have to show a Message, do it so
			std::string hinttext;
			if( (hinttext=m_Player[i].pollHintMessage()) != "")
				mp_MessageBox = new CMessageBox(getstring(hinttext));

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
			if(m_Player[i].mustExtendPlatform())
			{
				ExtendingPlatformSwitch(m_Player[i].x, m_Player[i].y);
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
		}
	}
}

// called when a switch is flipped. mx,my is the pixel coords of the switch,
// relative to the upper-left corner of the map.
// TODO: Should be part of an object
void CPlayGame::ExtendingPlatformSwitch(int x, int y)
{
	uint ppos;
	int platx, platy;
	signed char pxoff, pyoff;
	int mapx, mapy;

	// convert pixel coords to tile coords
	mapx = (x >> CSF);
	mapy = (y >> CSF);

	// figure out where the platform is supposed to extend at
	// (this is coded in the object layer...
	// high byte is the Y offset and the low byte is the X offset,
	// and it's relative to the position of the switch.)
	ppos = mp_Map->getObjectat(x, y);

	if (!ppos || !mp_ObjectAI->getPlatExtending())
	{
		// flip switch
		g_pSound->playStereofromCoord(SOUND_SWITCH_TOGGLE, PLAY_NOW, mapx<<STC);
		if ( mp_Map->at(x, y) == TILE_SWITCH_DOWN )
			mp_Map->setTile(mapx, mapy, TILE_SWITCH_UP,true);
		else
			mp_Map->setTile(mapx, mapy, TILE_SWITCH_DOWN,true);
	}

	// if zero it means he hit the switch on a tantalus ray!
	if (!ppos)
	{
		// TODO: Add Code for the tantalus ray trigger
		/*p_levelcontrol->success = 0;
		p_levelcontrol->command = LVLC_TANTALUS_RAY;*/
		return;
	}
	else
	{
		// it's a moving platform switch--don't allow player to hit it again while
		// the plat is still moving as this will glitch
		if (mp_ObjectAI->getPlatExtending()) return;
		mp_ObjectAI->extendPlat(true);
	}

	pxoff = (ppos & 0x00ff);
	pyoff = (ppos & 0xff00) >> 8;
	platx = mapx + pxoff;
	platy = mapy + pyoff;

	// spawn a "sector effector" to extend/retract the platform
	CObject platobject;
	platobject.spawn(mapx<<CSF,mapy<<CSF,OBJ_SECTOREFFECTOR, m_Episode);
	platobject.ai.se.type = SE_EXTEND_PLATFORM;
	platobject.ai.se.platx = platx;
	platobject.ai.se.platy = platy;
	m_Object.push_back(platobject);
}

