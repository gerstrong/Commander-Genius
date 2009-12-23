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
#include <stdlib.h>

///
// Initialization Part
///
CPlayer::CPlayer(char &Episode, short &Level, char &Difficulty,
				 short &player_index, bool *mp_level_completed, stOption *mp_option,
				 std::vector<CObject> &m_Object) :
m_episode(Episode),
m_level(Level),
m_difficulty(Difficulty),
m_player_number(player_index),
mp_object(&m_Object),
mp_levels_completed(mp_level_completed),
mp_map(NULL),
mp_option(mp_option),
mp_StatusScr(NULL)
{
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
	playframe = PMAPDOWNFRAME;

	goto_x = x = 0;
	goto_y = y = 0;
	pfallspeed = 0,
	pdir = pshowdir = DOWN;
	inhibitfall = hideplayer = false;
  	pwalkframe = pwalkframea = 0;
   	m_player_number = 0;
    dpadcount = 0;
    hideplayer = false;
    mounted = false;
    ppogostick = false;
    pogofirsttime = false;
    plastfire = false;
    psliding = psemisliding = false;
    blockedu = blockedd = blockedl = blockedr = false;
	
    pjumping = pjumptime = 0;
	
    pjustfell = plastfalling = pfalling = false;
    pwalking = playspeed = 0;
    pinertia_x = pinertia_y = 0;
    playpushed_x = 0;
    plastfire = pfiring = false;
    pwalkanimtimer = 0;
    inhibitfall = false;
    playerbaseframe = 0;
    mapplayx = mapplayy = 0;
    level_done = LEVEL_NOT_DONE;
	
    pfrozentime = 0;
    ankhtime = 0;
    keyprocstate = 0;         // KPROC_IDLE
    pjustjumped = pjustfell = true;
    pfireframetimer = 0;
    psupportingtile = psupportingobject = lastsupportingobject = 0;
	
    m_cheats_enabled = false;
    m_showStatusScreen = false;
    lastpogo = false;
	
    ppogostick=false;
    level_done_timer = pfriction_timer_x = 0;
  	dpadcount = dpadlastcount = 0;
  	beingteleported = false;
  	object_chosen = false;
	
  	exitXpos = 0;
  	m_Level_Trigger = LVLTRIG_NONE;
	
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

bool CPlayer::scrollTriggers()
{
	int px, py;
	bool scrollchanged=false;
	unsigned int scroll_x, scroll_y;

	scroll_x = mp_map->m_scrollx;
	scroll_y = mp_map->m_scrolly;

	if (pdie) return scrollchanged;

	px = (x>>STC)-scroll_x;
	py = (y>>STC)-scroll_y;

	// left-right scrolling
	if(px > SCROLLTRIGGERRIGHT && scroll_x < mp_map->m_maxscrollx)
	{
		do{
			scroll_x = mp_map->m_scrollx;
			px = (x>>STC)-scroll_x;
			mp_map->scrollRight();
		}while(px > 226 && scroll_x < mp_map->m_maxscrollx);
		scrollchanged = true;
	}
	else if(px < SCROLLTRIGGERLEFT && scroll_x > 32)
	{
		do{
			scroll_x = mp_map->m_scrollx;
			px = (x>>STC)-scroll_x;
			mp_map->scrollLeft();
		}while(px < 80 && scroll_x > 32);
		scrollchanged = true;
	}

	// up-down scrolling
	if (py > SCROLLTRIGGERDOWN && scroll_y < mp_map->m_maxscrolly)
	{
		do{
			scroll_y = mp_map->m_scrolly;
			py = (y>>STC)-scroll_y;
			mp_map->scrollDown();
		}while(py > 150 && scroll_y < mp_map->m_maxscrolly);
		scrollchanged = true;
	}
	else if ( py < SCROLLTRIGGERUP && scroll_y > 32  )
	{
		do{
			scroll_y = mp_map->m_scrolly;
			py = (y>>STC)-scroll_y;
			mp_map->scrollUp();
		}while(py < 50 && scroll_y > 32);
		scrollchanged = true;
	}

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
				pinertia_x = 0;
		return;
    }
	
    // this prevents a "slipping" effect if you jump, say, right, then
    // start walking left just as you hit the ground
    if (pjustjumped && ((pinertia_x > 0 && pdir==LEFT) ||\
                        (pinertia_x < 0 && pdir==RIGHT)))\
    {
    	if(!ppogostick)
			pinertia_x = 0;
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
    if ((psemisliding && pinertia_x!=0) || (((playcontrol[PA_X] < 0) || (playcontrol[PA_X] > 0) || (( (playcontrol[PA_Y] < 0) || (playcontrol[PA_Y] > 0)) && m_playingmode == WORLDMAP )) && !inhibitwalking))
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
					pinertia_x /= 2;
				pinertia_y = 0;
			}
			
			lastpdir = pdir;
			pwalking = 1;
		}
    }
    else
    {   // end "d-pad down and bit sliding"
    	if(pinertia_x != 0)
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
			{
				pinertia_x = pmaxspeed;
			}
			else if (pdir==LEFT)
			{
				pinertia_x = -pmaxspeed;
			}
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
		if (pwalkincreasetimer>=cur_pfastincrate && pinertia_x<mp_PhysicsSettings->player.max_x_speed)
		{
			if(pfalling) pinertia_x+=(1<<2);
			else pinertia_x+=(1<<4);
			pwalkincreasetimer=0;
		}
		else	pwalkincreasetimer++;
		
		// prevent sliding on map
		if ( m_playingmode == WORLDMAP )
		{
			if(pinertia_x < 0)  pinertia_x = 0;
			else	pinertia_x = playcontrol[PA_X]*mp_PhysicsSettings->player.max_x_speed/150;
		}
		
		// increase up to max speed every time frame is changed
		//if (!pwalkanimtimer && pinertia_x < pmaxspeed)	pinertia_x+=(1<<4);
	}
	else if (playcontrol[PA_X] < 0 && !ppogostick && !playpushed_x)
	{ // LEFT key down
		// quickly reach PFASTINCMAXSPEED
		if (pwalkincreasetimer>=cur_pfastincrate && pinertia_x<-mp_PhysicsSettings->player.max_x_speed)
		{
			if(pfalling) pinertia_x-=(1<<2);
			else pinertia_x-=(1<<4);
			pwalkincreasetimer=0;
		}
		else	pwalkincreasetimer++;
		
		// prevent sliding on map
		if ( m_playingmode == WORLDMAP )
		{
			if(pinertia_x > 0)  pinertia_x = 0;
			else	pinertia_x = playcontrol[PA_X]*mp_PhysicsSettings->player.max_x_speed/150;
		}
		
		// decrease down to max speed every time frame is changed
		if (!pwalkanimtimer && pinertia_x > -pmaxspeed)	pinertia_x-=(1<<4);
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
		
    	if(!psemisliding)
    	{
    		walkanimrate = 101*PWALKANIMRATE/(treshold+1);
        	if(walkanimrate > 150)
				walkanimrate = 150;
    	}
    	else
    		walkanimrate = PWALKANIMRATE;
		
    	// ** do walk animation **
        if (pwalkanimtimer > walkanimrate)
        { // time to change walking frame
			// make walk noise
			if ( (!pjumping && !pfalling) || m_playingmode == WORLDMAP )
			{
				if (!pfrozentime && pwalking)
				{
					if (pwalkframea&1)
						g_pSound->playStereofromCoord(SOUND_KEEN_WALK, PLAY_NOW, mp_object->at(m_player_number).scrx);
					else
						g_pSound->playStereofromCoord(SOUND_KEEN_WALK2, PLAY_NOW, mp_object->at(m_player_number).scrx);
					
					if( m_playingmode != WORLDMAP && (blockedr || blockedl) )
					{
						g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, mp_object->at(m_player_number).scrx);
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
							g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, mp_object->at(m_player_number).scrx);
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
	
	if(pinertia_x > pmaxspeed)
		pinertia_x = pmaxspeed;
	if(pinertia_x < -pmaxspeed)
		pinertia_x = -pmaxspeed;
	
	// apply pinertia_x and playpushed_x inertia
	// (unless we're about to make a pogo jump)
	if ( pjumping != PPREPAREPOGO && pjumping != PPREPAREJUMP)
	{
		int dx=pinertia_x + playpushed_x;
		// check first if the player is not blocked
		if( (!blockedr and dx>0) or (!blockedl and dx<0) )
			goto_x += dx;
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
		
		// and apply friction to pinertia_x
		// when pogoing apply friction till we get down to PFASTINCMAXSPEED
		// then stop the friction
		if (!ppogostick || (pinertia_x >  mp_PhysicsSettings->player.max_x_speed) ||
						   (pinertia_x < -mp_PhysicsSettings->player.max_x_speed) )
		{
	        if (pfriction_timer_x > friction_rate)
	        {
				if (!pfrozentime || m_episode!=1)
				{   // disable friction while frozen
					// here the wall animation must be applied!
					if (pinertia_x < 0)
					{
						pinertia_x+=(1<<5);
						if (pinertia_x > 0) pinertia_x = 0;
					}
					else if (pinertia_x > 0)
					{
						pinertia_x-=(1<<5);
						if (pinertia_x < 0) pinertia_x = 0;
					}
				}
				pfriction_timer_x = 0;
	        }
	        else pfriction_timer_x++;
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
	{
		pinertia_y = 0;
	}
	else if (pinertia_y < 0 && blockedu)
	{
		pinertia_y = 0;
	}
	
	// apply pinertia_y
	goto_y += pinertia_y;
	
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
	
	if(g_pInput->getHoldedCommand(m_player_number, IC_LEFT))
		playcontrol[PA_X] -= 100;
	if(g_pInput->getHoldedCommand(m_player_number, IC_RIGHT))
		playcontrol[PA_X] += 100;
	
	if(g_pInput->getHoldedCommand(m_player_number, IC_UP))
		playcontrol[PA_Y] -= 100;
	if(g_pInput->getHoldedCommand(m_player_number, IC_DOWN))
		playcontrol[PA_Y] += 100;
	
	if(g_pInput->getHoldedCommand(m_player_number, IC_JUMP))
		playcontrol[PA_JUMP]++;
	else
		playcontrol[PA_JUMP] = 0;
	
	playcontrol[PA_POGO]   = g_pInput->getHoldedCommand(m_player_number, IC_POGO)   ? 1 : 0;
	playcontrol[PA_FIRE]   = g_pInput->getHoldedCommand(m_player_number, IC_FIRE)   ? 1 : 0;
	playcontrol[PA_STATUS] = g_pInput->getHoldedCommand(m_player_number, IC_STATUS) ? 1 : 0;
	
	// The possibility to charge jumps. This is mainly used for the pogo.
	if( playcontrol[PA_JUMP] > 50) playcontrol[PA_JUMP] = 50;
	
	// are we doing the keen-walking-through-exit door animation?
	if (level_done)
	{
		// don't let player control keen
		
		memset(playcontrol,0,PA_MAX_ACTIONS);
		
		inhibitfall = true;
		if (level_done==LEVEL_DONE_WALK)
		{
	        // keep him going right
	        pdir = pshowdir = RIGHT;
	        // make keen walk slowly through the exit door
	        playcontrol[PA_X] = 80;
	        if (pinertia_x > PMAXEXITDOORSPEED)
	        {
				pinertia_x = PMAXEXITDOORSPEED;
	        }
		}
		else
		{
	        // he's all the way through the door; hold him still
	        pinertia_x = 0;
	        pwalking = false;
		}
		
		if (level_done_timer > LEVEL_DONE_TOTAL_WAIT_TIME)
		{
			// Now, that the level is complete, sprite can be shown again, and now goto map!
			int width = w>>(CSF-4);
			g_pGfxEngine->Sprite[playerbaseframe+0]->setWidth(width);
			g_pGfxEngine->Sprite[playerbaseframe+1]->setWidth(width);
			g_pGfxEngine->Sprite[playerbaseframe+2]->setWidth(width);
			g_pGfxEngine->Sprite[playerbaseframe+3]->setWidth(width);
			level_done_timer = 0;
			level_done = LEVEL_COMPLETE;
		}
		else if (level_done_timer > LEVEL_DONE_STOPWALKING_TIME)
			level_done = LEVEL_DONE_WAIT;
		
		level_done_timer++;
		return;
	}
	
	if(g_pInput->getTwoButtonFiring(m_player_number))
	{
		if(playcontrol[PA_JUMP] && playcontrol[PA_POGO])
		{
			playcontrol[PA_FIRE] = 1;
			playcontrol[PA_JUMP] = 0;
			playcontrol[PA_POGO] = 0;
		}
	}
}

void CPlayer::StatusBox()
{
	if( g_pInput->getPressedCommand(IC_STATUS) )
		m_showStatusScreen=true; // PlayGame class detects that variable and launches drawStatusScreen()
}

void CPlayer::freeze()
{
	if ( godmode ) return;
	if ( ankhtime) return;
	// give the player a little "kick"
	//pjumptime = PJUMP_NORMALTIME_1;
	//pjumpupdecreaserate = PJUMP_UPDECREASERATE_1;
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

// Draws the Status screen and return false, when it's still open.
bool CPlayer::drawStatusScreen()
{
	if(m_showStatusScreen)
	{
		if(mp_StatusScr == NULL)
			mp_StatusScr = new CStatusScreen(m_episode, &inventory, mp_levels_completed, ankhtime, playerbaseframe);
		
		mp_StatusScr->draw();
		
		if(g_pInput->getPressedAnyCommand())
		{
			m_showStatusScreen = false;
			delete mp_StatusScr;
			mp_StatusScr = NULL;
		}
		return false;
	}
	else return true;
}

///
// Cleanup Part
///
CPlayer::~CPlayer() {
	// TODO Auto-generated destructor stub
}
