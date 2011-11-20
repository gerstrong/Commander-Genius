/*
 * CPlayerWM.cpp
 *
 *  Created on: 07.10.2009
 *      Author: gerstrong
 */

#include "CPlayer.h"
#include "engine/vorticon/ai/CMessie.h"
#include "graphics/CGfxEngine.h"
#include "sdl/input/CInput.h"

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
	
   	if(!hideplayer && !beingteleported) ProcessInput();
   	setWorldMapdir();
	
    if(!hideplayer && !beingteleported)	Walking();

    if(!beingteleported)
    {
    	WalkingAnimation();

    	InertiaAndFriction_X();
    	InertiaAndFriction_Y();

    	performCollisions();

    	selectFrameOnWorldMap();
    }

    processEvents();
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
	
	if (playcontrol[PA_Y] < 0) pshowdir = UP;
	if (playcontrol[PA_Y] > 0) pshowdir = DOWN;

	if (dpadcount==1 || dpadlastcount==0 || exception)
		pshowdir = pdir;

	dpadlastcount = dpadcount;
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
        xb = getXMidPos()>>STC;
        yb = getYMidPos()>>STC;
        lvl = mp_Map->getObjectat(xb>>4, yb>>4);
        if (!lvl)
        {
			yb = (getYPosition()>>CSF)+8;
			lvl = mp_Map->getObjectat(xb>>4, yb>>4);
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
	level_coordinates = mp_Map->getObjectat(xb>>CSF, yb>>CSF);

	if (level_coordinates & 0x8000)
	{
		if( mp_levels_completed[level_coordinates & 0x7fff] ) // check if level is done
			  return false;

		if( (g_pInput->getHoldedKey(KTAB) && g_pInput->getHoldedKey(KSHIFT)) || godmode )
			  return false;
		else
			  return true;
	}
	return false;
}

void CPlayer::verifySolidLevels()
{
	if(godmode) return;

	int x1 = getXLeftPos();
	int x2 = getXRightPos();
	int y1 = getYUpPos();
	int y2 = getYDownPos();
	int ymid = getYMidPos();
	int xmid = getXMidPos();

	if(isWMSolid(xmid,y1)) {
		moveDown(1);
		blockedu = true;
	}
	if(isWMSolid(xmid,y2)) {
		moveUp(1);
		blockedd = true;
	}
	if(isWMSolid(x1,ymid)) {
		moveRight(1);
		blockedl = true;
	}
	if(isWMSolid(x2,ymid)) {
		moveLeft(1);
		blockedr = true;
	}
}

void CPlayer::selectFrameOnWorldMap()
{
    // select base frame for current direction
    if (pshowdir==RIGHT) sprite = playerbaseframe + PMAPRIGHTFRAME;
    else if (pshowdir==LEFT) sprite = playerbaseframe + PMAPLEFTFRAME;
    else if (pshowdir==UP) sprite = playerbaseframe + PMAPUPFRAME;
    else if (pshowdir==DOWN) sprite = playerbaseframe + PMAPDOWNFRAME;

    // add in walk frame if walking
    if (pwalking) sprite += pwalkframe;
}

// if nessie is at a mount point near the player, mounts him!
void CPlayer::MountNessieIfAvailable()
{
	// Look for the Nessie object
	std::vector<CObject*>::iterator obj = mp_object->begin();
	for(; obj != mp_object->end() ; obj++)
	{
		if((*obj)->m_type == OBJ_MESSIE)
		{
			CMessie &Messie = dynamic_cast<CMessie&>(**obj);
			Uint16 dist = 1<<CSF;
			Uint16 nessie_x, nessie_y;

			nessie_x = (*obj)->getXPosition();
			nessie_y = (*obj)->getYPosition();

			// Look if Nessie is nearby
			Uint32 x = getXPosition();
			Uint32 y = getYPosition();
			if( x >= nessie_x-dist+(*obj)->m_BBox.x1 and x <= nessie_x+dist+(*obj)->m_BBox.x2 )
			{
				if( y >= nessie_y-dist+(*obj)->m_BBox.y1 and y <= nessie_y+dist+(*obj)->m_BBox.y2 )
				{
					// Mount the Player
					Messie.mounted[m_index] = true;
		            mounted = true;
		            solid = false;
		            beingteleported = true;
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
	int x = getXPosition()>>CSF;
	int y = getYPosition()>>CSF;
	for(dy=-1 ; dy <= 1 ; dy++)
	{
		for(dx=-1 ; dx <= 1 ; dx++)
		{
			// If NESSIE_LAND_OBJ was found, than put the player there!
			if(mp_Map->getObjectat(x+dx, y+dy) == NESSIE_LAND)
			{
				// Look for the Nessie object
				std::vector<CObject*>::iterator obj = mp_object->begin();
				for(; obj != mp_object->end() ; obj++)
				{
					if((*obj)->m_type == OBJ_MESSIE)
					{
						CMessie &Messie = dynamic_cast<CMessie&>(**obj);
						// Check if the there are no blocked tiles there!
						std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
						CTileProperties &Tile = TileProperty[mp_Map->at(x+dx, y+dy)];
						if( !Tile.bdown and !Tile.bup and
							!Tile.bleft and !Tile.bright )
						{
							// unmount Messie
							Messie.mounted[m_index] = false;
							mounted = false;
							solid = godmode ? false : true;
							beingteleported = false;
							moveXDir(dx<<CSF);
							moveYDir(dy<<CSF);
							break;
						}
					}
				}
			}
		}
	}
}

