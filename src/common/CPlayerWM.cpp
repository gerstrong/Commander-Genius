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

        // check first if the player is not blocked by a level
        verifySolidLevels();
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
    //bool exception=false;
    int playX = playcontrol[PA_X];
    int playY = playcontrol[PA_Y];

    // How many buttons are pressed?
	dpadcount = 0;

    if (playX < 0) { pDir.x = LEFT; pDir.y = 0; dpadcount++; }
    if (playX > 0) { pDir.x = RIGHT; pDir.y = 0; dpadcount++; }
    if (playY < 0) { pDir.y = UP; pDir.x = 0; dpadcount++; }
    if (playY > 0) { pDir.y = DOWN; pDir.x = 0; dpadcount++; }
	
	
    if (playX != 0)
        pShowDir = pDir;


    if (playcontrol[PA_Y] < 0) pShowDir.y = UP;
    if (playcontrol[PA_Y] > 0) pShowDir.y = DOWN;


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

const int WMSOLIDBUMPSPEED = 1;

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
        moveDown(WMSOLIDBUMPSPEED);
		blockedu = true;
	}
	if(isWMSolid(xmid,y2)) {
        moveUp(WMSOLIDBUMPSPEED);
		blockedd = true;
	}
	if(isWMSolid(x1,ymid)) {
        moveRight(WMSOLIDBUMPSPEED);
		blockedl = true;
	}
	if(isWMSolid(x2,ymid)) {
        moveLeft(WMSOLIDBUMPSPEED);
		blockedr = true;
	}
}

void CPlayer::selectFrameOnWorldMap()
{
    // select base frame for current direction
    if (pShowDir.x==RIGHT) sprite = playerbaseframe + PMAPRIGHTFRAME;
    else if (pShowDir.x==LEFT) sprite = playerbaseframe + PMAPLEFTFRAME;

    if (pShowDir.y==UP) sprite = playerbaseframe + PMAPUPFRAME;
    else if (pShowDir.y==DOWN) sprite = playerbaseframe + PMAPDOWNFRAME;

    // add in walk frame if walking
    if (pwalking) sprite += pwalkframe;
}

