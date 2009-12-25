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
	
	CSprite &sprite = *g_pGfxEngine->Sprite[PMAPRIGHTFRAME];
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
	{ blockedu = 1; }
	else if (isWMSolid(goto_x+x2-(1<<STC), goto_y+y1) )
	{ blockedu = 1; }
	
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
/*void CPlayer::MountNessieIfAvailable(int cp)
{
char AtSameMountPoint;

   if (!player[cp].mounted)
   {
      // is nessie paused?
      if (objects[NessieObjectHandle].ai.nessie.pausetimer)
      {
         // is she at the same mount point the player is?
         if (player[cp].y>>CSF>>4 < map.ysize>>2 && objects[NessieObjectHandle].y>>CSF>>4 < map.ysize>>2) AtSameMountPoint = 1;    // both at mortimer's castle
         else if (player[cp].y>>CSF>>4 > map.ysize>>1 && objects[NessieObjectHandle].y>>CSF>>4 > map.ysize>>1) AtSameMountPoint = 1;    // both at secret island
         else AtSameMountPoint = 0;

         if (AtSameMountPoint)
         {
            objects[NessieObjectHandle].ai.nessie.mounted[cp] = 1;
            player[cp].mounted = 1;
            player[cp].hideplayer = 1;
         }
      }
   }
}*/

void CPlayer::AllowMountUnmountNessie()
{
	/*int objmarker;
	if (!mounted)
	{  // not mounted. find out if he's trying to mount
		// if the upper quarter of the map (mortimer's castle mount point)
		// he's trying to mount if he's on a NESSIE_MOUNTPOINT object marker
		// and he's going right and is blockedr and/or is going down and is
		// blockedd. in the bottom quarter (secret island mount point)
		// it's up and blockedu.

		// make sure he's on a mount point
		objmarker = mp_map->getObjectat((player[cp].x+(4<<STC))>>CSF, ((player[0].y)+(9<<STC))>>CSF);
		if (objmarker != NESSIE_MOUNTPOINT) return;

		// is he trying to mount?
		if ( (y>>CSF) < mp_map->m_width )
		{  // at mortimer's castle mount point
			if ( (playcontrol[PA_X] > 0 && blockedr) ||
					(playcontrol[PA_Y] > 0 && blockedd))
			{
				// YES! if nessie is at that mount point, mount her!!
				MountNessieIfAvailable(cp);
			}
		}
		else
		{  // at secret island mount point
			if (playcontrol[PA_Y] < 0 && blockedu)
			{
				MountNessieIfAvailable(cp);
			}
		}
	}
	else
	{  // mounted. find out if he's trying to unmount.
		if (objects[NessieObjectHandle].ai.nessie.pausetimer)
		{  // nessie is paused
			if (objects[NessieObjectHandle].y>>CSF>>4 < map.ysize>>2)
			{  // nessie is at mortimer's castle mount point
				if (player[cp].playcontrol[PA_Y] < 0)
				{
					// unmount nessie
					objects[NessieObjectHandle].ai.nessie.mounted[cp] = 0;
					player[cp].mounted = 0;
					player[cp].hideplayer = 0;
				}
			}
			else if (objects[NessieObjectHandle].y>>CSF>>4 > map.ysize>>1)
			{  // nessie is at secret island mount point
				if (player[cp].playcontrol[PA_Y] > 0)
				{
					// unmount nessie
					objects[NessieObjectHandle].ai.nessie.mounted[cp] = 0;
					player[cp].mounted = 0;
					player[cp].hideplayer = 0;
					player[cp].y += (18<<CSF);
					player[cp].x += (8<<CSF);
				}
			}
		}
	}*/
}

