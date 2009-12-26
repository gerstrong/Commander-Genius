/*
 * CPlayGamePhysics.cpp
 *
 *  Created on: 18.10.2009
 *      Author: gerstrong
 *
 *  Performs physics between and in common objects
 */

#include "CPlayGame.h"
#include "../sounds.h"
#include "../../keen.h"
#include "../../sdl/CInput.h"
#include "../../sdl/sound/CSound.h"
#include "../../graphics/CGfxEngine.h"
#include "../../common/Playerdefines.h"
#include "../../common/CPhysicsSettings.h"

void CPlayGame::processPlayerfallings(CPlayer *p_player)
{
	unsigned int temp;
	int objsupport=0;
	short tilsupport;
	char behaviour;
	int sprite = m_Object[p_player->m_player_number].sprite;
	Uint16 x1 = g_pGfxEngine->Sprite[sprite]->m_bboxX1-1;
	Uint16 x2 = g_pGfxEngine->Sprite[sprite]->m_bboxX2+1;
	Uint16 y1 = g_pGfxEngine->Sprite[sprite]->m_bboxY1-1;
	Uint16 y2 = g_pGfxEngine->Sprite[sprite]->m_bboxY2+1;
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

		if (p_player->pfalling)
		{
			if (p_player->plastfalling == 0)
			{
				if (!p_player->pjustjumped)
					g_pSound->playStereofromCoord(SOUND_KEEN_FALL, PLAY_NOW, p_player->mp_object->at(p_player->m_player_number).scrx);
			}
		}

		// save fall state so we can detect the high/low-going edges
		p_player->plastfalling = p_player->pfalling;

		if(p_player->pdie) return;

		// do not fall if we're jumping
		if (p_player->pjumping)
		{
			p_player->psemisliding = false;
			return;
		}

		// ** determine if player should fall (nothing solid is beneath him) **

		p_player->psupportingtile = BG_GRAY;
		p_player->psupportingobject = 0;
		// test if tile under player is solid; if so set psupportingtile
		objsupport = checkObjSolid(p_player->x+x1+1, p_player->y+y2+(1<<STC), p_player->m_player_number);

		behaviour = TileProperty[mp_Map->at((p_player->x+x1)>>CSF, (p_player->y+y2)>>CSF)].behaviour;
		tilsupport = TileProperty[mp_Map->at((p_player->x+x1)>>CSF, (p_player->y+y2)>>CSF)].bup;
		if( behaviour>=2 && behaviour<=5 )
			tilsupport = true; // This workaround prevents the player from falling through doors.

		if (!tilsupport && !objsupport)
		{ // lower-left isn't solid, check right side
			objsupport = checkObjSolid(p_player->x+x2-1, p_player->y+y2+(1<<STC), p_player->m_player_number);
			tilsupport = TileProperty[mp_Map->at((p_player->x+x2)>>CSF, (p_player->y+y2)>>CSF)].bup;

			if (!tilsupport && !objsupport)
			{  // lower-right isn't solid
				p_player->pfalling = true;        // so fall.
				p_player->pjustfell = true;
			}
			else
			{  // lower-left isn't solid but lower-right is
				if (objsupport)
				{
					while( checkObjSolid(p_player->x+x1+1, p_player->y+y2+(1<<STC)-1, p_player->m_player_number) )
					{
						p_player->y--;
						p_player->goto_y = p_player->y;
					}

					p_player->psupportingtile = PSUPPORTEDBYOBJECT;
					p_player->psupportingobject = objsupport;
				}
			}
		}
		else
		{   // lower-left is solid
			if (objsupport)
			{
				while( checkObjSolid(p_player->x+x1+1, p_player->y+y2+(1<<STC)-1, p_player->m_player_number) )
				{
					p_player->y--;
					p_player->goto_y = p_player->y;
				}

				p_player->psupportingtile = PSUPPORTEDBYOBJECT;
				p_player->psupportingobject = objsupport;
			}
		}

		// if not on a tile boundary, always fall, prevents being able
		// to land in the middle of a tile.
		if (!p_player->pfalling && p_player->psupportingtile != PSUPPORTEDBYOBJECT)
		{
			temp = (p_player->y+y2)>>CSF;    // bottom of player
			if ((temp>>4)<<4 != temp)   // true if it's not a multiple of 16
			{
				p_player->pfalling = true;   // not on a tile boundary. fall.
				p_player->pjustfell = true;
				p_player->psupportingtile = BG_GRAY;
				p_player->psupportingobject = 0;
			}
		}
		// if supported by an object make sure we're at the top of
		// the object else fall
		if (!p_player->pfalling && p_player->psupportingtile == PSUPPORTEDBYOBJECT)
		{
			if ((p_player->y+y2) > (m_Object[p_player->psupportingobject].y+y1) )
			{
				if (!tilsupport)
				{
					p_player->pfalling = true;
					p_player->pjustfell = 1;
					p_player->psupportingtile = BG_GRAY;
					p_player->psupportingobject = 0;
				}
			}
		}

		// the first time we land on an object, line us up to be exactly on
		// top of the object
		if (p_player->psupportingobject && !p_player->lastsupportingobject)
		{
			p_player->goto_y = m_Object[p_player->psupportingobject].y - y2;
		}
		p_player->lastsupportingobject = p_player->psupportingobject;

		// ** if the player should be falling, well what are we waiting for?
		//    make him fall! **
		if (mp_option[OPT_CHEATS].value && g_pInput->getHoldedKey(KPLUS))
		{
			p_player->pfalling = true;
			p_player->pjustfell = true;
		}

		if (p_player->pfalling)
		{  // nothing solid under player, let's make him fall
			p_player->psemisliding = 0;

			// gradually increase the fall speed up to maximum rate
			if (p_player->pfallspeed>m_PhysicsSettings.player.max_fallspeed)
				p_player->pfallspeed = m_PhysicsSettings.player.max_fallspeed;
			else if (p_player->pfallspeed<m_PhysicsSettings.player.max_fallspeed)
				p_player->pfallspeed += m_PhysicsSettings.player.fallspeed_decrease;

			// add current fall speed to player Y or make him fly in godmode with pogo
			if( !p_player->godmode || !p_player->ppogostick || !g_pInput->getHoldedCommand(IC_JUMP) )
				p_player->goto_y += p_player->pfallspeed;
		}
		else
		{  // not falling
			if (p_player->plastfalling)
			{  // just now stopped falling
				if (p_player->pdie != PDIE_FELLOFFMAP)
					g_pSound->stopSound(SOUND_KEEN_FALL);  // terminate fall noise
				// thud noise
				if (!p_player->ppogostick)
					g_pSound->playStereofromCoord(SOUND_KEEN_LAND, PLAY_NOW, p_player->mp_object->at(p_player->m_player_number).scrx);
				// fix "sliding" effect when you fall, go one way, then
				// before you land turn around and as you hit the ground
				// you're starting to move the other direction
				// (set inertia to 0 if it's contrary to player's current dir)
			}
		}   // close "not falling"

		// ensure no sliding if we fall or jump off of ice
		if (p_player->pfalling || p_player->pjumping) p_player->psliding=0;
}

