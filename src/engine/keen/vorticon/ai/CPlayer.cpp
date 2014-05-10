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

#include <base/CInput.h>
#include "sdl/audio/Audio.h"
#include <base/video/CVideoDriver.h>
#include "graphics/GsGraphics.h"
#include "engine/core/spritedefines.h"
#include <stdlib.h>

///
// Initialization Part
///
CPlayer::CPlayer(bool *mpLevelCompleted,
                 CMap &map, const int sprVar) :
CVorticonSpriteObject(&map, 0, 0, OBJ_PLAYER, sprVar),
pjumpupspeed_decrease(g_pBehaviorEngine->getPhysicsSettings().player.defaultjumpupdecreasespeed),
mp_levels_completed(mpLevelCompleted),
mp_option(g_pBehaviorEngine->m_option)
{
    canbezapped = true;
    m_index = 0;


    pjumping = PNOJUMP;
    pfalling = false;
    psemisliding = false;
    psliding = false;
    ppogostick = false;
    pslowingdown = false;
    m_playingmode = WORLDMAP;


    // Set every value in the class to zero.
    memset(&inventory, 0, sizeof(stInventory));
    setDefaultStartValues();
    setDatatoZero();
}


CPlayer::CPlayer(const CPlayer &player) :
CVorticonSpriteObject(player.mp_Map,
                      player.getXPosition(), player.getYPosition(),
                      OBJ_PLAYER, player.getSpriteVariantId() ),
pjumpupspeed_decrease(player.pjumpupspeed_decrease),
mp_levels_completed(player.mp_levels_completed),
mp_option(g_pBehaviorEngine->m_option)
{
    //mp_object = player.mp_object;
    canbezapped = true;
    m_index = 0;


    pjumping = PNOJUMP;
    pfalling = false;
    psemisliding = false;
    psliding = false;
    ppogostick = false;
    pslowingdown = false;
    m_playingmode = WORLDMAP;
    playpushed_decreasetimer = 0;


    // Set every value in the class to zero.
    memset(&inventory, 0, sizeof(stInventory));
    setDefaultStartValues();
    setDatatoZero();
    pinertia_y = 0;
}

CPlayer& CPlayer::operator=(const CPlayer &player)
{
    pjumpupspeed_decrease = player.pjumpupspeed_decrease;
    mp_levels_completed = player.mp_levels_completed;
    mp_option = g_pBehaviorEngine->m_option;
    canbezapped = true;
    m_index = 0;

    pjumping = PNOJUMP;
    pfalling = false;
    psemisliding = false;
    psliding = false;
    ppogostick = false;
    pslowingdown = false;
    m_playingmode = WORLDMAP;


    // Set every value in the class to zero.
    memset(&inventory, 0, sizeof(stInventory));
    setDefaultStartValues();
    setDatatoZero();
    
    return *this;
}


void CPlayer::setupCameraObject()
{
    mpCamera.reset(new CCamera(mp_Map, 0, 0, this));
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
	ankhtime = 0;
    exists = true;
	onscreen = true;
	pfallspeed = 0;

    const int ep = g_pBehaviorEngine->getEpisode();
    const int level = mp_Map->getLevel();

    if(level==80)
		pShowDir = Vector2D<direction_t>(CENTER,DOWN);
	else
		pShowDir = Vector2D<direction_t>(RIGHT,CENTER);

	pDir = pShowDir;
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
    dpadcount /*= dpadlastcount*/ = 0;
  	beingteleported = false;


    // This will setup the proper frames, in episode 3, there are differences
    playerbaseframe = 0;
    if(ep == 3) playerbaseframe--;

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
	pShowDir.x = RIGHT;
	ppogostick = false;
	pjumping = PNOJUMP;
    psliding = psemisliding = false;
    pfrozentime = 0;
    keyprocstate = 0;         // KPROC_IDLE
    pjustjumped = pjustfell = true;
    pfireframetimer = 0;
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
  	mLevelTrigger = LVLTRIG_NONE;
  	checkObjSolid();
}










// upon starting to walk, keen will quickly increase to
// PFASTINCMAXSPEED. keen can, at a slower rate,
// reach up to PMAXSPEED (increased every walk anim frame)
#define PFASTINCRATE     4        // accel delay rate up to PFASTINCMAXSPEED
#define PFASTINCRATE_POGO  2      // rate when pogo stick is out

// handles walking. the walking animation is handled by gamepdo_walkinganim()
void CPlayer::Walking()
{
	int cur_pfastincrate;

	if(pfiring)
		return;

    // this prevents a "slipping" effect if you jump, say, right, then
    // start walking left just as you hit the ground
    if (pjustjumped && ((xinertia > 0 && pDir.x == LEFT) ||
                        (xinertia < 0 && pDir.x == RIGHT)))
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
			pjustfell = 0;
		
		// if we fall onto a semislide tile with no inertia
		// don't move!.
		if (pjustfell && psemisliding)
		{
			if (pDir.x==RIGHT)
				pjustfell = blockedr ? 0 : pDir.x;
			else
				pjustfell = blockedl ? 0 : pDir.x;
		}
    }
	
    // test if we're trying to walk
    if ((psemisliding && xinertia!=0)
            || (((playcontrol[PA_X] < 0) ||
                 (playcontrol[PA_X] > 0) ||
                 (( (playcontrol[PA_Y] < 0) ||
                    (playcontrol[PA_Y] > 0))
               && m_playingmode == WORLDMAP )) && !inhibitwalking))
    {
		// we just started walking or we changed directions suddenly?
		if (pwalking == 0 || ((lastPDir.x==RIGHT && pDir.x==LEFT)||(lastPDir.x==LEFT && pDir.x==RIGHT)))
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
			
			lastPDir = pDir;
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
	const int pmaxspeed = PhysicsSettings.player.max_x_speed;

	// when sliding on ice force maximum speed
	if (psliding)
	{
		if (pjumping != PPREPAREJUMP &&
			pjumping != PPREPAREPOGO)
		{
			// reset walk frame because we have no walk animation while on ice
			pwalkframe = 0;
			// keep player sliding at maximum speed
			if (pDir.x==RIGHT)
				xinertia = pmaxspeed;
			else if (pDir.x==LEFT)
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

    if(!ppogostick && !pfalling)
    {

        if (playcontrol[PA_X] > 0)
        {   // RIGHT key down
            // quickly reach PLAYER_FASTINCMAXSPEED

            if( xinertia<0 )
                xinertia = 0;

            if (pwalkincreasetimer>=cur_pfastincrate)
            {
                if(pfalling) xinertia+=(1<<2);
                else xinertia += (1<<3);
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
            if (!pwalkanimtimer && xinertia < pmaxspeed)
            {
                xinertia += (1<<4);
                if(xinertia > pmaxspeed)
                    xinertia = pmaxspeed;
            }
        }
        else if (playcontrol[PA_X] < 0)
        { 	// LEFT key down
            // quickly reach PFASTINCMAXSPEED
            if( xinertia>0 )
                xinertia = 0;

            if (pwalkincreasetimer>=cur_pfastincrate)
            {
                if(pfalling) xinertia-=(1<<2);
                else xinertia -= (1<<3);
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
            if (!pwalkanimtimer && xinertia > -pmaxspeed)
            {
                xinertia -= (1<<4);
                if(xinertia < -pmaxspeed)
                    xinertia = -pmaxspeed;
            }
        }
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

// rate at which player walking animation is shown
#define PWALKANIMRATE             8

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
					playSound((m_playingmode == WORLDMAP) ? SOUND_KEEN_WALK : SOUND_KEEN_WALK2 );
					
					if( m_playingmode != WORLDMAP )
					{
						// Play walking sound in level.
						// It sounds very similar to the one of the map, only similar
						playSound( SOUND_KEEN_WALK2 );

						// It is not bumping a head, but walking in some direction and being blocked.
						// The same sound for that is played
						if(blockedr || blockedl)
							playSound( SOUND_KEEN_BUMPHEAD );
					}
					else
					{
						// Play walking sound on map
						playSound(SOUND_KEEN_WALK);

						// Check if the player walking against walls (solid tiles)
						bool obs=false;
						obs |= (blockedu && pDir.y == UP);
						obs |= (blockedd && pDir.y == DOWN);
						obs |= (blockedl && pDir.x == LEFT);
						obs |= (blockedr && pDir.x == RIGHT);
						
						if (obs)
							playSound(SOUND_KEEN_BUMPHEAD);
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

void CPlayer::pumpEvent(const CEvent *evPtr)
{
    CSpriteObject::pumpEvent(evPtr);
    mpCamera->pumpEvent(evPtr);
}


void CPlayer::processEvents()
{
    CSpriteObject::processEvents();
    mpCamera->processEvents();
}

/*
 * This function calls the camera object of the player
 */
void CPlayer::processCamera()
{
    mpCamera->process();
}



// rates at which player slows down while he is walking suddenly player does not
// move him anymore
#define PFRICTION_RATE_INAIR      2
#define PFRICTION_RATE_ONGROUND   14
#define PFRICTION_RATE_WM         32


// handles inertia and friction for the X direction
// (this is where the xinertia is actually applied to playx)
void CPlayer::InertiaAndFriction_X()
{
	int friction_rate;
	treshold = 0;
	CPhysicsSettings &PhysicsSettings = g_pBehaviorEngine->getPhysicsSettings();


	// Calculate Threshold of your analog device for walking animation speed!
	if(!pfrozentime && !pfiring)
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

    const int ep = g_pBehaviorEngine->getEpisode();
    const int level = mp_Map->getLevel();

    if(level == 80) // We are on World map
		verifySolidLevels();
	
	// apply xinertia
    // (unless we're about to make a pogo or jump)
	if ( pjumping != PPREPAREPOGO && pjumping != PPREPAREJUMP)
	{
		int dx = xinertia;
		// check first if the player is not blocked
		if( (!blockedr and dx>0 ) or 
		    (!blockedl and dx<0 ) )
			moveXDir(dx);
		else
			xinertia = 0;
	}
	
	// if we stopped walking (i.e. left or right not held down) apply friction
	// there's no friction if we're semisliding

	if ( ( playcontrol[PA_X] == 0 || pfiring ) && !psemisliding)
	{
		// determine friction rate--different rates for on ground and in air
		if (m_playingmode == WORLDMAP)
		{
			friction_rate = PFRICTION_RATE_WM;
		}
		else
		{
			friction_rate = (!pfalling && !pjumping) ? PFRICTION_RATE_ONGROUND : PFRICTION_RATE_INAIR;
		}
		
		// and apply friction to xinertia
		// when pogoing apply friction till we get down to PFASTINCMAXSPEED
		// then stop the friction
		if (!ppogostick || (xinertia >  PhysicsSettings.player.max_x_speed) ||
						   (xinertia < -PhysicsSettings.player.max_x_speed) )
		{
            if (!pfrozentime || ep != 1)
			{   // disable friction while frozen
				// here the wall animation must be applied!
				if(!psliding)
				{
					if( (xinertia>0 && blockedr) ||
						(xinertia<0 && blockedl) )
					{
						friction_rate = 3*friction_rate;
					}

                    if(pfalling)
                        friction_rate /= 2;

					decreaseXInertia(friction_rate);
				}
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

	if(playpushed_decreasetimer>0)
	    playpushed_decreasetimer--;
	else if(playpushed_decreasetimer<0)
	    playpushed_decreasetimer++;

	
	if(gInput.getHoldedCommand(m_index, IC_LEFT) && playpushed_decreasetimer<=0 )
	{
	    const int newval = gInput.isJoystickAssgmnt(m_index, IC_LEFT) && gInput.isAnalog(m_index) ? -gInput.getJoyValue(m_index, IC_LEFT) : 100;
	    playcontrol[PA_X] -= newval;
	}
	else if(gInput.getHoldedCommand(m_index, IC_RIGHT) && playpushed_decreasetimer>=0 )
	{
	    const int newval = gInput.isJoystickAssgmnt(m_index, IC_RIGHT) && gInput.isAnalog(m_index) ? gInput.getJoyValue(m_index, IC_RIGHT) : 100;
	    playcontrol[PA_X] += newval;
	}
	
	if(gInput.getHoldedCommand(m_index, IC_DOWN) )
	{
	    const int newval = gInput.isJoystickAssgmnt(m_index, IC_DOWN) && gInput.isAnalog(m_index) ? gInput.getJoyValue(m_index, IC_DOWN) : 100;
	    playcontrol[PA_Y] += newval;
	}
	else if(gInput.getHoldedCommand(m_index, IC_UP))
	{
	    const int newval = gInput.isJoystickAssgmnt(m_index, IC_UP) && gInput.isAnalog(m_index) ? -gInput.getJoyValue(m_index, IC_UP) : 100;
	    playcontrol[PA_Y] -= newval;
	}	    
	
	if(gInput.getHoldedCommand(m_index, IC_UPPERLEFT))
	{
	    playcontrol[PA_X] -= 100;
	    playcontrol[PA_Y] -= 100;
	}
	else if(gInput.getHoldedCommand(m_index, IC_UPPERRIGHT))
	{
	    playcontrol[PA_X] += 100;
	    playcontrol[PA_Y] -= 100;
	}
	else if(gInput.getHoldedCommand(m_index, IC_LOWERLEFT))
	{
	    playcontrol[PA_X] -= 100;
	    playcontrol[PA_Y] += 100;
	}
	else if(gInput.getHoldedCommand(m_index, IC_LOWERRIGHT))
	{
	    playcontrol[PA_X] += 100;
	    playcontrol[PA_Y] += 100;
	}
	

	if(!pfiring)
	{
		if(gInput.getHoldedCommand(m_index, IC_JUMP))
			playcontrol[PA_JUMP]++;
		else
			playcontrol[PA_JUMP] = 0;

	}
	else
		playcontrol[PA_JUMP]   = gInput.getHoldedCommand(m_index, IC_JUMP)   ? 1 : 0;

	playcontrol[PA_POGO]   = gInput.getHoldedCommand(m_index, IC_POGO)   ? 1 : 0;
	
	// The possibility to charge jumps. This is mainly used for the pogo.
	if( playcontrol[PA_JUMP] > 50) playcontrol[PA_JUMP] = 50;
	
	// Two button firing process
	if(gInput.getTwoButtonFiring(m_index))
	{
		if(playcontrol[PA_JUMP] && playcontrol[PA_POGO])
		{
			playcontrol[PA_FIRE] = 1;
			playcontrol[PA_JUMP] = 0;
			playcontrol[PA_POGO] = 0;
		}
		else if(playcontrol[PA_FIRE])
		{
			playcontrol[PA_FIRE] = 0;
			playcontrol[PA_JUMP] = 0;
			playcontrol[PA_POGO] = 0;
			gInput.flushCommand(IC_JUMP);
			gInput.flushCommand(IC_FIRE);
			gInput.flushCommand(IC_POGO);
		}

	}
	else
		playcontrol[PA_FIRE]   = gInput.getHoldedCommand(m_index, IC_FIRE)   ? 1 : 0;
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
		pDir.x = pShowDir.x = RIGHT;
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
	if( gInput.getPressedCommand(m_index, IC_STATUS) && !level_done )
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



bool CPlayer::checkMapBoundaryR(const int x2)
{
	if( solid && x2 >= (int)((mp_Map->m_width-2)<<CSF) )
		return true;

	return false;
}

bool CPlayer::checkMapBoundaryL(const int x1)
{
	if( solid && x1 <= (2<<CSF) )
		return true;

	return false;
}


bool CPlayer::checkMapBoundaryU(const int y1)
{
	if( y1 <= (2<<CSF) )
		return true;

	return false;
}


bool CPlayer::checkObjSolid()
{
	if(pSupportedbyobject)
	{
		pfalling = false;
		if(pjumping == PJUMPLAND)
		    pjumping = PNOJUMP;
		
		int dy = pSupportedbyobject->getYPosition() - getYDownPos()+3;
		if(pjumping == PNOJUMP)
		{
		    moveYDir(dy);
		}
		blockedd = true;		
	}
	
	return true;
}

// Draws the Status screen and return false, when it's still open.
bool CPlayer::drawStatusScreen()
{
	if(m_showStatusScreen)
	{
        if(mpStatusScr)
            mpStatusScr->draw();
		
		return false;
	}
    else
    {
        return true;
    }
}
