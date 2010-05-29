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

#include "../keen.h"
#include "../sdl/CInput.h"
#include "../sdl/sound/CSound.h"
#include "../sdl/CVideoDriver.h"
#include "../graphics/CGfxEngine.h"
#include "../graphics/effects/CColorMerge.h"
#include "../engine/spritedefines.h"
#include "../engine/vorticon/CCamera.h"
#include <stdlib.h>

///
// Initialization Part
///
CPlayer::CPlayer(const char &Episode, short &Level, char &Difficulty,
				 short player_index, bool *mp_level_completed, stOption *mp_option,
				 std::vector<CObject> &m_Object, CMap &map) :
CObject(&map, player_index),
m_episode(Episode),
m_level(Level),
m_difficulty(Difficulty),
m_Ankhshield(CObject(&map)),
mp_levels_completed(mp_level_completed),
mp_map(NULL),
mp_option(mp_option),
mp_StatusScr(NULL)
{
	mp_object = &m_Object;

	// Ankhshield is a special object the player is holded.
	// It's normally seen in Ep3 and when he gets invincible by the ankh
	m_Ankhshield.solid = false;
	m_Ankhshield.exists = true;

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

	m_type = OBJ_PLAYER;
	moveTo(0,0);
    exists = true;
	onscreen = true;
	pfallspeed = 0,
	pdir = pshowdir = DOWN;
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
	
    pjumping = pjumptime = 0;
	
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
    memset(playcontrol, 0, PA_MAX_ACTIONS*sizeof(char));
}

// sets the lifes and some shots, and other stuff, depending what game it is.
// NOTE: This must only be called once, per new game.
void CPlayer::setDefaultStartValues()
{
    pdie = PDIE_DEAD; // At first time he is revived when map is started
	godmode  = false;
    inventory.extralifeat = 20000;
    inventory.lives = 4;

    if (m_episode==1) inventory.charges = 0;
	else if (m_episode==2) inventory.charges = 3;
	else if (m_episode==3) inventory.charges = 5;

	// start with pogo stick in all episodes but 1
	inventory.HasPogo = (m_episode==1) ? false : true;
}

// This function should be called, when the worldmap starts or any level is started
void CPlayer::setupforLevelPlay()
{
	plastfalling = true;
	solid = true;
	pfalling = false;
	pshowdir = RIGHT;
	ppogostick = false;
	pjumping = PNOJUMP;
    psliding = psemisliding = false;
    pfrozentime = 0;
    ankhtime = 0;
    keyprocstate = 0;         // KPROC_IDLE
    pjustjumped = pjustfell = true;
    pfireframetimer = 0;
    psupportingobject = lastsupportingobject = 0;
	supportedbyobject = false;
  	object_chosen = false;
    lastpogo = false;
  	hintused = false;
  	exitXpos = 0;
    pjustfell = plastfalling = pfalling = false;
    pwalking = playspeed = 0;
    xinertia = pinertia_y = 0;
    playpushed_x = 0;
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

	Uint16& scroll_x = mp_map->m_scrollx;
	Uint16& scroll_y = mp_map->m_scrolly;
	
	if (pdie) return scrollchanged;

	px = (getXPosition()>>STC)-scroll_x;
	py = (getYPosition()>>STC)-scroll_y;
	
	left = g_pCamera->getScrollLeft();
	up = g_pCamera->getScrollUp();
	right = g_pCamera->getScrollRight();
	down = g_pCamera->getScrollDown();
	speed = g_pCamera->getScrollSpeed();

	// left-right scrolling
	if(px > right && scroll_x < mp_map->m_maxscrollx)
	{
		do{
			px = (getXPosition()>>STC)-scroll_x;
			mp_map->scrollRight();
		}while(px > right+speed && scroll_x < mp_map->m_maxscrollx);
		scrollchanged = true;
	}
	else if(px < left && scroll_x > 32)
	{
		do{
			px = (getXPosition()>>STC)-scroll_x;
			mp_map->scrollLeft();
		}while(px < left-speed && scroll_x > 32);
		scrollchanged = true;
	}

	// up-down scrolling
	if (py > down && scroll_y < mp_map->m_maxscrolly)
	{
		do{
			py = (getYPosition()>>STC)-scroll_y;
			mp_map->scrollDown();
		}while(py > down+speed && scroll_y < mp_map->m_maxscrolly);
		scrollchanged = true;
	}
	else if ( py < up && scroll_y > 32  )
	{
		do{
			py = (getYPosition()>>STC)-scroll_y;
			mp_map->scrollUp();
		}while(py < up-speed && scroll_y > 32);
		scrollchanged = true;
	}

	// This will always snap correctly to the edge
	while(scroll_x < 32)
		mp_map->scrollRight();
	while(scroll_x > mp_map->m_maxscrollx)
		mp_map->scrollLeft();
	while(scroll_y < 32)
		mp_map->scrollDown();
	while(scroll_y > mp_map->m_maxscrolly)
		mp_map->scrollUp();

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

	int pmaxspeed;
	pmaxspeed = mp_PhysicsSettings->player.max_x_speed;

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

	if (playcontrol[PA_X] > 0 && !ppogostick && !playpushed_x)
	{ // RIGHT key down
		// quickly reach PLAYER_FASTINCMAXSPEED
		if (pwalkincreasetimer>=cur_pfastincrate)
		{
			if(pfalling) xinertia+=(1<<2);
			else xinertia+=(1<<4);
			pwalkincreasetimer=0;
		}
		else	pwalkincreasetimer++;
		
		// prevent sliding on map
		if ( m_playingmode == WORLDMAP )
		{
			if(xinertia < 0)  xinertia = 0;
			else	xinertia = playcontrol[PA_X]*mp_PhysicsSettings->player.max_x_speed/150;
		}
		
		// increase up to max speed every time frame is changed
		if (!pwalkanimtimer && xinertia < pmaxspeed)	xinertia+=(1<<4);
	}
	else if (playcontrol[PA_X] < 0 && !ppogostick && !playpushed_x)
	{ // LEFT key down
		// quickly reach PFASTINCMAXSPEED
		if (pwalkincreasetimer>=cur_pfastincrate)
		{
			if(pfalling) xinertia-=(1<<2);
			else xinertia-=(1<<4);
			pwalkincreasetimer=0;
		}
		else	pwalkincreasetimer++;
		
		// prevent sliding on map
		if ( m_playingmode == WORLDMAP )
		{
			if(xinertia > 0)  xinertia = 0;
			else	xinertia = playcontrol[PA_X]*mp_PhysicsSettings->player.max_x_speed/150;
		}
		
		// decrease down to max speed every time frame is changed
		if (!pwalkanimtimer && xinertia > -pmaxspeed)	xinertia-=(1<<4);
	}
	
	if (playcontrol[PA_Y] > 0)
	{
		// quickly reach PFASTINCMAXSPEED
		if (pwalkincreasetimer>=PFASTINCRATE && pinertia_y<mp_PhysicsSettings->player.max_x_speed)
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
				pinertia_y=playcontrol[PA_Y]*mp_PhysicsSettings->player.max_x_speed/150;
			}
		}
		
	}
	else if (playcontrol[PA_Y] < 0)
	{
		// quickly reach PFASTINCMAXSPEED
		if (pwalkincreasetimer>=PFASTINCRATE && pinertia_y>-mp_PhysicsSettings->player.max_x_speed)
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
				pinertia_y=playcontrol[PA_Y]*mp_PhysicsSettings->player.max_x_speed/150;
			}
		}
	}
}

// animation for walking
void CPlayer::WalkingAnimation()
{
    // no walk animation while sliding
    if (inhibitwalking || psliding ) return;
	
    // should we do walk animation?
    if (pwalking  || playpushed_x || psemisliding)
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
						
						short play=0;
						
						if (blockedu && pdir == UP)
							play=1;
						
						if (blockedd && pdir == DOWN)
							play=1;
						
						if (blockedl && pdir == LEFT)
							play=1;
						
						if (blockedr && pdir == RIGHT)
							play=1;
						
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
// (this is where the inertia/playpushed_x is actually applied to playx)
void CPlayer::InertiaAndFriction_X()
{
	int friction_rate;
	treshold = 0;

	// Calculate Threshold of your analog device for walking animation speed!
	if(!pfrozentime)
	{
		treshold = playcontrol[PA_X];

		int pmaxspeed;
		int pmaxmovespeed = mp_PhysicsSettings->player.max_x_speed;


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
	
	// If player is blocked against a wall, decrease the inertia
	if( ( (blockedl && xinertia<0 ) || (blockedr && xinertia>0) ) ) {
		if(playcontrol[PA_X] == 0) xinertia = 0;
		else decreaseXInertia(1);
	}


	// apply xinertia and playpushed_x inertia
	// (unless we're about to make a pogo jump)
	if ( pjumping != PPREPAREPOGO && pjumping != PPREPAREJUMP)
	{
		int dx=xinertia + playpushed_x;
		// check first if the player is not blocked
		if( (!blockedr and dx>0) or (!blockedl and dx<0) )
			moveXDir(dx, (g_pInput->getHoldedKey(KTAB) && mp_option[OPT_CHEATS].value) );
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
	        if (!pfalling & !pjumping)
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
		if (!ppogostick || (xinertia >  mp_PhysicsSettings->player.max_x_speed) ||
						   (xinertia < -mp_PhysicsSettings->player.max_x_speed) )
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

void CPlayer::ProcessInput()
{
	// Entry for every player
	playcontrol[PA_X] = 0;
	playcontrol[PA_Y] = 0;
	
	if(g_pInput->getHoldedCommand(m_index, IC_LEFT))
		playcontrol[PA_X] -= 100;
	if(g_pInput->getHoldedCommand(m_index, IC_RIGHT))
		playcontrol[PA_X] += 100;
	
	if(g_pInput->getHoldedCommand(m_index, IC_UP))
		playcontrol[PA_Y] -= 100;
	if(g_pInput->getHoldedCommand(m_index, IC_DOWN))
		playcontrol[PA_Y] += 100;
	
	if(g_pInput->getHoldedCommand(m_index, IC_JUMP))
		playcontrol[PA_JUMP]++;
	else
		playcontrol[PA_JUMP] = 0;
	
	playcontrol[PA_POGO]   = g_pInput->getHoldedCommand(m_index, IC_POGO)   ? 1 : 0;
	playcontrol[PA_FIRE]   = g_pInput->getHoldedCommand(m_index, IC_FIRE)   ? 1 : 0;
	playcontrol[PA_STATUS] = g_pInput->getHoldedCommand(m_index, IC_STATUS) ? 1 : 0;
	
	// The possibility to charge jumps. This is mainly used for the pogo.
	if( playcontrol[PA_JUMP] > 50) playcontrol[PA_JUMP] = 50;
	
	// are we doing the keen-walking-through-exit door animation?
	if (level_done)
	{	// don't let player control keen
		memset(playcontrol,0,PA_MAX_ACTIONS);
		
		inhibitfall = true;
		if (level_done==LEVEL_DONE_WALK)
		{
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
		}
		else if (level_done_timer > LEVEL_DONE_STOPWALKING_TIME)
			level_done = LEVEL_DONE_WAIT;
		
		level_done_timer++;
		return;
	}
	
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

void CPlayer::StatusBox()
{
	if( g_pInput->getPressedCommand(m_index, IC_STATUS) )
		m_showStatusScreen=true; // PlayGame class detects that variable and launches drawStatusScreen()
}

void CPlayer::freeze()
{
	if ( godmode ) return;
	if ( ankhtime) return;
	// give the player a little "kick"
	pjumpupspeed = mp_PhysicsSettings->player.maxjumpspeed;
	pjumpupspeed_decrease = mp_PhysicsSettings->player.defaultjumpupdecreasespeed;

	pjumping = PJUMPUP;
	//pjumpupspeed_decreasetimer = 0;
	pjustjumped = true;

	// and freeze him (stun him on ep2/3)
	pfrozentime = PFROZEN_TIME;
	pfrozenframe = 0;
	pfrozenanimtimer = 0;
	ppogostick = false;
}

bool CPlayer::checkObjSolid()
{
	supportedbyobject = false;

	// This code prevents Keen making stick in the air when a supported object leaves him
	if(getYDownPos()+1 == (((getYDownPos()+1)>>CSF)<<CSF))
		blockedd = checkSolidD(getXLeftPos(), getXRightPos(), getYDownPos()+1);
	else
		blockedd = false;

	std::vector<CObject>::iterator it_obj = mp_object->begin();
	for( ; it_obj != mp_object->end() ; it_obj++ )
	{
		if(it_obj->cansupportplayer)
		{	// can support player
			if(getXRightPos() >= it_obj->getXLeftPos()  &&
					getXLeftPos() <= it_obj->getXRightPos() )
			{
				if(getYUpPos() >= it_obj->getYUpPos()-(1<<STC)  &&
					getYUpPos() <= it_obj->getYMidPos() )
				{	// In this case the object pushs the player down!
					pjumping = PNOJUMP;
					int dy = it_obj->getYDownPos() - getYUpPos();
					moveDown(dy);
				}
				else if(getYDownPos() >= it_obj->getYUpPos()-(1<<STC)  &&
						getYDownPos() <= it_obj->getYMidPos() )
				{	// In this case stand on the object
					pfalling = false;
					blockedd = true;
					if(pjumping == PJUMPLAND)
						pjumping = PNOJUMP;
					supportedbyobject = true;
					psupportingobject = it_obj->m_index;
					int dy = it_obj->getYUpPos() - getYDownPos()+1;
					moveYDir(dy);
					break;
				}
			}
		}
	}
	return true;
}

// Draws the Player actual frame
// Master class CObject is called, but also his puppy
// object ankhframe if he is in this status.
void CPlayer::draw()
{
	CObject::draw();

	// Here comes the part of the ankhshield
	if(ankhtime)
		m_Ankhshield.draw();
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
			g_pGfxEngine->pushEffectPtr(new CColorMerge(16));
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
}
