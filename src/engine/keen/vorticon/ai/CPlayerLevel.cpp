/*
 * CPlayerLevel.cpp
 *
 *  Created on: 07.10.2009
 *      Author: gerstrong
 */

#include <base/CInput.h>

#include "CPlayer.h"

#include "CManglingMachine.h"
#include "CRay.h"
#include "CBridges.h"
#include "engine/core/spritedefines.h"
#include "sdl/audio/Audio.h"
#include "sdl/audio/music/CMusic.h"
#include "graphics/GsGraphics.h"
#include "engine/core/CPhysicsSettings.h"

#define PDIEFRAME             22

////
// Process the stuff of the player when playing in a normal level
void CPlayer::processInLevel()
{
    StatusBox();

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
			raygun();
			keencicle();
		}
		
		if(!pfrozentime)
		{
			if(!pjumping)
			{
				Walking();

                // if we bump against a wall all inertia stops
                if (xinertia > 0 && blockedr) xinertia = 0;
                if (xinertia < 0 && blockedl) xinertia = 0;

				WalkingAnimation();
			}
		}

		checkSolidDoors();

		InertiaAndFriction_X();

		if(!level_done)
		{
			TogglePogo_and_Switches();
			JumpAndPogo();
		}

		// Check collision with other objects
		performCollisions();
		checkObjSolid();
		if(!inhibitfall) Playerfalling();
	}

    if(pSupportedbyobject)
    	blockedd = true;
}

void CPlayer::touchedExit(int mpx)
{
	exitXpos = (mpx+2)<<TILE_S;

	if (!pjumping && !pfalling  && !pfiring &&
		!ppogostick && level_done==LEVEL_NOT_DONE)
	{
		// don't allow player to walk through a door if he's standing
		// on an object such as a platform or an enemy
		if (pSupportedbyobject)	return;
		
		ankhtime = 0;
		
		ppogostick = false;
		
		g_pMusicPlayer->stop();
		g_pSound->playSound(SOUND_LEVEL_DONE, PLAY_NOW);
		level_done = LEVEL_DONE_WALK;
		solid = false;
		inhibitfall = true;
	}
}

void CPlayer::walkbehindexitdoor()
{
	int xb, diff, width;

    const int ep = gpBehaviorEngine->getEpisode();

	
    // don't draw keen as he walks through the door (past exitXpos)
    // X pixel position of right side of player
    xb = (getXRightPos())>>(STC);
    diff = (xb - exitXpos) + 6;        // dist between keen and door
    if (diff > 0)                             // past exitXpos?
    {
        width = (w>>(STC)) - diff;    // get new width of sprite
        if (width < 0) width = 0;               // don't set to negative

        int frame = playerbaseframe;
        if(ep == 3) frame++;

        auto &sprVec = gGraphics.getSpriteVec(mSprVar);

        // set new width of all player walk frames
        sprVec[frame+0].setWidth(width);
        sprVec[frame+1].setWidth(width);
        sprVec[frame+2].setWidth(width);
        sprVec[frame+3].setWidth(width);
    }
}

void CPlayer::kill()
{
    kill(false);
}

void CPlayer::kill(bool force)
{
	if(!force) // force can happens for example, when player leaves the map to the most lower-side
	{
        if ( gpBehaviorEngine->mCheatmode.god ) return;
		if (ankhmode || ankhtime || level_done) return;
	}

	if (!pdie)
	{
		ankhmode = false;
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
		g_pMusicPlayer->stop();

		if(inventory.canlooseitem[0])	inventory.HasJoystick = false;
		if(inventory.canlooseitem[1])	inventory.HasBattery = false;
		if(inventory.canlooseitem[2])	inventory.HasVacuum = false;
		if(inventory.canlooseitem[3])	inventory.HasWiskey = false;

		for(size_t i = 0 ; i<4 ; i++)
			inventory.canlooseitem[i] = false;
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
		if (((getYPosition()>>STC)+128 > mp_Map->m_scrolly) && (getYPosition()>(48<<STC)))
		{   // player has not reached top of screen
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

            const int ep = gpBehaviorEngine->getEpisode();

            if (ep==3)
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
		if (pShowDir.x == LEFT && blockedl) stuck = true;
		if (pShowDir.x == RIGHT && blockedr) stuck = true;
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
		if (playcontrol[PA_X] < 0 && xinertia < 0) { pDir.x = pShowDir.x = LEFT; }
		if (playcontrol[PA_X] > 0 && xinertia > 0) { pDir.x = pShowDir.x = RIGHT; }
	}
	else
	{
		if (playcontrol[PA_X] < 0) { pDir.x = pShowDir.x = LEFT;  }
		if (playcontrol[PA_X] > 0) { pDir.x = pShowDir.x = RIGHT;  }
	}
}

#define TILE_SWITCH_UP             480
#define TILE_SWITCH_DOWN           493
#define TILE_LIGHTSWITCH           271


// allow Keen to toggle the pogo stick and hit switches
void CPlayer::TogglePogo_and_Switches()
{
	int i;
	unsigned int mx, my;
	Uint16 t;
	
	std::vector<CTileProperties> &TileProperty = gpBehaviorEngine->getTileProperties(1);

	// detect if KPOGO key only pressed
	if ( playcontrol[PA_POGO] && !pfrozentime && !lastpogo )
	{
		// if we are standing near a switch hit the switch instead
		mx = (getXMidPos())>>CSF;
		
		for( i=h ; i>=0 ; i-=8 )
		{
			my = (getYPosition()+(i<<STC))>>CSF;
			
			t = mp_Map->at(mx, my);

			// check for extending-platform switch
			if ( TileProperty[t].behaviour == 25  ||  TileProperty[t].behaviour == 26 || TileProperty[t].behaviour == 23 )
			{
				// Flip the switch!
				playSound(SOUND_SWITCH_TOGGLE);
				if ( TileProperty[t].behaviour == 26 && t == TILE_SWITCH_DOWN )
					mp_Map->changeTile(mx, my, TILE_SWITCH_UP);
				else if ( TileProperty[t].behaviour == 25 && t == TILE_SWITCH_UP )
					mp_Map->changeTile(mx, my, TILE_SWITCH_DOWN);

				// figure out where the platform is supposed to extend at
				// (this is coded in the object layer...
				// high byte is the Y offset and the low byte is the X offset,
				// and it's relative to the position of the switch.)
				Uint16 bridge = mp_Map->getObjectat(mx, my);

				if (bridge == 0) // Uh Oh! This means you have enabled a tantalus ray of the ship
				{ // lightswitch
					if(TileProperty[t].behaviour == 23)
						mLevelTrigger = LVLTRIG_LIGHT;
					else
						mLevelTrigger = LVLTRIG_TANTALUS_RAY;
				}
				else
				{
					mLevelTrigger = LVLTRIG_BRIDGE;

					const Sint8 pxoff = (bridge & 0x00ff);
					const Sint8 pyoff = (bridge & 0xff00) >> 8;
					const int platx = mx + pxoff;
					const int platy = my + pyoff;
					
					// spawn a "sector effector" to extend/retract the platform
					CBridges *platobject = new CBridges(mp_Map, mx<<CSF, my<<CSF,
							platx, platy);
                    spawnObj(platobject);
				}

				ppogostick = false;
				break;
			}
		}
		
		// toggle pogo stick
		if (inventory.HasPogo && mLevelTrigger == LVLTRIG_NONE)
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
	CPhysicsSettings &PhysicsSettings = gpBehaviorEngine->getPhysicsSettings();

	// handle the JUMP key, both for normal jumps and (high) pogo jumps
	if (!pjumping && !pfalling && !pfiring)
	{
		// give em the chance to jump
		if (playcontrol[PA_JUMP] && !ppogostick && !pfrozentime )
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
					playSound(SOUND_KEEN_JUMP);

					// jump high if JUMP key down, else bounce low
					if (playcontrol[PA_JUMP])
					{
						if( !gInput.SuperPogo(m_index) )
						{  // normal high pogo jump
							if( playcontrol[PA_JUMP] > 12 || !gInput.ImpossiblePogo(m_index) )
							{
								if(!pogofirsttime)
								{
									const int jump = PhysicsSettings.player.maxjumpspeed;
                                    const int pogo = (gpBehaviorEngine->mDifficulty >= NORMAL) ?
													(11*PhysicsSettings.player.maxpogospeed)/10
                                    				: PhysicsSettings.player.maxpogospeed;
                                   	pjumpupspeed = 3*(pogo-jump)*playcontrol[PA_JUMP] / 50 + jump;

                                    if(pjumpupspeed > pogo)
                                        pjumpupspeed = pogo;

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
					playSound(SOUND_KEEN_JUMP);
					pjumping = PJUMPUP;
					
					// make so if we're jumping left or right
					// the walk code will start at full speed
					pwalking = 1;
					pwalkanimtimer = 0;
					pwalkframe = 1;
					if ( psliding )
					{ // on ice, always jump direction facing
						if (pShowDir.x==LEFT) pDir.x=LEFT;
						else pDir.x=RIGHT;
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
        			playSound(SOUND_KEEN_BUMPHEAD);
            		bumped = true;
        		}
        		pjumpupspeed /= 2;
        	}

			if (!pjumptime)
			{
				if (pjumpupspeed <= 0)
				{
					pjumpupspeed = 0;
					pfallspeed = PhysicsSettings.max_fallspeed;
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
					if(pjumpupspeed < PhysicsSettings.max_fallspeed)
						pjumpupspeed+=pjumpupspeed_decrease*2;
					else
					{
						pjumpupspeed=PhysicsSettings.max_fallspeed;
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
					xinertia -= 3;
			if (playcontrol[PA_X] > 0)
					xinertia += 3;
		}
		else if(ppogostick)
		{
			if (playcontrol[PA_X] < 0)
				xinertia -= (PhysicsSettings.player.pogoforce_x/15);
			if (playcontrol[PA_X] > 0)
				xinertia += (PhysicsSettings.player.pogoforce_x/15);
		}
	}
	else if(pfalling)
	{
        boostInertia(1);
	}
	
    // If we are in Godmode, use the Pogo, and holding the jump button will make the player fly
    if( ppogostick && gpBehaviorEngine->mCheatmode.jump )
    {
    	if(playcontrol[PA_X] < 0) xinertia-=3;
    	if(playcontrol[PA_X] > 0) xinertia+=3;
    	if(playcontrol[PA_JUMP] && !blockedu)
    		moveUp(PPOGOUP_SPEED);
    }
}

void CPlayer::boostInertia(const int amt)
{
	CPhysicsSettings &PhysicsSettings = gpBehaviorEngine->getPhysicsSettings();
	const int pinitspeed = PhysicsSettings.player.max_x_speed/2;

	if (playcontrol[PA_X] < 0)
	{
		if(xinertia>-pinitspeed && xinertia < 0)
			xinertia = -pinitspeed;
		else
			xinertia -= amt;
	}
	if (playcontrol[PA_X] > 0)
	{
		if(xinertia<pinitspeed && xinertia > 0)
			xinertia = pinitspeed;
		else
			xinertia += amt;
	}
}


void CPlayer::Playerfalling()
{
	std::vector<CTileProperties> &TileProperty = gpBehaviorEngine->getTileProperties();
	CPhysicsSettings &PhysicsSettings = gpBehaviorEngine->getPhysicsSettings();

	if (pfalling)
	{
		bumped = false;
		if (plastfalling == 0)
		{
			if (!pjustjumped)
				playSound( SOUND_KEEN_FALL );
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

	auto behaviour = TileProperty[mp_Map->at(xleft>>CSF, ydown>>CSF)].behaviour;
	if( behaviour>=2 && behaviour<=5 )
		blockedu = true; // This workaround prevents the player from falling through doors.

	if(!blockedd && !pjumping && !pSupportedbyobject)
	{ // lower-left isn't solid, check right side
		pfalling = true;        // so fall.
		pjustfell = true;
	}
	else
	{
		pfalling = false;
		psliding = false;
		psemisliding = false;

		// Check if player is on iceplayer.
		if(!pjumping && !ppogostick)
		{
			int ice = TileProperty[mp_Map->at(getXLeftPos()>>CSF, (ydown+(1<<STC))>>CSF)].slippery;
			ice |= TileProperty[mp_Map->at(getXRightPos()>>CSF, (ydown+(1<<STC))>>CSF)].slippery;
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
		if(!pfallspeed)
			pfallspeed = (1<<STC); // Set initial fall position

		// gradually increase the fall speed up to maximum rate
		if (pfallspeed>PhysicsSettings.max_fallspeed)
			pfallspeed = PhysicsSettings.max_fallspeed;
		else if (pfallspeed<PhysicsSettings.max_fallspeed)
			pfallspeed += PhysicsSettings.fallspeed_increase;

		// add current fall speed to player Y or make him fly in godmode with pogo        
        if( !gpBehaviorEngine->mCheatmode.jump || !ppogostick || !gInput.getHoldedCommand(IC_JUMP) )
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
				playSound( SOUND_KEEN_LAND );
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
	if ( playcontrol[PA_FIRE] && !pfrozentime && !(gInput.getHoldedKey(KC) && gInput.getHoldedKey(KT)) )
	{ // fire is pressed
		inhibitwalking = 1;            // prevent moving
		pfiring = true;  // flag that we're firing
		ppogostick = false;            // put away pogo stick if out
		
		// limit how quickly shots can be fired
		if ( !plastfire || gInput.AutoGun(m_index) )
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

				// In case the player hasn't any direction assigned yet, because he can only shoot in those two directions
				if(pDir.x != LEFT) pDir.x = RIGHT;

				inventory.charges--;
				pShowDir = pDir;
				
				playSound( SOUND_KEEN_FIRE );
				
				ydir = getYPosition()+(9<<STC);
				if (pDir.x == RIGHT) xdir = getXRightPos()+xinertia;
				else xdir = getXLeftPos()+xinertia-(16<<STC);

                CRay *rayobject = new CRay(mp_Map, xdir, ydir,
                                           static_cast<direction_t>(pDir.x),
                                           CENTER,
                                           getSpriteVariantId(),
                                           OBJ_PLAYER, m_index);
				rayobject->setSpeed(124);
				gEventManager.add(new EventSpawnObject(rayobject) );
			}
			else
			{ // uh oh, out of bullets
				// click!
				playSound( SOUND_GUN_CLICK );
				
			}  // end "do we have charges?"
		} // end "limit how quickly shots can be fired"
		plastfire = true;
	} // end "fire button down and not keencicled"
	else
	{ // FIRE button is NOT down
		// put away ray gun after it's shown for the minimum period of time
		if (!pfireframetimer)
		{  // ray gun shown for minimum time
			pfiring = false;
		}
		else
		{  // minimum time not expired
			pfiring = true;
			inhibitwalking = 1;
		}
		plastfire = false;
	}
}



// select the appropriate player frame based on what he's doing
void CPlayer::SelectFrame()
{
    sprite = playerbaseframe;      // basic standing
	
    const int ep = gpBehaviorEngine->getEpisode();

    if (ep==3) sprite++;

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
        else if (pwalking || psemisliding) sprite += pwalkframe;
    }
	
    // if he's going left switch the frame selected above to the
    // appropriate one for the left direction
    if (pShowDir.x == LEFT && !pdie && !pfrozentime)
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

const int bumpamount = 160;

// yorp/scrub etc "bump".
// if solid = false, player can possibly force his way through.
// if solid = true, object acts like a solid "wall".
void CPlayer::bump( const direction_t direction )
{
	if( pjumping == PPREPAREJUMP || pjumping == PPREPAREPOGO || dead || level_done!=LEVEL_NOT_DONE )
		return;

	playSound( SOUND_YORP_BUMP, PLAY_NORESTART );

	if(!pfiring)
		pShowDir.x = pDir.x = direction;

	xinertia = (direction==RIGHT) ? bumpamount : -bumpamount;
	playpushed_decreasetimer = xinertia/10;

	pwalking = true;
}

void CPlayer::bump(const direction_t direction, const int amount )
{
    if( pjumping == PPREPAREJUMP || pjumping == PPREPAREPOGO || dead || level_done!=LEVEL_NOT_DONE )
        return;

    playSound( SOUND_YORP_BUMP, PLAY_NORESTART );

    if(!pfiring)
        pShowDir.x = pDir.x = direction;

    xinertia = (direction==RIGHT) ? amount : -amount;
    playpushed_decreasetimer = xinertia/10;

    pwalking = true;
}

// Scrub, etc "push".
void CPlayer::push( CSpriteObject &theObject )
{
	if( dead || level_done!=LEVEL_NOT_DONE )
		return;

	int obj_lx = theObject.getXLeftPos();
	int obj_midx = theObject.getXMidPos();
	int obj_rx = theObject.getXRightPos();
	int lx = getXLeftPos();
	int midx = getXMidPos();
	int rx = getXRightPos();

	if( midx < obj_midx )
	{
		moveLeft(rx - obj_lx);
		pDir.x = pShowDir.x = LEFT;
	}

	if( midx > obj_midx )
	{
		moveRight(obj_rx - lx);
		pDir.x = pShowDir.x = RIGHT;
	}
	pwalking = true;
}

// plats in keen 3 can push him down as well
void CPlayer::pushDown( CSpriteObject &theObject )
{
    if( dead || level_done!=LEVEL_NOT_DONE )
        return;

    int objY = theObject.getYDownPos();
    int ry = getYUpPos();

    moveDown(objY - ry);
    pDir.y = pShowDir.y = DOWN;

    pwalking = true;
}

void CPlayer::checkSolidDoors()
{
	int mx1 = getXLeftPos();
	int mx2 = getXRightPos();
	int my1 = getYUpPos();
	int my2 = getYDownPos();
	std::vector<CTileProperties> &TileProperty = gpBehaviorEngine->getTileProperties();

	for( int my=my1 ; my<my2 ; my+=(1<<STC) )
	{
		if( (TileProperty[mp_Map->at(mx1>>CSF, my>>CSF)].behaviour>1 &&
				TileProperty[mp_Map->at(mx1>>CSF, my>>CSF)].behaviour<6 ) )
		{
			blockedl = true; break;
		}
	}

	for( int my=my1 ; my<my2 ; my+=(1<<STC) )
	{
		if( (TileProperty[mp_Map->at(mx2>>CSF, my>>CSF)].behaviour>1 &&
				TileProperty[mp_Map->at(mx2>>CSF, my>>CSF)].behaviour<6 ) )
		{
			blockedr = true; break;
		}
	}

	for( int mx=mx1 ; mx<mx2 ; mx+=(1<<STC) )
	{
		if( (TileProperty[mp_Map->at(mx>>CSF, my1>>CSF)].behaviour>1 &&
				TileProperty[mp_Map->at(mx>>CSF, my1>>CSF)].behaviour<6 ) )
		{
			blockedd = true; break;
		}
	}

	for( int mx=mx1 ; mx<mx2 ; mx+=(1<<STC) )
	{
		if( (TileProperty[mp_Map->at(mx>>CSF, my2>>CSF)].behaviour>1 &&
				TileProperty[mp_Map->at(mx>>CSF, my2>>CSF)].behaviour<6 ) )
		{
			blockedu = true; break;
		}
	}
}

LEVEL_TRIGGER CPlayer::pollLevelTrigger()
{
	LEVEL_TRIGGER trigger = mLevelTrigger;
	mLevelTrigger = LVLTRIG_NONE;
	return trigger;
}

void CPlayer::getShotByRay(object_t &obj_type)
{
	if (pfrozentime)
	{
		if( pfrozentime > PFROZEN_THAW )
			pfrozentime = PFROZEN_THAW;

		if(gpBehaviorEngine->getEpisode() == 1)
			return;
	}
	else
	{
		kill();
	}
}

void CPlayer::processStatusScreen()
 {
    if(!m_showStatusScreen)
        return;

    if(!mpStatusScr)
    {
        const int ep = gpBehaviorEngine->getEpisode();

        mpStatusScr.reset(new CStatusScreen(ep, &inventory, mp_levels_completed, ankhtime, playerbaseframe, mSprVar));
    }

    if( mpStatusScr->isClosed() )
    {
        m_showStatusScreen = false;
        mpStatusScr.release();
    }
    else if( gInput.getPressedAnyButtonCommand(m_index) )
    {
        mpStatusScr->close();
        gInput.flushCommands();
    }
}
