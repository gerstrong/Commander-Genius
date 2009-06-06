/*
 * CPlayer.cpp
 *
 *  Created on: 31.03.2009
 *      Author: gerstrong
 */

#include <SDL.h>
#include "CPlayer.h"
#include "../sdl/CInput.h"

#include "../keen.h"
#include "../funcdefs.h"

extern int **TileProperty;

////////////////////////
// section of defines //
////////////////////////

#define CSF    5

// the walking defines
#define STANDING_LEFT		0
#define STANDING_RIGHT 		4

#define WALK_SPEED		16

// rate at which player walking animation is shown
#define WALKANIMRATE       40+WALK_SPEED

// friction

// the various jump states
enum jumpstates{
NOJUMP, PREPAREJUMP, JUMPUP, JUMPED, PREPAREPOGO,POGOING};

// Now the frictions we have in the game
enum frictiontable
{
	FRIC_GROUND = 10,
	FRIC_SLIPPERY = 10,
	FRIC_ICE = 10,
	FRIC_AIR = 10,
};

enum direction{	LEFT, RIGHT};

#define MAX_FRICTIONTIME	1000

#define PFIREFRAME          20     // raygun frame index

// pogo frames
#define PFRAME_POGO         24
#define PFRAME_POGOBOUNCE   25

// frame and animation speed for frozen keen (ep1) and stunned keen (ep2&3)
#define PFRAME_FROZEN         28
#define PFROZENANIMTIME       100
// how long keen should stay frozen when hit by an ice chunk
#define PFROZEN_TIME          1000
#define PFROZEN_THAW          100

// when falling keen's Y inertia increases at INCREASERATE up to MAXSPEED
#define PFALL_INCREASERATE       3
#define PFALL_MAXSPEED           19

// friction when player is pushed by yorp's, ball's, etc.
#define PLAYPUSH_DECREASERATE   1

#define PDIEFRAME             	22

#define PLAYERHEIGHT			24
#define PLAYERWIDTH				12

/////////////////////////////////////////
// Section of functions implementation //
/////////////////////////////////////////


CPlayer::CPlayer() {
	object = 1; // it is only 1 for the first player

	episode=0; // while no episode is selected. Indeed the program must select one manually

	mp_scrx = NULL;
	mp_scry = NULL;

	active = false;
	isGameover = false;
	isFalling = false;
	isDying = false;
	isWalking = false;
	isPushed = false;
	cannotWalk = false;
	cannotfall = false;
	usePogostick = false;
	isStanding = true;

    bisBlockedUp = false;
    bisBlockedDown = false;
    bisBlockedLeft = false;
    bisBlockedRight = false;

	walktimer = 0;
	frictiontimer = 0;

	Jumpmode = 0;
}

CPlayer::~CPlayer() {
}

unsigned long CPlayer::getCoordX(void)
{
	return m_x;
}

unsigned long CPlayer::getCoordY(void)
{
	return m_y;
}

void CPlayer::setpScrCoord(int *px, int *py)
{
	mp_scrx = px;
	mp_scry = py;
}

void CPlayer::setCoord(unsigned long in_x, unsigned long in_y)
{
	m_x = in_x;
	m_y = in_y;
}

unsigned short CPlayer::getObject(void)
{
	return object;
}

void CPlayer::useObject(unsigned short value)
{
	object = value;
}

bool CPlayer::isPlaying(void)
{
	return active;
}

void CPlayer::enablePlayer(bool value)
{
	active = value;
}

void CPlayer::handlePlayer(void)
{
/*	char doFall;

	    if (player[cp].pdie)
	    {
	       gamepdo_dieanim(cp, pCKP);
	       if (!pCKP->Control.levelcontrol.gameovermode)
	       {
	         gamepdo_StatusBox(cp, pCKP);
	       }
	    }
	    else
	    {
	       if (!pCKP->Control.levelcontrol.gameovermode)
	       {
	    	  player[cp].inhibitwalking = 0;
	    	  player[cp].inhibitfall = 0;

	          gamepdo_StatusBox(cp, pCKP);

	       	  gamepdo_ProcessInput(cp, pCKP);

	          gamepdo_setdir(cp, pCKP);

	          gamepdo_setblockedlru(cp, pCKP);
	          gamepdo_getgoodies(cp, pCKP);

	          if (pCKP->Control.levelcontrol.episode==3) gamepdo_ankh(cp);

	          gamepdo_raygun(cp, pCKP);

	          gamepdo_keencicle(cp, pCKP);


	          if(!player[cp].pjumping && !player[cp].pfalling)
	          {
	        	  gamepdo_walking(cp, pCKP);
	        	  gamepdo_walkinganim(cp, pCKP);
	          }


	          if (fade.mode==NO_FADE || fade.dir==FADE_IN || demomode)
	          {
	            gamepdo_playpushed(cp, pCKP);
	            gamepdo_InertiaAndFriction_X(cp, pCKP);
	          }

	       	  gamepdo_JumpAndPogo(cp, pCKP);

	          // decide if player should fall
	          doFall = 1;
	          if (player[cp].inhibitfall) doFall = 0;
	          //else if (pCKP->Option[OPT_CHEATS].value) doFall = 0;

	          if (doFall)
	          {
	             gamepdo_falling(cp, pCKP);
	          }
	          else
	          {
	        	  if(player[cp].pjumping == PJUMPED)
	        		  player[cp].pfalling = 0;
	             player[cp].psupportingtile = 145;
	             player[cp].psupportingobject = 0;
	          }

	       }
	       else
	       { // we're in game-over mode

	       }
	    }
*/
	applyFrame();
}

void CPlayer::performDieAnimation(void)
{

}

void CPlayer::processInput(void)
{

}

void CPlayer::processDirection(void)
{

}

void CPlayer::checkCollisions(void)
{
	unsigned int tx,ty;
	short i;

    tx = (m_x>>CSF)+2;
    ty = (m_y>>CSF);

    bisBlockedDown = false;
    bisBlockedUp = false;
    bisBlockedLeft = false;
    bisBlockedRight = false;

    for( i=1 ; i < PLAYERWIDTH ; i++ )
    {
  	  if(TileProperty[getmaptileat((tx+i),ty)][BDOWN] || checkobjsolid((tx+i)<<CSF,(ty)<<CSF,0))
  	  {
		  bisBlockedUp = true;
	  }
  	  if(TileProperty[getmaptileat((tx+i),ty+PLAYERHEIGHT)][BUP] || checkobjsolid((tx+i)<<CSF,(ty+PLAYERHEIGHT)<<CSF,0))
  	  {
  		  // Standing on an solid object
		  bisBlockedDown = true;
	  }
    }

    for( i=1 ; i < PLAYERHEIGHT ; i++ )
    {
  	  if(TileProperty[getmaptileat(tx+PLAYERWIDTH,ty+i)][BRIGHT] || checkobjsolid((tx+PLAYERWIDTH)<<CSF,(ty+i)<<CSF,0))
  	  {

		  bisBlockedRight = true;
	  }
  	  if(TileProperty[getmaptileat(tx,ty+i)][BLEFT] || checkobjsolid((tx)<<CSF,(ty+i)<<CSF,0))
  	  {
		  bisBlockedLeft = true;
	  }
    }
}

void CPlayer::processGoodies(void)
{

}

void CPlayer::processInvincibility(void)
{

}

void CPlayer::processRaygun(void)
{

}

void CPlayer::processStunned(void)
{

}


// Process and decide if player is walking or standing
void CPlayer::processWalking(void)
{
	m_vx = 0;
	if(isStanding || isWalking)
	{
		if(g_pInput->getHoldedCommand(IC_RIGHT))
		{
				isStanding = false;
				isWalking = true;
				m_vx = WALK_SPEED;
				headingDirection = RIGHT;
		}
		else if(g_pInput->getHoldedCommand(IC_LEFT))
		{
				isStanding = false;
				isWalking = true;
				m_vx = -WALK_SPEED;
				headingDirection = LEFT;
		}
		else
		{
			isStanding = true;
			isWalking = false;
		}

		if(g_pInput->getHoldedCommand(IC_UP))
		{
				isStanding = false;
				isWalking = true;
				m_vy = -WALK_SPEED;
		}
		else if(g_pInput->getHoldedCommand(IC_DOWN))
		{
				isStanding = false;
				isWalking = true;
				m_vy = WALK_SPEED;
		}
	}

}

void CPlayer::processMovements(void)
{
	// Acceleration formula
	if(m_vx != 0)
		m_vx += m_ax;
	if(m_vy != 0)
		m_vy += m_ay;

	if(bisBlockedDown)
	{
		if(headingDirection == RIGHT && (isWalking || walkframe > 0))
		{
			if(walktimer < WALKANIMRATE)
			{
				//m_vx = 0;
				walktimer++;
			}
			else
			{
				walktimer = 0;
				if(walkframe < 3)
					walkframe++;
				else
					walkframe = 0;
			}

			if(walkframe != 0 && !isWalking)
				m_vx = 1;
		}
	}

	// Check, if player is blocked
	// If it is not the add Players velocity
	if((!bisBlockedRight && m_vx > 0) ||
		(!bisBlockedLeft && m_vx < 0))
		m_x += m_vx;

	if((!bisBlockedUp && m_vy < 0) ||
		(!bisBlockedDown && m_vy > 0))
		m_y += m_vy;
}

void CPlayer::processInertia(void)
{

}

void CPlayer::processFriction(void)
{
	frictiontimer++;
	if(frictiontimer > MAX_FRICTIONTIME)
		frictiontimer = 0;

	// We have two sections: friction in the air and on ground

	// friction in the air

	// friction on the ground: Division again in three section. Normal, slippery and ice
	// on the Ground
	if(bisBlockedDown)
	{
		// He is standing on something solid
		if(frictiontimer % 1 == 0)
		{
			if(m_vx > 0)
			{
				m_vx-=3;
				if(m_vx < 0)
					m_vx = 0;
			}
			if(m_vx < 0)
			{
				m_vx+=3;
				if(m_vx > 0)
					m_vx = 0;
			}
		}
	}

}

void CPlayer::processSwitches(void)
{

}

void CPlayer::processPogo(void)
{

}

void CPlayer::processJump(void)
{

}

void CPlayer::processFalling(void)
{

}

void CPlayer::showStatusBox(void)
{

}

unsigned short CPlayer::getPlayerframe(void)
{
	return playframe;
}

// select the appropriate player frame based on what he's doing
void CPlayer::applyFrame(void)
{
    playframe = 0;      // basic standing

    // select the frame assuming he's pointing right. ep1 does not select
    // a walk frame while fading--this is for the bonus teleporter in L13.
    if (isDying) playframe = PDIEFRAME + dieframe;
    else
    {
        if (frozentime) playframe = PFRAME_FROZEN + frozenframe;
        else if (isFiring) playframe = PFIREFRAME;
        else if (usePogostick) playframe = PFRAME_POGO + (Jumpmode==PREPAREPOGO);
        else if (Jumpmode) playframe += jumpframe;
        else if (isFalling) playframe += 13;
        else if (isWalking || isPushed) playframe += walkframe;
    }

    // if he's going left switch the frame selected above to the
    // appropriate one for the left direction
    if (headingDirection==LEFT && !isDying && !frozentime)
    {
       if (isFiring)
       {
          playframe++;
       }
       else if (usePogostick)
       {
          playframe+=2;
       }
       else if (Jumpmode || isFalling)
       {
          playframe+=6;
       }
       else
       {
          playframe+=4;
       }
    }
}
