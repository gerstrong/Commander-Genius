/*
 * CPlayGameLevel.cpp
 *
 *  Created on: 05.11.2009
 *      Author: gerstrong
 */

#include "CPlayGame.h"
#include "../../sdl/sound/CSound.h"

void CPlayGame::processInLevel()
{
	if(!m_gameover)
	{
		// Perform player Objects...
		for( int i=0 ; i<m_NumPlayers ; i++ )
		{
			// check if someone has lifes
			if(mp_Player[i].inventory.lives==0 && mp_Player[i].pdie==PDIE_DEAD)
				continue;

			// Process the other stuff like, items, jump, etc.
			mp_Player[i].processInLevel();

			// If the player touched did in which we have to show a Message, do it so
			std::string hinttext;
			if( (hinttext=mp_Player[i].pollHintMessage()) != "")
				mp_MessageBox = new CMessageBox(getstring(hinttext));

			// Process the falling physics of the player here.
			// We need to know the objects and tiles which could hinder the fall.
			// decide if player should fall
			if (!mp_Player[i].inhibitfall) processPlayerfallings(&mp_Player[i]);
			else
			{
				mp_Player[i].psupportingtile = 145;
				mp_Player[i].psupportingobject = 0;
			}

			// Check Collisions and only move player, if it is not blocked
			checkPlayerCollisions(&mp_Player[i]);

			// Check if the first player is dead, and if the others also are...
			if(i==0) m_alldead = (mp_Player[i].pdie == PDIE_DEAD);
			else m_alldead &= (mp_Player[i].pdie == PDIE_DEAD);

			// Now draw the player to the screen
			mp_Player[i].SelectFrame();

			// finished the level
			if(mp_Player[i].level_done == LEVEL_COMPLETE)
			{
				mp_level_completed[m_Level] = true;
				goBacktoMap();
				break;
			}
		}

		// gets to bonus level

		// Check if all players are dead. In that case, go back to map
		if(m_alldead)
		{
			m_gameover = true; // proof contrary case
			for( int i=0 ; i<m_NumPlayers ; i++ )
				m_gameover &= ( mp_Player[i].inventory.lives <= 0 );

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
	/*uint ppos;
	 int platx, platy;
	 signed char pxoff, pyoff;
	 int mapx, mapy;
	 int o;

	 // convert pixel coords to tile coords
	 mapx = (x >> TILE_S);
	 mapy = (y >> TILE_S);

	 // figure out where the platform is supposed to extend at
	 // (this is coded in the object layer...
	 // high byte is the Y offset and the low byte is the X offset,
	 // and it's relative to the position of the switch.)
	 ppos = getlevelat(x, y);

	 if (!ppos || !p_levelcontrol->PlatExtending)
	 {
	 // flip switch
	 g_pSound->playStereofromCoord(SOUND_SWITCH_TOGGLE, PLAY_NOW, mapx);
	 if (getmaptileat(x, y)==TILE_SWITCH_DOWN)
	 map_chgtile(mapx, mapy, TILE_SWITCH_UP);
	 else
	 map_chgtile(mapx, mapy, TILE_SWITCH_DOWN);
	 }

	 // if zero it means he hit the switch on a tantalus ray!
	 if (!ppos)
	 {
	 p_levelcontrol->success = 0;
	 p_levelcontrol->command = LVLC_TANTALUS_RAY;
	 return;
	 }
	 else
	 {
	 // it's a moving platform switch--don't allow player to hit it again while
	 // the plat is still moving as this will glitch
	 if (p_levelcontrol->PlatExtending) return;
	 p_levelcontrol->PlatExtending = 1;
	 }

	 pxoff = (ppos & 0x00ff);
	 pyoff = (ppos & 0xff00) >> 8;
	 platx = mapx + pxoff;
	 platy = mapy + pyoff;

	 // spawn a "sector effector" to extend/retract the platform
	 o = spawn_object(mapx<<TILE_S<<CSF,mapy<<TILE_S<<CSF,OBJ_SECTOREFFECTOR);
	 objects[o].ai.se.type = SE_EXTEND_PLATFORM;
	 objects[o].ai.se.platx = platx;
	 objects[o].ai.se.platy = platy;*/
}

