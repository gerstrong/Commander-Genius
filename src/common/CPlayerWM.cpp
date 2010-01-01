/*
 * CPlayerWM.cpp
 *
 *  Created on: 07.10.2009
 *      Author: gerstrong
 */

#include "CPlayer.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CInput.h"
#include "../keen.h"

///
// Process Part
///

////
// Process the stuff of the player when playing on the world map
void CPlayer::processWorldMap()
{
    inhibitwalking = false;
    inhibitfall = false;
	
    StatusBox();
	
   	if(!hideplayer) ProcessInput();
   	setWorldMapdir();
	
   	setWMblockedlrud();
	
    if(!hideplayer && !beingteleported)	Walking();
   	WalkingAnimation();
	
   	InertiaAndFriction_X();
   	InertiaAndFriction_Y();
	
   	selectFrameOnWorldMap();
}

// select proper player direction
void CPlayer::setWorldMapdir()
{
	bool exception=false;
	dpadcount = 0;
	if (playcontrol[PA_X] < 0) { pdir = LEFT; dpadcount++; }
	if (playcontrol[PA_X] > 0) { pdir = RIGHT; dpadcount++; }
	if (playcontrol[PA_Y] < 0) { pdir = UP; dpadcount++; }
	if (playcontrol[PA_Y] > 0) { pdir = DOWN; dpadcount++; }
	
	if ( pshowdir==UP && !playcontrol[PA_Y] < 0) exception = true;
	if ( pshowdir==DOWN && !playcontrol[PA_Y] > 0) exception = true;
	if ( pshowdir==LEFT && !playcontrol[PA_X] < 0) exception = true;
	if ( pshowdir==RIGHT && !playcontrol[PA_X] > 0) exception = true;
	if ((playcontrol[PA_Y] < 0) && (playcontrol[PA_Y] > 0)) pshowdir = DOWN;
	if ((playcontrol[PA_X] < 0) && (playcontrol[PA_X] > 0)) pshowdir = RIGHT;
	
	if (dpadcount==1 || dpadlastcount==0 || exception)
		pshowdir = pdir;

	dpadlastcount = dpadcount;
}

// set blockedl and blockedr...is Keen up against a solid object?
void CPlayer::setWMblockedlrud()
{
	int x1, x2, y1, y2;

	blockedl = blockedr = false;
	blockedu = blockedd = false;
	
	// cheat: holding down TAB will turn off clipping. or if you are in godmode
	if ((m_cheats_enabled && g_pInput->getHoldedKey(KTAB)) || godmode) return;
	
	CSprite &sprite = g_pGfxEngine->getSprite(PMAPRIGHTFRAME);
	x1 = sprite.m_bboxX1;
	x2 = sprite.m_bboxX2;
	y1 = sprite.m_bboxY1;
	y2 = sprite.m_bboxY2;

	// L
	if (isWMSolid(goto_x+x1, goto_y+y1+(1<<STC)) )
	{ blockedl = true; }
	else if (isWMSolid(goto_x+x1, goto_y+y2-(1<<STC)) )
	{ blockedl = true; }

	// R
	if (isWMSolid(goto_x+x2, goto_y+y1+(1<<STC)) )
	{ blockedr = true; }
	else if (isWMSolid(goto_x+x2, goto_y+y2-(1<<STC)) )
	{ blockedr = true; }
	
	// U
	if (isWMSolid(goto_x+x1+(1<<STC), goto_y+y1) )
	{ blockedu = true; }
	else if (isWMSolid(goto_x+x2-(1<<STC), goto_y+y1) )
	{ blockedu = true; }
	
	// D
	if (isWMSolid(goto_x+x1+(1<<STC), goto_y+y2) )
	{ blockedd = true; }
	else if (isWMSolid(goto_x+x2-(1<<STC), goto_y+y2) )
	{ blockedd = true; }
}

// tell me, if the player tries to use an object on the map like entering the level
int CPlayer::getNewObject()
{
	int xb, yb;
	int lvl;
	
    if ( !object_chosen && (playcontrol[PA_JUMP] || playcontrol[PA_POGO]) )
    {   // trying to enter a level (or use a teleporter, etc)
    	object_chosen = true;
		
        // get level/object marker beneath player
        xb = ((x+w/2)>>STC);
        yb = ((y+h/2)>>STC);
        lvl = mp_map->getObjectat(xb>>4, yb>>4);
        if (!lvl)
        {
			yb = (y>>CSF)+8;
			lvl = mp_map->getObjectat(xb>>4, yb>>4);
        }
        return lvl;
    }

    if (!playcontrol[PA_JUMP] && !playcontrol[PA_POGO])
    	object_chosen = false;

    return 0;
}

bool CPlayer::isWMSolid(int xb, int yb)
{
	int level_coordinates;
	
	// Now check if the levels must block the player
	level_coordinates = mp_map->getObjectat(xb>>CSF, yb>>CSF);
	
	if (level_coordinates & 0x8000)
	{
		if( mp_levels_completed[level_coordinates & 0x7fff] ) // check if level is done, but can be replayed
		{	  return false;	}
		
		if( g_pInput->getHoldedKey(KTAB) && g_pInput->getHoldedKey(KSHIFT) )
		{	  return false;	}
		else
		{	  return true;	}
	}
	return false;
}

void CPlayer::selectFrameOnWorldMap()
{
    // select base frame for current direction
    if (pshowdir==RIGHT) playframe = PMAPRIGHTFRAME;
    else if (pshowdir==LEFT) playframe = PMAPLEFTFRAME;
    else if (pshowdir==UP) playframe = PMAPUPFRAME;
    else if (pshowdir==DOWN) playframe = PMAPDOWNFRAME;
	
    // episode 3 map frames start at 31, ep1&2 at 32
    if (m_episode==3) playframe--;
	
    // add in walk frame if walking
    if (pwalking) playframe += pwalkframe;
}

// if nessie is at a mount point near the player, mounts him!
void CPlayer::MountNessieIfAvailable()
{
	// Look for the Nessie object
	std::vector<CObject>::iterator obj = mp_object->begin();
	for(; obj != mp_object->end() ; obj++)
	{
		if(obj->m_type == OBJ_NESSIE)
		{
			Uint16 dist = 1<<CSF;
			Uint16 nessie_x, nessie_y;

			nessie_x = obj->x;
			nessie_y = obj->y;

			// Look if Nessie is nearby
			if( x >= nessie_x-dist+obj->bboxX1 and x <= nessie_x+dist+obj->bboxX2 )
			{
				if( y >= nessie_y-dist+obj->bboxY1 and y <= nessie_y+dist+obj->bboxY2 )
				{
					// Mount the Player
					obj->ai.nessie.mounted[m_player_number] = true;
		            mounted = true;
		            hideplayer = true;
				}
			}
			break;
		}
	}

}

void CPlayer::UnmountNessie()
{
	// Check if a NESSIE_LAND_OBJ is nearby the player. Only then he can unmount
	int dx, dy;
	for(dy=-2 ; dy <= 2 ; dy++)
	{
		for(dx=-2 ; dx <= 2 ; dx++)
		{
			// If NESSIE_LAND_OBJ was found, than put the player there!
			if(mp_map->getObjectat((x>>CSF)+dx, (y>>CSF)+dy) == NESSIE_LAND)
			{
				// Look for the Nessie object
				std::vector<CObject>::iterator obj = mp_object->begin();
				for(; obj != mp_object->end() ; obj++)
				{
					if(obj->m_type == OBJ_NESSIE)
					{
						// Check if the there are no blocked tiles there!
						stTile Tile = g_pGfxEngine->Tilemap->mp_tiles[mp_map->at((x>>CSF)+dx, (y>>CSF)+dy)];
						if( !Tile.bdown and !Tile.bup and
							!Tile.bleft and !Tile.bright )
						{
							// unmount nessie
							obj->ai.nessie.mounted[m_player_number] = false;
							mounted = false;
							hideplayer = false;
							goto_x = x = x+(dx<<CSF);
							goto_y = y = y+(dy<<CSF);
							break;
						}
					}
				}
			}
		}
	}
}

