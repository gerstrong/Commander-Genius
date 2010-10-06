/*
 * CPlayer.cpp
 *
 *  Created on: 05.10.2009
 *      Author: gerstrong
 *
 *  The Player logics are processed here.
 *  Some functions are common, others map and level specific.
 *  Those are stored in CPlayerWM.cpp and CPlayerLevel.cpp
 *
 */

#include "CPlayer.h"

#include "keen.h"
#include "sdl/CInput.h"
#include "sdl/sound/CSound.h"
#include "sdl/CVideoDriver.h"
#include "graphics/CGfxEngine.h"
#include "graphics/effects/CColorMerge.h"
#include "engine/spritedefines.h"
#include <stdlib.h>

size_t CPlayer::m_playerID = 0;

///
// Initialization Part
///
CPlayer::CPlayer(const char &Episode, short &Level, char &Difficulty,
				 bool *mp_level_completed, stOption *mp_option,
				 std::vector<CObject*> &m_Object, CMap &map) :
CObject(&map, 0, 0, OBJ_PLAYER),
m_episode(Episode),
m_level(Level),
m_difficulty(Difficulty),
pjumpupspeed_decrease(g_pBehaviorEngine->getPhysicsSettings().player.defaultjumpupdecreasespeed),
mp_levels_completed(mp_level_completed),
mp_option(mp_option),
mp_StatusScr(NULL)
{
	m_playerID++;
	mp_object = &m_Object;
	canbezapped = true;
	m_index = 0;

	pjumping = PNOJUMP;
	pfalling = false;
	psemisliding = false;
	psliding = false;
	ppogostick = false;
	pslowingdown = false;
    m_playingmode=WORLDMAP;


	// Set every value in the class to zero.
    memset(&inventory, 0, sizeof(stInventory));
    setDefaultStartValues();
    setDatatoZero();
}

// Sets the player data to the initial value,
// We also could put that in the init list of the constructor,
// but it would be much longer...
void CPlayer::setDatatoZero()
{
	// When worldmap is set up, use that frame
	sprite = PMAPDOWNFRAME;

	performCollisions();

	m_type = OBJ_PLAYER;
	moveTo(0,0);
	ankhtime = 0;
    exists = true;
	onscreen = true;
	pfallspeed = 0,
	pdir = pshowdir = (m_level==80) ? DOWN : RIGHT;
	inhibitfall = hideplayer = false;
  	pwalkframe = pwalkframea = 0;
    dpadcount = 0;
    hideplayer = false;
    mounted = false;
    ppogostick = false;
    pogofirsttime = false;
    plastfire = false;
    honorPriority = true;
    solid = true;
    bumped = false;

	
    pjumptime = 0;
    pjumping = PNOJUMP;
	
    mapplayx = mapplayy = 0;
    level_done = LEVEL_NOT_DONE;

    m_cheats_enabled = false;
    m_showStatusScreen = false;
	
    level_done_timer = 0;
  	dpadcount = dpadlastcount = 0;
  	beingteleported = false;

  	// This will setup the proper frames, so second, third and fourth player get the correct sprites
   	playerbaseframe = (m_index==0) ? 0 : SECOND_PLAYER_BASEFRAME+(m_index-1)*48;
	if(m_episode == 3) playerbaseframe--;

    // Set all the inventory to zero.
    memset(playcontrol, 0, sizeof(playcontrol));
}

// sets the lifes and some shots, and other stuff, depending what game it is.
// NOTE: This must only be called once, per new game.
void CPlayer::setDefaultStartValues()
{
	CPhysicsSettings &PhysicsSettings = g_pBehaviorEngine->getPhysicsSettings();
    pdie = PDIE_DEAD; // At first time he is revived when map is started
	godmode  = false;
    inventory.extralifeat = 20000;
    inventory.lives = 4;

    inventory.charges = PhysicsSettings.player.start_with_shots;

	// start with pogo stick in all episodes but 1
	inventory.HasPogo = PhysicsSettings.player.pogostickonstart;
}

// This function should be called, when the worldmap starts or any level is started
void CPlayer::setupforLevelPlay()
{
	plastfalling = true;
	solid = true;
	pshowdir = RIGHT;
	ppogostick = false;
	pjumping = PNOJUMP;
    psliding = psemisliding = false;
    pfrozentime = 0;
    keyprocstate = 0;         // KPROC_IDLE
    pjustjumped = pjustfell = true;
    pfireframetimer = 0;
    psupportingobject = lastsupportingobject = 0;
	supportedbyobject = false;
  	object_chosen = false;
    lastpogo = false;
  	hintused = false;
  	exitXpos = 0;
    pjustfell = false;
    plastfalling = false;
    pfalling = false;
    pwalking = playspeed = 0;
    xinertia = pinertia_y = 0;
    plastfire = pfiring = false;
    pwalkanimtimer = 0;
    inhibitfall = false;
  	m_Level_Trigger = LVLTRIG_NONE;
  	checkObjSolid();
}

bool CPlayer::scrollTriggers()
{
	int px, py, left, up, right, down, speed;
	bool scrollchanged=false;

	Uint16& scroll_x = mp_Map->m_scrollx;
	Uint16& scroll_y = mp_Map->m_scrolly;
	
	if (pdie) return scrollchanged;

	px = (getXPosition()>>STC)-scroll_x;
	py = (getYPosition()>>STC)-scroll_y;
	
	st_camera_bounds CameraBounds = g_pVideoDriver->getCameraBounds();
	left = CameraBounds.left;
	up = CameraBounds.up;
	right = CameraBounds.right;
	down = CameraBounds.down;
	speed = CameraBounds.speed;

	// left-right scrolling
	if(px > right && scroll_x < mp_Map->m_maxscrollx)
	{
		do{
			px = (getXPosition()>>STC)-scroll_x;
			mp_Map->scrollRight();
		}while(px > right+speed && scroll_x < mp_Map->m_maxscrollx);
		scrollchanged = true;
	}
	else if(px < left && scroll_x > 32)
	{
		do{
			px = (getXPosition()>>STC)-scroll_x;
			mp_Map->scrollLeft();
		}while(px < left-speed && scroll_x > 32);
		scrollchanged = true;
	}

	// up-down scrolling
	if (py > down && scroll_y < mp_Map->m_maxscrolly)
	{
		do{
			py = (getYPosition()>>STC)-scroll_y;
			mp_Map->scrollDown();
		}while(py > down+speed && scroll_y < mp_Map->m_maxscrolly);
		scrollchanged = true;
	}
	else if ( py < up && scroll_y > 32  )
	{
		do{
			py = (getYPosition()>>STC)-scroll_y;
			mp_Map->scrollUp();
		}while(py < up-speed && scroll_y > 32);
		scrollchanged = true;
	}

	// This will always snap correctly to the edge
	while(scroll_x < 32)
		mp_Map->scrollRight();
	while(scroll_x > mp_Map->m_maxscrollx)
		mp_Map->scrollLeft();
	while(scroll_y < 32)
		mp_Map->scrollDown();
	while(scroll_y > mp_Map->m_maxscrolly)
		mp_Map->scrollUp();

	return scrollchanged;
}

// handles walking. the walking animation is handled by gamepdo_walkinganim()
void CPlayer::Walking()
{
	int cur_pfastincrate;

    if (inhibitwalking && !psliding)
    {
		if (!pfrozentime||m_episode!=1)
			if (!pjumping && !pfalling)
				xinertia = 0;
		return;
    }
	
    // this prevents a "slipping" effect if you jump, say, right, then
    // start walking left just as you hit the ground
    if (pjustjumped && ((xinertia > 0 && pdir==LEFT) ||\
                        (xinertia < 0 && pdir==RIGHT)))\
    {
    	if(!ppogostick)
    		xinertia = 0;
    }
	
    // this code makes it so that if you jump/fall onto a semi-sliding
    // block you'll start moving a little
    if (!pjumping && !pfalling)
    {
    	pjustjumped = false;
		// on left/right press clear pjustjumped
		if ( (playcontrol[PA_X] < 0) || (playcontrol[PA_X] > 0) )
		{
			pjustfell = 0;
		}
		
		// if we fall onto a semislide tile with no inertia
		// don't move!.
		if (pjustfell && psemisliding)
		{
			if (pdir==RIGHT)
			{
				if (blockedr)
				{
					pjustfell = 0;
				}
				else
				{
					pshowdir = pdir;
				}
			}
			else
			{
				if (blockedl)
				{
					pjustfell = 0;
				}
				else
				{
					pshowdir = pdir;
				}
			}
		}
    }
	
    // test if we're trying to walk
    if ((psemisliding && xinertia!=0) || (((playcontrol[PA_X] < 0) || (playcontrol[PA_X] > 0) || (( (playcontrol[PA_Y] < 0) || (playcontrol[PA_Y] > 0)) && m_playingmode == WORLDMAP )) && !inhibitwalking))
    {
		// we just started walking or we changed directions suddenly?
		if (pwalking == 0 || ((lastpdir==RIGHT && pdir==LEFT)||(lastpdir==LEFT && pdir==RIGHT)))
		{
			pwalkanimtimer = 0;
			pwalkframe = 1;
			pwalkincreasetimer = 0;
			
			if (!pjumping && !pfalling)
			{
				if(!ppogostick) // Only if he stays on the ground (No pogoing)
					xinertia /= 2;
				pinertia_y = 0;
			}
			
			lastpdir = pdir;
			pwalking = 1;
		}
    }
    else
    {   // end "d-pad down and bit sliding"
    	if(xinertia != 0)
    		pslowingdown=1;
    	else
    	{
    		pslowingdown=0;
    		pwalking = 0;
    	}
    }

	CPhysicsSettings &PhysicsSettings = g_pBehaviorEngine->getPhysicsSettings();
	int pmaxspeed = PhysicsSettings.player.max_x_speed;

	// when sliding on ice force maximum speed
	if (psliding)
	{
		if (pjumping != PPREPAREJUMP &&
			pjumping != PPREPAREPOGO)
		{
			// reset walk frame because we have no walk animation while on ice
			pwalkframe = 0;
			// keep player sliding at maximum speed
			if (pdir==RIGHT)
				xinertia = pmaxspeed;
			else if (pdir==LEFT)
				xinertia = -pmaxspeed;
		}
		return;
	}
	else if (!pwalking) return;    // don't run rest of sub if not walking
	// if we get here we're walking and not sliding
	
	// increase player inertia while walk key held down
	if (ppogostick)
        cur_pfastincrate = PFASTINCRATE_POGO;
	else
        cur_pfastincrate = PFASTINCRATE;

	if (playcontrol[PA_X] > 0 && !ppogostick)
	{ // RIGHT key down
		// quickly reach PLAYER_FASTINCMAXSPEED
		if (pwalkincreasetimer>=cur_pfastincrate)
		{
			if(pfalling) xinertia+=(1<<2);
			//else xinertia+=(1<<4);
			else xinertia+=(1<<3);
			pwalkincreasetimer=0;
		}
		else	pwalkincreasetimer++;
		
		// prevent sliding on map
		if ( m_playingmode == WORLDMAP )
		{
			if(xinertia < 0)  xinertia = 0;
			else	xinertia = playcontrol[PA_X]*PhysicsSettings.player.max_x_speed/150;
		}
		
		// increase up to max speed every time frame is changed
		if (!pwalkanimtimer && xinertia < pmaxspeed)	xinertia+=(1<<4);
	}
	else if (playcontrol[PA_X] < 0 && !ppogostick )
	{ 	// LEFT key down
		// quickly reach PFASTINCMAXSPEED
		if (pwalkincreasetimer>=cur_pfastincrate)
		{
			if(pfalling) xinertia-=(1<<2);
			//else xinertia-=(1<<4);
			else xinertia-=(1<<3);
			pwalkincreasetimer=0;
		}
		else	pwalkincreasetimer++;
		
		// prevent sliding on map
		if ( m_playingmode == WORLDMAP )
		{
			if(xinertia > 0)  xinertia = 0;
			else	xinertia = playcontrol[PA_X]*PhysicsSettings.player.max_x_speed/150;
		}
		
		// decrease down to max speed every time frame is changed
		if (!pwalkanimtimer && xinertia > -pmaxspeed)	xinertia-=(1<<4);
	}
	
	if (playcontrol[PA_Y] > 0)
	{
		// quickly reach PFASTINCMAXSPEED
		if (pwalkincreasetimer>=PFASTINCRATE && pinertia_y<PhysicsSettings.player.max_x_speed)
		{
			pinertia_y++;
			pwalkincreasetimer=0;
		}
		else
		{
			pwalkincreasetimer++;
		}
		// increase up to max speed every time frame is changed
		if (!pwalkanimtimer && pinertia_y<pmaxspeed)
		{
			pinertia_y++;
		}
		
		// prevent sliding on map
		if ( m_playingmode == WORLDMAP )
		{
			if(pinertia_y < 0)
			{
				pinertia_y = 0;
			}
			else
			{
				pinertia_y=playcontrol[PA_Y]*PhysicsSettings.player.max_x_speed/150;
			}
		}
		
	}
	else if (playcontrol[PA_Y] < 0)
	{
		// quickly reach PFASTINCMAXSPEED
		if (pwalkincreasetimer>=PFASTINCRATE && pinertia_y>-PhysicsSettings.player.max_x_speed)
		{
			pinertia_y--;
			pwalkincreasetimer=0;
		}
		else
		{
			pwalkincreasetimer++;
		}
		// increase up to max speed every time frame is changed
		if (!pwalkanimtimer && pinertia_y>-pmaxspeed)
		{
			pinertia_y--;
		}
		
		// prevent sliding on map
		if ( m_playingmode == WORLDMAP )
		{
			if(pinertia_y > 0)
			{
				pinertia_y = 0;
			}
			else
			{
				pinertia_y=playcontrol[PA_Y]*PhysicsSettings.player.max_x_speed/150;
			}
		}
	}
}

// animation for walking
void CPlayer::WalkingAnimation()
{
	if(pfrozentime)
		return;

    // no walk animation while sliding
    if (inhibitwalking || psliding ) return;
	
    // should we do walk animation?
    if (pwalking  || psemisliding)
    {
    	int walkanimrate; // walk animation speed according to player speed
		
    	if(!psemisliding) {
    		walkanimrate = 101*PWALKANIMRATE/(treshold+1);
        	if(walkanimrate > 150)
				walkanimrate = 150;
    	}
    	else if(xinertia!=0) {
    		walkanimrate = PWALKANIMRATE;
    	}
    	else {
    		pwalkframe = 0;
    		return;
    	}
		
    	// ** do walk animation **
        if (pwalkanimtimer > walkanimrate)
        {   // time to change walking frame
			// make walk noise
			if ( (!pjumping && !pfalling) || m_playingmode == WORLDMAP )
			{
				if (!pfrozentime && pwalking)
				{
					if (pwalkframea&1)
						g_pSound->playStereofromCoord(SOUND_KEEN_WALK, PLAY_NOW, scrx);
					else
						g_pSound->playStereofromCoord(SOUND_KEEN_WALK2, PLAY_NOW, scrx);
					
					if( m_playingmode != WORLDMAP && (blockedr || blockedl) )
					{
						g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, scrx);
						// It is not bumping a head, but walking in some direction and being blocked
					}
					else if ( m_playingmode == WORLDMAP )
					{
						// Same on world map!
						bool play=false;
						play |= (blockedu && pdir == UP);
						play |= (blockedd && pdir == DOWN);
						play |= (blockedl && pdir == LEFT);
						play |= (blockedr && pdir == RIGHT);
						
						if (play)
							g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, scrx);
					}
					
				}
			}
			// increase walk frame and wrap it to 1st frame if needed
			if (pwalkframea < 4)
            { pwalkframea++; }
			else
            { pwalkframea=1; }
			
			pwalkanimtimer = 0;
        }
        else
        {  // did not change walk frame
			pwalkanimtimer++;
        }
		
        // set walk frame: map frame "4" to frame "2", this gives a
        // sequence of 1,2,3,2,1,2,3,2,1,2,3,2....
        if (pwalkframea==4)
        {
			pwalkframe = 2;
        }
        else
        {
			pwalkframe = pwalkframea;
        }
    }
}

// handles inertia and friction for the X direction
// (this is where the xinertia is actually applied to playx)
void CPlayer::InertiaAndFriction_X()
{
	int friction_rate;
	treshold = 0;
	CPhysicsSettings &PhysicsSettings = g_pBehaviorEngine->getPhysicsSettings();

	// Calculate Threshold of your analog device for walking animation speed!
	if(!pfrozentime)
	{
		treshold = playcontrol[PA_X];

		int pmaxspeed;
		int pmaxmovespeed = PhysicsSettings.player.max_x_speed;


		if( (!pjumping && !pfalling &&
				!psemisliding && !psliding && !ppogostick && !pslowingdown) || m_playingmode==WORLDMAP )
		{
			if(treshold < playcontrol[PA_Y] && playcontrol[PA_Y] > 0 )
				treshold = playcontrol[PA_Y];

			if(treshold > playcontrol[PA_Y] && playcontrol[PA_Y] < 0 )
				treshold = playcontrol[PA_Y];

			if(treshold < 0)
				treshold *= (-1);
		}
		else	treshold = 100;

		pmaxspeed = treshold*pmaxmovespeed/100;

		if(xinertia > pmaxspeed)
			xinertia = pmaxspeed;
		if(xinertia < -pmaxspeed)
			xinertia = -pmaxspeed;
	}

	if(m_level == 80) // We are on World map
		verifySolidLevels();
	
	// apply xinertia
	// (unless we're about to make a pogo jump)
	if ( pjumping != PPREPAREPOGO && pjumping != PPREPAREJUMP)
	{
		int dx=xinertia;
		// check first if the player is not blocked
		if( (!blockedr and dx>0) or (!blockedl and dx<0) )
			moveXDir(dx);
		else
			xinertia = 0;
	}
	
	// if we stopped walking (i.e. left or right not held down) apply friction
	// there's no friction if we're semisliding

	if (!(playcontrol[PA_X] < 0) && !(playcontrol[PA_X] > 0) &&
		!psemisliding)
	{
		// determine friction rate--different rates for on ground and in air
		if (m_playingmode == WORLDMAP)
		{
			friction_rate = PFRICTION_RATE_WM;
		}
		else
		{
	        if (!pfalling && !pjumping)
	        {
				friction_rate = PFRICTION_RATE_ONGROUND;
	        }
	        else
	        {
				friction_rate = PFRICTION_RATE_INAIR;
	        }
		}
		
		// and apply friction to xinertia
		// when pogoing apply friction till we get down to PFASTINCMAXSPEED
		// then stop the friction
		if (!ppogostick || (xinertia >  PhysicsSettings.player.max_x_speed) ||
						   (xinertia < -PhysicsSettings.player.max_x_speed) )
		{
			if (!pfrozentime || m_episode!=1)
			{   // disable friction while frozen
				// here the wall animation must be applied!
				if(!psliding)
					decreaseXInertia(friction_rate);
			}
		}
	}
}

// handles inertia and friction for the Y direction
// (this is where the inertia is actually applied to playx)
void CPlayer::InertiaAndFriction_Y()
{
	if (hideplayer)
	{
		pinertia_y = 0;
		return;
	}
	
	// if we hit a solid object kill all inertia
	if (pinertia_y > 0 && blockedd)
		pinertia_y = 0;
	else if (pinertia_y < 0 && blockedu)
		pinertia_y = 0;
	else if( isWMSolid(getXMidPos(), getYMidPos()) )
		pinertia_y = 0;
	
	// check first if the player is not blocked by a level
	verifySolidLevels();

	// then apply pinertia_y
	if( (!blockedu && pinertia_y<0) || (!blockedd && pinertia_y>0) )
		moveYDir(pinertia_y);
	
	// if we stopped walking (i.e. LRUD not held down) apply friction
    if (playcontrol[PA_Y] == 0)
    {
		// and apply friction to pinertia_y
		if (pinertia_y < 0)
		{
			pinertia_y += PFRICTION_RATE_WM<<4;
			if(pinertia_y > 0) pinertia_y = 0;
		}
		else if (pinertia_y > 0)
		{
			pinertia_y -= PFRICTION_RATE_WM<<4;
			if(pinertia_y < 0) pinertia_y = 0;
		}
    }
}

/**
 * \brief Everything the player controls is processed here!
 */
void CPlayer::ProcessInput()
{
	// Entry for every player
	playcontrol[PA_X] = 0;
	playcontrol[PA_Y] = 0;
	
	if(g_pInput->getHoldedCommand(m_index, IC_LEFT))
		playcontrol[PA_X] -= 100;
	else if(g_pInput->getHoldedCommand(m_index, IC_RIGHT))
		playcontrol[PA_X] += 100;
	
	if(g_pInput->getHoldedCommand(m_index, IC_DOWN))
		playcontrol[PA_Y] += 100;
	else if(g_pInput->getHoldedCommand(m_index, IC_UP))
		playcontrol[PA_Y] -= 100;

	if(g_pInput->getHoldedCommand(m_index, IC_UPPERLEFT))
	{
		playcontrol[PA_X] -= 100;
		playcontrol[PA_Y] -= 100;
	}
	else if(g_pInput->getHoldedCommand(m_index, IC_UPPERRIGHT))
	{
		playcontrol[PA_X] += 100;
		playcontrol[PA_Y] -= 100;
	}
	else if(g_pInput->getHoldedCommand(m_index, IC_LOWERLEFT))
	{
		playcontrol[PA_X] -= 100;
		playcontrol[PA_Y] += 100;
	}
	else if(g_pInput->getHoldedCommand(m_index, IC_LOWERRIGHT))
	{
		playcontrol[PA_X] += 100;
		playcontrol[PA_Y] += 100;
	}

	if(!pfiring)
	{
		if(g_pInput->getHoldedCommand(m_index, IC_JUMP))
			playcontrol[PA_JUMP]++;
		else
			playcontrol[PA_JUMP] = 0;

		playcontrol[PA_POGO]   = g_pInput->getHoldedCommand(m_index, IC_POGO)   ? 1 : 0;
	}

	playcontrol[PA_FIRE]   = g_pInput->getHoldedCommand(m_index, IC_FIRE)   ? 1 : 0;
	playcontrol[PA_STATUS] = g_pInput->getHoldedCommand(m_index, IC_STATUS) ? 1 : 0;
	
	// The possibility to charge jumps. This is mainly used for the pogo.
	if( playcontrol[PA_JUMP] > 50) playcontrol[PA_JUMP] = 50;
	
	if(g_pInput->getTwoButtonFiring(m_index))
	{
		if(playcontrol[PA_FIRE])
		{
			playcontrol[PA_FIRE] = 0;
		}
		else if(playcontrol[PA_JUMP] && playcontrol[PA_POGO])
		{
			playcontrol[PA_FIRE] = 1;
			playcontrol[PA_JUMP] = 0;
			playcontrol[PA_POGO] = 0;
		}
	}
}

/**
 * \brief This function is processed while Keen is walking through the exit-door.
 */
void CPlayer::ProcessExitLevel()
{
	inhibitfall = true;
	ankhtime = 0;
	if (level_done==LEVEL_DONE_WALK)
	{
		walkbehindexitdoor();

        // keep him going right
        pdir = pshowdir = RIGHT;
        // make keen walk slowly through the exit door
        playcontrol[PA_X] = 80;
        if (xinertia > PMAXEXITDOORSPEED)
        {
        	xinertia = PMAXEXITDOORSPEED;
        }
	}
	else
	{
        // he's all the way through the door; hold him still
		xinertia = 0;
        pwalking = false;
	}

	if (level_done_timer > LEVEL_DONE_TOTAL_WAIT_TIME)
	{
		// Now, that the level is complete, sprite can be shown again, and now goto map!
		int width = w>>(CSF-4);
		int frame = playerbaseframe;
		if(m_episode == 3) frame++;

		g_pGfxEngine->getSprite(frame+0).setWidth(width);
		g_pGfxEngine->getSprite(frame+1).setWidth(width);
		g_pGfxEngine->getSprite(frame+2).setWidth(width);
		g_pGfxEngine->getSprite(frame+3).setWidth(width);

		level_done_timer = 0;
		level_done = LEVEL_COMPLETE;
		for(size_t i = 0 ; i<4 ; i++)
			inventory.canlooseitem[i] = false;
	}
	else if (level_done_timer > LEVEL_DONE_STOPWALKING_TIME)
		level_done = LEVEL_DONE_WAIT;

	level_done_timer++;
}

void CPlayer::StatusBox()
{
	if( g_pInput->getPressedCommand(m_index, IC_STATUS) && !level_done )
		m_showStatusScreen=true; // PlayGame class detects that variable and launches drawStatusScreen()
}

void CPlayer::freeze()
{
	if ( godmode ) return;
	if ( ankhtime ) return;
	// give the player a little "kick"

	CPhysicsSettings &PhysicsSettings = g_pBehaviorEngine->getPhysicsSettings();

	pjumpupspeed = PhysicsSettings.player.maxjumpspeed;

	pjumping = PJUMPUP;
	pjustjumped = true;

	// and freeze him (stun him on ep2/3)
	pfrozentime = PFROZEN_TIME;
	pfrozenframe = 0;
	pfrozenanimtimer = 0;
	ppogostick = false;
	playcontrol[PA_X] = 0;
}

bool CPlayer::checkObjSolid()
{
	supportedbyobject = false;

	std::vector<CObject*>::iterator it_obj = mp_object->begin();
	for( ; it_obj != mp_object->end() ; it_obj++ )
	{
		if((*it_obj)->cansupportplayer)
		{	// can support player
			if(getXRightPos() >= (*it_obj)->getXLeftPos()  &&
					getXLeftPos() <= (*it_obj)->getXRightPos() )
			{
				if(getYUpPos() <= (*it_obj)->getYDownPos()+(1<<STC)  &&
					getYUpPos() >= (*it_obj)->getYMidPos() )
				{	// In this case the object pushs the player down!
					pjumping = PNOJUMP;
					int dy = (*it_obj)->getYDownPos() - getYUpPos();
					//supportedbyobject = true;
					blockedu = true;
					moveDown(dy);
				}
				else if(getYDownPos() >= (*it_obj)->getYUpPos()-(3<<STC)  &&
						getYDownPos() <= (*it_obj)->getYMidPos() )
				{	// In this case stand on the object
					pfalling = false;
					if(pjumping == PJUMPLAND)
						pjumping = PNOJUMP;
					supportedbyobject = true;
					psupportingobject = (*it_obj)->m_index;
					int dy = (*it_obj)->getYUpPos() - getYDownPos()+1;
					if(pjumping == PNOJUMP)
						moveYDir(dy);
					blockedd = true;
					break;
				}
			}
		}
	}
	return true;
}

// Draws the Status screen and return false, when it's still open.
bool CPlayer::drawStatusScreen()
{
	if(m_showStatusScreen)
	{
		if(mp_StatusScr == NULL)
			mp_StatusScr = new CStatusScreen(m_episode, &inventory, mp_levels_completed, ankhtime, playerbaseframe);
		
		mp_StatusScr->draw();
		
		if( mp_StatusScr->isClosed() )
		{
			m_showStatusScreen = false;
			delete mp_StatusScr;
			mp_StatusScr = NULL;
		}
		else if( g_pInput->getPressedAnyCommand() )
		{
			mp_StatusScr->close();
		}
		return false;
	}
	else return true;
}


///
// Cleanup Part
///
CPlayer::~CPlayer() {
	m_playerID--;
}
