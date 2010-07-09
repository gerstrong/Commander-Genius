/*
 * CPlayerLevel.cpp
 *
 *  Created on: 07.10.2009
 *      Author: gerstrong
 */

#include "CPlayer.h"

#include "../engine/vorticon/ai/CSectorEffector.h"
#include "../engine/vorticon/ai/CRay.h"
#include "../engine/spritedefines.h"
#include "../keen.h"
#include "../sdl/sound/CSound.h"
#include "../sdl/CInput.h"
#include "../graphics/CGfxEngine.h"
#include "../hqp/CMusic.h"
#include "CPhysicsSettings.h"

#define PDIEFRAME             22

////
// Process the stuff of the player when playing in a normal level
void CPlayer::processInLevel(const bool &platextending)
{
    StatusBox();

    if(HealthPoints <= 0) kill();
	
    if (pdie) dieanim();
	else
	{
		inhibitwalking = false;
		inhibitfall = false;
		
		// when walking through the exit door don't show keen's sprite past
		// the door frame (so it looks like he walks "through" the door)
		if(!pfrozentime)
		{
			if (!level_done)
				ProcessInput();
			else
				ProcessExitLevel();
		}
		
		setDir();
		
		if(!level_done)
		{
			getgoodies();
			ankh(); // Normally can happen in Episode 3
			raygun();
			keencicle();
		}
		
		if(!pfrozentime)
		{
			if(!pjumping)
			{
				Walking();
				WalkingAnimation();
			}
			playpushed();
		}

		checkSolidDoors();

		InertiaAndFriction_X();
		
		TogglePogo_and_Switches(platextending);
		JumpAndPogo();

		// Check collision with other objects
		checkObjSolid();
		if(!inhibitfall) Playerfalling();
	}
}

void CPlayer::touchedExit(int mpx)
{
	exitXpos = (mpx+2)<<TILE_S;

	if (!pjumping && !pfalling  &&
		!ppogostick && level_done==LEVEL_NOT_DONE)
	{
		// don't allow player to walk through a door if he's standing
		// on an object such as a platform or an enemy
		if (supportedbyobject)	return;
		
		// TODO: in episode 2 on level 13 aka Rome Tantalus Ray, if
		// keen hasn't flipped the switch he shouldn't be able to
		// leave the level, this is a bug from the original game
		// that needs fixing, hopefully we will fix all of those
		
		// if player has ankh shut it off
		ankhtime = 0;
		
		ppogostick = false;
		
		g_pMusicPlayer->stop();
		g_pSound->playSound(SOUND_LEVEL_DONE, PLAY_NOW);
		level_done = LEVEL_DONE_WALK;
	}
}

void CPlayer::walkbehindexitdoor()
{
	int xb, diff, width;
	
    // don't draw keen as he walks through the door (past exitXpos)
    // X pixel position of right side of player
    xb = (getXRightPos())>>(STC);
    diff = (xb - exitXpos) + 6;        // dist between keen and door
    if (diff >= 0)                             // past exitXpos?
    {
        width = (w>>(STC)) - diff;    // get new width of sprite
        if (width < 0) width = 0;               // don't set to negative

        int frame = playerbaseframe;
		if(m_episode == 3) frame++;

        // set new width of all player walk frames
        g_pGfxEngine->getSprite(frame+0).setWidth(width);
        g_pGfxEngine->getSprite(frame+1).setWidth(width);
		g_pGfxEngine->getSprite(frame+2).setWidth(width);
		g_pGfxEngine->getSprite(frame+3).setWidth(width);
    }
}

void CPlayer::kill(bool force)
{
	if(!force) // force can happens for example, when player leaves the map to the most lower-side
	{
		if (godmode) return;
		if (ankhtime) return;
	}
	if (!pdie)
	{
		godmode = false;
		solid = false;
		pdie = PDIE_DYING;
		pdieframe = 0;
		pdietimer = 0;
		pdietillfly = DIE_TILL_FLY_TIME;
		pdie_xvect = rand()%(DIE_MAX_XVECT*2);
		pdie_xvect -= DIE_MAX_XVECT;
		inventory.lives--;
		SelectFrame();
		g_pSound->playSound(SOUND_KEEN_DIE, PLAY_NOW);
	}
}

void CPlayer::dieanim() // Bad word for that. It's the entire die code
{
	if (!pdie) return;                // should never happen...
	if (pdie==PDIE_DEAD) return;      // if true animation is over
	if (pdie==PDIE_FELLOFFMAP)
	{
		// wait for falling sound to complete, then kill the player
		if (!g_pSound->isPlaying(SOUND_KEEN_FALL))
		{
			pdie = 0;
			kill();
			// TODO: Not sure, what must go here! Check out!
		}
		else return;
	}
	
	// peridocally toggle dying animation frame
	if (pdietimer > DIE_ANIM_RATE)
	{
		pdieframe = 1 - pdieframe;
		pdietimer = 0;
	}
	else pdietimer++;
	
	// is it time to start flying off the screen?
	if (!pdietillfly)
	{  // time to fly off the screen
		if (((getYPosition()>>(CSF-4))+96 > mp_Map->m_scrolly) && (getYPosition()>(16<<(CSF-4))))
		{  // player has not reached top of screen
			// make player fly up
			moveUp(-PDIE_RISE_SPEED);
			if (getXPosition() > (4<<CSF))
			{
				moveXDir(pdie_xvect);
			}
		}
		else
		{  // reached top of screen. he's done.
			pdie = PDIE_DEAD;
		}
	}
	else
	{  // not yet time to fly off screen, decrement timer
		pdietillfly--;
	}  // end "time to fly"
}

void CPlayer::keencicle()
{
	// keencicle code (when keen gets hit by an ice chunk)
	if(pfrozentime)
	{
		if (pfrozentime > PFROZEN_THAW)
		{
			if (pfrozenanimtimer > PFROZENANIMTIME)
			{
				if (pfrozenframe) pfrozenframe=0; else pfrozenframe=1;
				pfrozenanimtimer = 0;
			}
			else pfrozenanimtimer++;
		}
		else
		{ // thawing out, show the thaw frame
			if (m_episode==3)
				pfrozenframe = 2;
			else
				pfrozenframe = 3;
		}
		
		pfrozentime--;
		inhibitwalking = true;
	}
}

// if player not sliding and not jumping, allow
// them to change their direction. if jumping,
// we can change direction but it will not be shown
// in the frame.
void CPlayer::setDir()
{
	if (pfrozentime) return;
	// can't change direction on ice,
	// UNLESS we're stuck up against a wall
	if (psliding && pjumping < PJUMPED)
	{
		bool stuck = false;
		if (pshowdir == LEFT && blockedl) stuck = true;
		if (pshowdir == RIGHT && blockedr) stuck = true;
		if (stuck)
		{
			// jumped off an ice block into a wall?
			if (pjumping || pfalling)
			{
				psliding = false;
			}
		}
		else
		{
			// since we're not stuck up against a wall, we can't change direction
			return;
		}
	}
	
	if (!pjumping && !pfiring)
	{
		if (playcontrol[PA_X] < 0) { pdir = pshowdir = LEFT; }
		if (playcontrol[PA_X] > 0) { pdir = pshowdir = RIGHT; }
	}
	else
	{
		if (playcontrol[PA_X] < 0) { pdir = pshowdir = LEFT;  }
		if (playcontrol[PA_X] > 0) { pdir = pshowdir = RIGHT;  }
	}
}

// handle playpushed_x: for yorps/scrubs/etc pushing keen
void CPlayer::playpushed()
{
    if ( g_pInput->getHoldedKey(KTAB) ) return;
	
    // if we're being pushed...
    if (playpushed_x != 0)
    {
		// do friction on push force...
		if (playpushed_decreasetimer>PLAYPUSH_DECREASERATE)
		{
			// push playpushed_x towards zero
			if (playpushed_x < 0)
			{
				playpushed_x+= 4;
				if(playpushed_x > 0) playpushed_x=0;
			}
			else
			{
				playpushed_x-= 4;
				if(playpushed_x < 0) playpushed_x=0;
			}
			playpushed_decreasetimer = 0;
		}
		else playpushed_decreasetimer++;
		
		// if we run up against a wall all push inertia stops
		if (playpushed_x > 0 && blockedr) playpushed_x = 0;
		if (playpushed_x < 0 && blockedl) playpushed_x = 0;

		if( (!(playcontrol[PA_X]>0) && playpushed_x > 0) or
			(!(playcontrol[PA_X]<0) && playpushed_x < 0) )
			xinertia = 0;
    }
}

// allow Keen to toggle the pogo stick and hit switches
void CPlayer::TogglePogo_and_Switches(const bool &platextending)
{
	int i;
	int mx, my;
	Uint16 t;
	
	// detect if KPOGO key only pressed
	if ( playcontrol[PA_POGO] && !pfrozentime && !lastpogo )
	{
		// if we are standing near a switch hit the switch instead
		mx = (getXMidPos())>>CSF;
		
		for(i=h;i>=0;i-=8)
		{
			my = (getYPosition()+i)>>CSF;
			
			t = mp_Map->at(mx, my);
			
			// check for extending-platform switch
			if (t==TILE_SWITCH_UP || t==TILE_SWITCH_DOWN )
			{
				// Flip the switch!
				g_pSound->playStereofromCoord(SOUND_SWITCH_TOGGLE, PLAY_NOW, getXPosition()>>STC);
				if ( mp_Map->at(mx, my) == TILE_SWITCH_DOWN )
					mp_Map->changeTile(mx, my, TILE_SWITCH_UP);
				else
					mp_Map->changeTile(mx, my, TILE_SWITCH_DOWN);

				// figure out where the platform is supposed to extend at
				// (this is coded in the object layer...
				// high byte is the Y offset and the low byte is the X offset,
				// and it's relative to the position of the switch.)
				Uint16 bridge = mp_Map->getObjectat(mx, my);

				if (bridge==0) // Uh Oh! This means you have enabled a tantalus ray of the ship
				{
					m_Level_Trigger = LVLTRIG_TANTALUS_RAY;
				}
				else
				{
					if(!platextending)
					{
						m_Level_Trigger = LVLTRIG_BRIDGE;
						char pxoff = (bridge & 0x00ff);
						char pyoff = (bridge & 0xff00) >> 8;
						int platx = mx + pxoff;
						int platy = my + pyoff;

						// spawn a "sector effector" to extend/retract the platform
						//CSectorEffector *platobject = new CSectorEffector(mp_Map, mx<<CSF,my<<CSF,
							//							,*mp_object, SE_EXTEND_PLATFORM);
						//platobject->platx = platx;
						//platobject->platy = platy;
						//mp_object->push_back(platobject);
					}
				}

				if (ppogostick) ppogostick = false;
				break;
			}
			else if (t==TILE_LIGHTSWITCH)
			{ // lightswitch
				m_Level_Trigger = LVLTRIG_LIGHT;
				g_pSound->playStereofromCoord(SOUND_SWITCH_TOGGLE, PLAY_NOW, getXPosition()>>CSF);
				break;
			}
		}
		
		// toggle pogo stick
		if (inventory.HasPogo && m_Level_Trigger == LVLTRIG_NONE)
		{
			ppogostick ^= 1;
			pogofirsttime = true;
		}
		lastpogo = true;
	}

	if(!playcontrol[PA_POGO])	lastpogo = false;
}

void CPlayer::JumpAndPogo()
{
	CPhysicsSettings &PhysicsSettings = g_pBehaviorEngine->getPhysicsSettings();

	// handle the JUMP key, both for normal jumps and (high) pogo jumps
	if (!pjumping && !pfalling && !pfiring)
	{
		// give em the chance to jump
		if (playcontrol[PA_JUMP] && !ppogostick && !pfrozentime && !playpushed_x)
		{
			if(!psliding) xinertia = 0;
			pjumping = PPREPAREJUMP;
			pjumpframe = PPREPAREJUMPFRAME;
			pjumpanimtimer = 0;
			pwalking = false;
		}
		else if (ppogostick)
		{
			pjumping = PPREPAREPOGO;
			pjumpanimtimer = 0;
			pwalking = false;
		}
	}

    switch(pjumping)
    {
		case PPREPAREPOGO:
			if(ppogostick)
			{
				if (pjumpanimtimer>PPOGO_PREPARE_TIME)
				{
					pjumpupspeed = PhysicsSettings.player.maxjumpspeed;

					pjumpframe = PJUMP_PREPARE_LAST_FRAME;
					pjumping = PPOGOING;

					// continously bounce while pogo stick is out
					g_pSound->playStereofromCoord(SOUND_KEEN_JUMP, PLAY_NOW, scrx);

					// jump high if JUMP key down, else bounce low
					if (playcontrol[PA_JUMP])
					{
						if (!mp_option[OPT_SUPERPOGO].value)
						{  // normal high pogo jump
							if(playcontrol[PA_JUMP] > 12 || !mp_option[OPT_IMPPOGO].value)
							{
								if(!pogofirsttime)
								{
									int jump = PhysicsSettings.player.maxjumpspeed;
									int pogo = PhysicsSettings.player.maxpogospeed;
									pjumpupspeed = 3*(pogo-jump)*playcontrol[PA_JUMP] / 50 + jump;

									if(pjumpupspeed > PhysicsSettings.player.maxpogospeed)
										pjumpupspeed = PhysicsSettings.player.maxpogospeed;

									pogofirsttime = false;
								}
								else
									pjumpupspeed = PhysicsSettings.player.maxpogospeed;
							}
							else if(playcontrol[PA_JUMP] && pogofirsttime)
							{
								pjumpupspeed = PhysicsSettings.player.impossiblepogospeed;
							}
						}
						else
						{
							pjumpupspeed = PPOGOUP_SPEED_SUPER;
						}
					}
					else pogofirsttime = false;
					pjumpframe = PJUMP_PREPARE_LAST_FRAME;
					pjumping = PPOGOING;
				} else pjumpanimtimer++;
			}
			else pjumping = PNOJUMP;
			break;
		case PPREPAREJUMP:
			if (pjumpanimtimer > PJUMP_PREPARE_ANIM_RATE)
			{
				if (pjumpframe == PJUMP_PREPARE_LAST_FRAME || !playcontrol[PA_JUMP])
				{  	// time to start the jump
					// select a jump depending on how long keen was preparing
					pjumpupspeed = PhysicsSettings.player.maxjumpspeed;

					if(pjumpframe > PPREPAREJUMPFRAME+4)
						pjumpupspeed = PhysicsSettings.player.maxjumpspeed;
					else
						pjumpupspeed = (PhysicsSettings.player.maxjumpspeed*(pjumpframe-PPREPAREJUMPFRAME))/5;
					
					pjumpframe = PJUMP_PREPARE_LAST_FRAME;
					g_pSound->playStereofromCoord(SOUND_KEEN_JUMP, PLAY_NOW, scrx);
					pjumping = PJUMPUP;
					
					// make so if we're jumping left or right
					// the walk code will start at full speed
					pwalking = 1;
					pwalkanimtimer = 0;
					pwalkframe = 1;
					if ( psliding )
					{ // on ice, always jump direction facing
						if (pshowdir==LEFT) pdir=LEFT;
						else pdir=RIGHT;
					}
					else
						pjumpdir = UP;
					
					pwalkincreasetimer = 0;
					if(playcontrol[PA_X] < 0)	xinertia = -PhysicsSettings.player.jumpdecrease_x;
					if(playcontrol[PA_X] > 0)	xinertia = PhysicsSettings.player.jumpdecrease_x;
				}
				else pjumpframe++;

				pjumpanimtimer=0;
			} else pjumpanimtimer++;
			break;
        case PJUMPUP:
        case PPOGOING:
			// do the jump
        	// check for hitting a ceiling
        	if (blockedu)   // did we bonk something?
        	{  // immediatly abort the jump
        		if(!bumped)
        		{
        			g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, scrx);
            		bumped = true;
        		}
        		pjumpupspeed = 0;
        	}

			if (!pjumptime)
			{
				if (pjumpupspeed <= 0)
				{
					pjumpupspeed = 0;
					pfallspeed = PhysicsSettings.player.max_fallspeed;
					pjumping = PJUMPLAND;
				}
				else
				{
					pjumpupspeed-=pjumpupspeed_decrease;
				}
			}
			else pjumptime--;
			
			moveUp(pjumpupspeed);
			pjustjumped = true;

			break;
        case PJUMPLAND:
			// do the jump

			if (!pjumptime)
			{
				if (blockedd)
				{
					pjumpupspeed = 0;
					pjumping = PNOJUMP;
				}
				else
				{
					if(pjumpupspeed < PhysicsSettings.player.max_fallspeed)
						pjumpupspeed+=pjumpupspeed_decrease*2;
					else
					{
						pjumpupspeed=PhysicsSettings.player.max_fallspeed;
						pjumping = PNOJUMP;
					}
				}
			}
			else pjumptime--;

			moveDown(pjumpupspeed);
			pjustjumped = true;

			break;
        default:
        	break;
    }
	
	// Now check how much the direction of the player is given
    // (The real inertia in x-direction)
	if(pjumping)
	{
		if(!ppogostick)
		{
			if (playcontrol[PA_X] < 0)
				xinertia-=4;
			if (playcontrol[PA_X] > 0)
				xinertia+=4;
		}
		else if(ppogostick)
		{
			if (playcontrol[PA_X] < 0)
				xinertia -= (PhysicsSettings.player.pogoforce_x/10);
			if (playcontrol[PA_X] > 0)
				xinertia += (PhysicsSettings.player.pogoforce_x/10);
		}
	}
	else if(pfalling)
	{
		if(!ppogostick) {
			if(xinertia<0) xinertia+=2;
			if(xinertia>0) xinertia-=2;
		}

		if (playcontrol[PA_X] < 0)
			xinertia-=4;
		if (playcontrol[PA_X] > 0)
			xinertia+=4;
	}
	
    // If we are in Godmode, use the Pogo, and pressing the jump button, make the player fly
    if( godmode && ppogostick )
    {
    	if(playcontrol[PA_X] < 0) xinertia-=4;
    	if(playcontrol[PA_X] > 0) xinertia+=4;
    	if(g_pInput->getHoldedCommand(0, IC_JUMP) && !blockedu)
    		moveUp(PPOGOUP_SPEED);
    }
}

void CPlayer::Playerfalling()
{
char behaviour;
std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
CPhysicsSettings &PhysicsSettings = g_pBehaviorEngine->getPhysicsSettings();

	if (pfalling)
	{
		bumped = false;
		if (plastfalling == 0)
		{
			if (!pjustjumped)
				g_pSound->playStereofromCoord(SOUND_KEEN_FALL, PLAY_NOW, scrx);
		}
	}

	// save fall state so we can detect the high/low-going edges
	plastfalling = pfalling;

	if(pdie) return;

	// do not fall if we're jumping
	if (pjumping)
	{
		psemisliding = false;
		return;
	}

	// ** determine if player should fall (nothing solid is beneath him) **
	int xleft  = getXLeftPos();
	int ydown  = getYDownPos();

	behaviour = TileProperty[mp_Map->at(xleft>>CSF, ydown>>CSF)].behaviour;
	if( behaviour>=2 && behaviour<=5 )
		blockedu = true; // This workaround prevents the player from falling through doors.

	if(!blockedd && !pjumping && !supportedbyobject)
	{ // lower-left isn't solid, check right side
		pfalling = true;        // so fall.
		pjustfell = true;
	}
	else
	{
		pfalling = false;
		psliding = false;
		psemisliding = false;

		// Check if player is on ice
		if(!pjumping && !ppogostick)
		{
			int ice = TileProperty[mp_Map->at(getXLeftPos()>>CSF, (ydown+1)>>CSF)].bup;
			ice |= TileProperty[mp_Map->at(getXRightPos()>>CSF, (ydown+1)>>CSF)].bup;
			if(!blockedl && !blockedr)
			{
				if(ice == 2) psemisliding = true;
				else if(ice == 3) psliding = true;
			}
		}
	}

	// ** if the player should be falling, well what are we waiting for?
	//    make him fall! **
	if (pfalling)
	{  // nothing solid under player, let's make him fall
		psemisliding = 0;

		// gradually increase the fall speed up to maximum rate
		if (pfallspeed>PhysicsSettings.player.max_fallspeed)
			pfallspeed = PhysicsSettings.player.max_fallspeed;
		else if (pfallspeed<PhysicsSettings.player.max_fallspeed)
			pfallspeed += PhysicsSettings.player.fallspeed_decrease;

		// add current fall speed to player Y or make him fly in godmode with pogo
		if( !godmode || !ppogostick || !g_pInput->getHoldedCommand(IC_JUMP) )
			moveDown(pfallspeed);
	}
	else
	{  // not falling
		if (plastfalling)
		{  // just now stopped falling
			if (pdie != PDIE_FELLOFFMAP)
				g_pSound->stopSound(SOUND_KEEN_FALL);  // terminate fall noise
			// thud noise
			if (!ppogostick)
				g_pSound->playStereofromCoord(SOUND_KEEN_LAND, PLAY_NOW, scrx);
			// fix "sliding" effect when you fall, go one way, then
			// before you land turn around and as you hit the ground
			// you're starting to move the other direction
			// (set inertia to 0 if it's contrary to player's current dir)
		}
		pfallspeed = 0;
	}   // close "not falling"

	// ensure no sliding if we fall or jump off of ice
	if (pfalling || pjumping) psliding=0;

	// If he falls to the ground even being god, kill him
	if( (Uint16)getYDownPos() > ((mp_Map->m_height)<<CSF) )
		kill(true);
}

// wouldn't it be cool if keen had a raygun, and he could shoot things?
// oh wait, he does, and here's the code for it.
void CPlayer::raygun()
{
	bool canRefire;
	
	if (pfireframetimer) pfireframetimer--;
	
	// FIRE button down, and not keencicled?
	if ( playcontrol[PA_FIRE] && !pfrozentime && !(g_pInput->getHoldedKey(KC) && g_pInput->getHoldedKey(KT)) )
	{ // fire is pressed
		inhibitwalking = 1;            // prevent moving
		pfiring = true;  // flag that we're firing
		ppogostick = false;            // put away pogo stick if out
		
		// limit how quickly shots can be fired
		if ( !plastfire || mp_option[OPT_FULLYAUTOMATIC].value )
		{
			if (pfireframetimer < PFIRE_LIMIT_SHOT_FREQ_FA) canRefire = true;
			else canRefire = false;
		}
		else
		{
			if (pfireframetimer<PFIRE_LIMIT_SHOT_FREQ && !plastfire) canRefire = true;
			else canRefire = false;
		}
		
		if (canRefire)
		{
			// show raygun for a minimum time even if FIRE is immediatly released
			pfireframetimer = PFIRE_SHOWFRAME_TIME;
			
			// try to fire off a blast
			if (inventory.charges)
			{  // we have enough charges
				int xdir, ydir;
				inventory.charges--;
				pshowdir = pdir;
				
				g_pSound->playStereofromCoord(SOUND_KEEN_FIRE, PLAY_NOW, scrx);
				
				ydir = getYPosition()+(9<<STC);
				if (pdir==RIGHT) xdir = getXRightPos()+xinertia;
				else xdir = getXLeftPos()+xinertia-(16<<STC);

				CRay *rayobject = new CRay(mp_Map, xdir, ydir, pdir, OBJ_PLAYER, m_index);
				
				mp_object->push_back(rayobject);
			}
			else
			{ // uh oh, out of bullets
				// click!
				g_pSound->playStereofromCoord(SOUND_GUN_CLICK, PLAY_NOW, scrx);
				
			}  // end "do we have charges?"
		} // end "limit how quickly shots can be fired"
		plastfire = true;
	} // end "fire button down and not keencicled"
	else
	{ // FIRE button is NOT down
		// put away ray gun after it's shown for the minimum period of time
		if (!pfireframetimer)
		{  // ray gun shown for minimum time
			pfiring = 0;
		}
		else
		{  // minimum time not expired
			pfiring = 1;
			inhibitwalking = 1;
		}
		plastfire = false;
	}
}

// select the appropriate player frame based on what he's doing
void CPlayer::SelectFrame()
{
    sprite = playerbaseframe;      // basic standing
	
	if (m_episode==3) sprite++;

    // select the frame assuming he's pointing right. ep1 does not select
    // a walk frame while fading--this is for the bonus teleporter in L13.
    if (pdie) sprite += PDIEFRAME + pdieframe;
    else
    {
        if (pfrozentime) sprite = PFRAME_FROZEN + pfrozenframe;
        else if (pfiring) sprite += PFIREFRAME;
        else if (ppogostick) sprite += PFRAME_POGO + (pjumping==PPREPAREPOGO);
        else if (pjumping) sprite += pjumpframe;
        else if (pfalling) sprite += 13;
        else if (pwalking || playpushed_x || psemisliding) sprite += pwalkframe;
    }
	
    // if he's going left switch the frame selected above to the
    // appropriate one for the left direction
    if (pshowdir==LEFT && !pdie && !pfrozentime)
    {
		if (pfiring)
		{
			sprite++;
		}
		else if (ppogostick)
		{
			sprite+=2;
		}
		else if (pjumping || pfalling)
		{
			sprite+=6;
		}
		else
		{
			sprite+=4;
		}
    }
}

void CPlayer::ankh()
{
	//const unsigned int ANKH_FLICKER_DELAY = 3;
	//const unsigned int ANKH_SHIELD_FRAME = 61;

	/*if (!ankhtime) return;
	else if (ankhtime < ANKH_STAGE3_TIME)
		m_Ankhshield.ai.se.state = ANKH_STATE_FLICKERSLOW;
	else if (ankhtime < ANKH_STAGE2_TIME)
		m_Ankhshield.ai.se.state = ANKH_STATE_FLICKERFAST;
	else
		m_Ankhshield.ai.se.state = ANKH_STATE_NOFLICKER;

	ankhtime--;

	m_Ankhshield.moveToForce(getXPosition()-(8<<STC), getYPosition()-(8<<STC));

	if (m_Ankhshield.needinit)
	{
		m_Ankhshield.ai.se.frame = 0;
		m_Ankhshield.ai.se.timer = 0;
		m_Ankhshield.inhibitfall = 1;
		m_Ankhshield.canbezapped = 0;
		m_Ankhshield.needinit = 0;

		m_Ankhshield.ai.se.state = ANKH_STATE_NOFLICKER;
	}

	switch(m_Ankhshield.ai.se.state)
	{
	case ANKH_STATE_NOFLICKER:
		m_Ankhshield.sprite = ANKH_SHIELD_FRAME + (m_Ankhshield.ai.se.frame&1);
		break;
	case ANKH_STATE_FLICKERFAST:
		if (m_Ankhshield.ai.se.frame&1)
			m_Ankhshield.sprite = BLANKSPRITE;
		else
		{
			if (m_Ankhshield.ai.se.frame&2)
				m_Ankhshield.sprite = ANKH_SHIELD_FRAME+1;
			else
				m_Ankhshield.sprite = ANKH_SHIELD_FRAME;
		}
		break;
	case ANKH_STATE_FLICKERSLOW:
		if (m_Ankhshield.ai.se.frame>4)
			m_Ankhshield.sprite = BLANKSPRITE;
		else
			m_Ankhshield.sprite = ANKH_SHIELD_FRAME;
		break;
	}

	if (m_Ankhshield.ai.se.timer > ANKH_FLICKER_DELAY)
	{
		m_Ankhshield.ai.se.frame++;
		if (m_Ankhshield.ai.se.frame>8) m_Ankhshield.ai.se.frame = 0;
		m_Ankhshield.ai.se.timer = 0;
	}
	else m_Ankhshield.ai.se.timer++;*/

}

// yorp/scrub etc "bump".
// if solid = false, player can possibly force his way through.
// if solid = true, object acts like a solid "wall".
void CPlayer::bump( int pushamt, bool solid )
{
	if (pushamt > 0 && xinertia < pushamt)
	{
		pshowdir = pdir = RIGHT;
		xinertia = pushamt;
	}
	else if (xinertia > pushamt)
	{
		pshowdir = pdir = LEFT;
		xinertia = pushamt;
	}

	pwalking = true;

	if (solid)
	{
		playpushed_x = pushamt;
		if (pushamt > 0)
		{
			pshowdir = pdir = RIGHT;
			if (xinertia < 0)
				xinertia = 0;
		}
		else
		{
			pshowdir = pdir = LEFT;
			if (xinertia > 0)
				xinertia = 0;
		}
		playpushed_decreasetimer = 0;
	}
	
	if (!pjumping)
		pdir = pshowdir = (pushamt<0) ? LEFT : RIGHT;
}

void CPlayer::checkSolidDoors()
{
	int mx1 = getXLeftPos()>>CSF;
	int mxmid = getXMidPos()>>CSF;
	int mx2 = getXRightPos()>>CSF;
	int my1 = getYUpPos()>>CSF;
	int mymid = getYMidPos()>>CSF;
	int my2 = getYDownPos()>>CSF;
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	if( (TileProperty[mp_Map->at(mx1 ,mymid)].behaviour>1 &&
			TileProperty[mp_Map->at(mx1 ,mymid)].behaviour<6 ) ) {
		blockedl = true;	}

	if( (TileProperty[mp_Map->at(mx2 ,mymid)].behaviour>1 &&
			TileProperty[mp_Map->at(mx2 ,mymid)].behaviour<6 ) ) {
		blockedr = true;	}

	if( (TileProperty[mp_Map->at(mxmid ,my1)].behaviour>1 &&
			TileProperty[mp_Map->at(mxmid ,my1)].behaviour<6 ) ) {
		blockedd = true;	}

	if( (TileProperty[mp_Map->at(mxmid ,my2)].behaviour>1 &&
			TileProperty[mp_Map->at(mxmid ,my2)].behaviour<6 ) ) {
		blockedu = true;	}
}

int CPlayer::pollLevelTrigger()
{
	int trigger = m_Level_Trigger;
	m_Level_Trigger = LVLTRIG_NONE;
	return trigger;
}

void CPlayer::getShotByRay()
{
	if(!godmode)
		CObject::getShotByRay();
}

