/*
 * CPlayGamePhysics.cpp
 *
 *  Created on: 18.10.2009
 *      Author: gerstrong
 *
 *  Performs physics between and in common objects
 */

#include "CPlayGame.h"
#include "sounds.h"
#include "../keen.h"
#include "../sdl/CInput.h"
#include "../sdl/sound/CSound.h"
#include "../graphics/CGfxEngine.h"
#include "../common/Playerdefines.h"

void CPlayGame::processPlayerfallings(CPlayer *p_player)
{
	unsigned int temp;
	int objsupport=false;
	short tilsupport;
	Uint8 height=p_player->h>>(CSF-4);
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	p_player->pfalling = 0;         // assume not falling if not jumped to the maximum height

	// do not fall if we're jumping
	if (p_player->pjumping)
	{
		p_player->psemisliding = 0;
	  return;
	}

	// ** determine if player should fall (nothing solid is beneath him) **

	p_player->psupportingtile = BG_GRAY;
	p_player->psupportingobject = 0;
	// test if tile under player is solid; if so set psupportingtile
	//objsupport = checkobjsolid(x+(4<<CSF), y+( height<<(CSF-4)),cp);

	tilsupport = TileProperty[mp_Map->at((p_player->x>>CSF)+5, (p_player->y>>CSF)+height)].bup;
	if(TileProperty[mp_Map->at((p_player->x>>CSF)+4, (p_player->y>>CSF)+height)].behaviour >= 2&&
			TileProperty[mp_Map->at((p_player->x>>CSF)+4, (p_player->y>>CSF)+height)].behaviour <= 5)
		tilsupport = 1; // This workaround prevents the player from falling through doors.

	if (!tilsupport && !objsupport)
	{ // lower-left isn't solid
	  //objsupport = checkobjsolid(x+(12<<CSF), y+(height<<CSF),cp);
	  tilsupport = TileProperty[mp_Map->at((p_player->x>>CSF)+10, (p_player->y>>CSF)+height)].bup;
	  if (!tilsupport && !objsupport)
	  {  // lower-right isn't solid
		  p_player->pfalling = 1;        // so fall.
		  p_player->pjustfell = 1;
	  }
	  else
	  {  // lower-left isn't solid but lower-right is
		if (objsupport)
		{
			p_player->psupportingtile = PSUPPORTEDBYOBJECT;
			p_player->psupportingobject = objsupport;
		}
	  }
	}
	else
	{   // lower-left is solid
		if (objsupport)
		{
			p_player->psupportingtile = PSUPPORTEDBYOBJECT;
			p_player->psupportingobject = objsupport;
		}
	}

	// if not on a tile boundary, always fall, prevents being able
	// to land in the middle of a tile.
	if (!p_player->pfalling && p_player->psupportingtile != PSUPPORTEDBYOBJECT)
	{
	   temp = (p_player->y>>CSF)+height;    // bottom of player
	   if ((temp>>4)<<4 != temp)   // true if it's not a multiple of 16
	   {
		   p_player->pfalling = 1;   // not on a tile boundary. fall.
		   p_player->pjustfell = 1;
		   p_player->psupportingtile = BG_GRAY;
		   p_player->psupportingobject = 0;
	   }
	}
	// if supported by an object make sure we're at the top of
	// the object else fall
	if (!p_player->pfalling && p_player->psupportingtile == PSUPPORTEDBYOBJECT)
	{
	   if ((p_player->y>>CSF)+height > (m_Object[p_player->psupportingobject].y>>CSF)+4)
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
		p_player->y = m_Object[p_player->psupportingobject].y - (height<<CSF);
	}
	p_player->lastsupportingobject = p_player->psupportingobject;

	// ** if the player should be falling, well what are we waiting for?
	//    make him fall! **
	if (mp_option[OPT_CHEATS].value && g_pInput->getHoldedKey(KPLUS)) { p_player->pfalling = true; p_player->pjustfell = 1; }

	if (p_player->pfalling)
	{  // nothing solid under player, let's make him fall
		p_player->psemisliding = 0;

	   // just now started falling? (wasn't falling last time)
	   if (p_player->plastfalling == 0)
	   {
		 // set initial fall speed and make the AAAAAUUUUHHHHH noise
		 p_player->pfallspeed = 1;
		 if (!p_player->pjustjumped)
			 g_pSound->playStereofromCoord(SOUND_KEEN_FALL, PLAY_NOW, p_player->mp_object->scrx);
	   }

	   // gradually increase the fall speed up to maximum rate
	   if (p_player->pfallspeed>PFALL_MAXSPEED)
		   p_player->pfallspeed = PFALL_MAXSPEED;
	   else if (p_player->pfallspeed<PFALL_MAXSPEED)
		   p_player->pfallspeed += PFALL_INCREASE;

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
			  g_pSound->playStereofromCoord(SOUND_KEEN_LAND, PLAY_NOW, p_player->mp_object->scrx);
		  // fix "sliding" effect when you fall, go one way, then
		  // before you land turn around and as you hit the ground
		  // you're starting to move the other direction
		  // (set inertia to 0 if it's contrary to player's current dir)
	   }
	}   // close "not falling"

	// save fall state so we can detect the high/low-going edges
	p_player->plastfalling = p_player->pfalling;

	// ensure no sliding if we fall or jump off of ice
	if (p_player->pfalling || p_player->pjumping) p_player->psliding=0;
}

