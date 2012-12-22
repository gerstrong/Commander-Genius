/*
 * CPlayerBase.cpp
 *
 *  Created on: 19.06.2011
 *      Author: gerstrong
 */

#include "CPlayerBase.h"
#include "CItemEffect.h"
#include "sdl/input/CInput.h"
#include "sdl/sound/CSound.h"

namespace galaxy {

const int CPlayerBase::mEpisodeActionNumMap[2][NUM_KEEN_ACTIONS] =
{
		// Episode 4
		{
				0,  // A_KEEN_STAND
				1,  // A_KEEN_ON_PLAT
				1,  // A_KEEN_BORED
				2,  // A_KEEN_QUESTION
				8,  // A_KEEN_MOON
				11, // A_KEEN_BOOK_OPEN
				15, // A_KEEN_BOOK_READ
				18, // A_KEEN_BOOK_CLOSE
				21, // A_KEEN_LOOKUP
				23, // A_KEEN_LOOKDOWN
				26, // A_KEEN_ACTION_1
				27, // A_KEEN_ACTION_2
				28, // A_KEEN_ACTION_3
				29, // A_KEEN_DIE
				33, // A_KEEN_SHOOT
				35, // A_KEEN_SHOOT_UP
				37, // A_KEEN_ACTION_4
				40, // A_KEEN_SLIDE
				41, // A_KEEN_ENTER_DOOR
				46, // A_KEEN_POLE
				47, // A_KEEN_POLE_CLIMB
				50, // A_KEEN_POLE_SLIDE
				54, // A_KEEN_POLE_SHOOT
				56, // A_KEEN_POLE_SHOOTUP
				58, // A_KEEN_POLE_SHOOTDOWN
				60, // A_KEEN_RUN
				64, // A_KEEN_POGO_START
				65, // A_KEEN_POGO_UP
				66, // A_KEEN_POGO_HIGH
				67, // A_KEEN_JUMP
				68, // A_KEEN_JUMP_DOWN
				69, // A_KEEN_FALL
				71, // A_KEEN_JUMP_SHOOT
				74, // A_KEEN_JUMP_SHOOTUP
				77, // A_KEEN_JUMP_SHOOTDOWN
				80, // A_KEEN_HANG
				82, // A_KEEN_CLIMB
				94	// A_KEEN_RIDING_ON_FOOT
		},

		// Episode 5
		{
				0,  // A_KEEN_STAND
				1,  // A_KEEN_ON_PLAT
				2,  // A_KEEN_BORED
				3,  // A_KEEN_QUESTION
				8,  // A_KEEN_MOON
				11, // A_KEEN_BOOK_OPEN
				15, // A_KEEN_BOOK_READ
				18, // A_KEEN_BOOK_CLOSE
				19, // A_KEEN_LOOKUP
				21, // A_KEEN_LOOKDOWN
				26, // A_KEEN_ACTION_1
				27, // A_KEEN_ACTION_2
				28, // A_KEEN_ACTION_3
				27, // A_KEEN_DIE
				29, // A_KEEN_SHOOT
				31, // A_KEEN_SHOOT_UP
				37, // A_KEEN_ACTION_4
				36, // A_KEEN_SLIDE
				37, // A_KEEN_ENTER_DOOR
				45, // A_KEEN_POLE
				46, // A_KEEN_POLE_CLIMB
				49, // A_KEEN_POLE_SLIDE
				53, // A_KEEN_POLE_SHOOT
				55, // A_KEEN_POLE_SHOOTUP
				57, // A_KEEN_POLE_SHOOTDOWN
				59, // A_KEEN_RUN
				63, // A_KEEN_POGO_START
				64, // A_KEEN_POGO_UP
				65, // A_KEEN_POGO_HIGH
				66, // A_KEEN_JUMP
				67, // A_KEEN_JUMP_DOWN
				68, // A_KEEN_FALL
				70, // A_KEEN_JUMP_SHOOT
				73, // A_KEEN_JUMP_SHOOTUP
				76, // A_KEEN_JUMP_SHOOTDOWN
				79, // A_KEEN_HANG
				81  // A_KEEN_CLIMB
		}//,

		// Episode 6
		//{},

		// Keen Dreams
		//{}
};

CPlayerBase::CPlayerBase(
		CMap *pmap,
		const Uint16 foeID,
		Uint32 x,
		Uint32 y,
		direction_t facedir,
		CInventory &l_Inventory,
		stCheat &Cheatmode) :
CGalaxySpriteObject(pmap, foeID, x, y),
m_Inventory(l_Inventory),
m_camera(pmap,x,y,this),
mPlayerNum(0),
m_Cheatmode(Cheatmode),
mp_processState(NULL)
{
	mActionMap[A_KEEN_DIE] = &CPlayerBase::processDying;
	mActionMap[A_KEEN_DIE+1] = &CPlayerBase::processDying;

	m_walktimer = 0;
	m_timer = 0;
	m_dying = false;
	xDirection = facedir;

	memset(m_playcontrol, 0, PA_MAX_ACTIONS);
	m_camera.setPosition(m_Pos);
}


void CPlayerBase::getAnotherLife(const int lc_x, const int lc_y, const bool display)
{
	m_Inventory.Item.m_lifes++;
	g_pSound->playSound( SOUND_EXTRA_LIFE );
	if(display)
	{
	    const int ep = g_pBehaviorEngine->getEpisode();
	    CItemEffect *lifeUp = new CItemEffect(mp_Map, 0, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[ep-4][10], FADEOUT);
	    g_pBehaviorEngine->m_EventList.add( new EventSpawnObject( lifeUp ) );	    
	}
}







void CPlayerBase::processInput()
{
	// Entry for every player
	m_playcontrol[PA_X] = 0;
	m_playcontrol[PA_Y] = 0;

	if(g_pInput->getHoldedCommand(mPlayerNum, IC_LEFT))
		m_playcontrol[PA_X] -= 100;
	else if(g_pInput->getHoldedCommand(mPlayerNum, IC_RIGHT))
		m_playcontrol[PA_X] += 100;

	if(g_pInput->getHoldedCommand(mPlayerNum, IC_DOWN))
		m_playcontrol[PA_Y] += 100;
	else if(g_pInput->getHoldedCommand(mPlayerNum, IC_UP))
		m_playcontrol[PA_Y] -= 100;

	if(g_pInput->getHoldedCommand(mPlayerNum, IC_UPPERLEFT))
	{
		m_playcontrol[PA_X] -= 100;
		m_playcontrol[PA_Y] -= 100;
	}
	else if(g_pInput->getHoldedCommand(mPlayerNum, IC_UPPERRIGHT))
	{
		m_playcontrol[PA_X] += 100;
		m_playcontrol[PA_Y] -= 100;
	}
	else if(g_pInput->getHoldedCommand(mPlayerNum, IC_LOWERLEFT))
	{
		m_playcontrol[PA_X] -= 100;
		m_playcontrol[PA_Y] += 100;
	}
	else if(g_pInput->getHoldedCommand(mPlayerNum, IC_LOWERRIGHT))
	{
		m_playcontrol[PA_X] += 100;
		m_playcontrol[PA_Y] += 100;
	}

	m_playcontrol[PA_JUMP]   = g_pInput->getHoldedCommand(mPlayerNum, IC_JUMP)   ? 1 : 0;
	m_playcontrol[PA_POGO]   = g_pInput->getHoldedCommand(mPlayerNum, IC_POGO)   ? 1 : 0;

	// The possibility to charge jumps. This is mainly used for the pogo. it is limited to 50
	if( m_playcontrol[PA_JUMP] > 50) m_playcontrol[PA_JUMP] = 50;

	// Two button firing process
	if(g_pInput->getTwoButtonFiring(mPlayerNum))
	{
		if(m_playcontrol[PA_JUMP] && m_playcontrol[PA_POGO])
		{
			m_playcontrol[PA_FIRE] = 1;
			m_playcontrol[PA_JUMP] = 0;
			m_playcontrol[PA_POGO] = 0;
		}
		else if(m_playcontrol[PA_FIRE])
		{
			m_playcontrol[PA_FIRE] = 0;
			m_playcontrol[PA_JUMP] = 0;
			m_playcontrol[PA_POGO] = 0;
			g_pInput->flushCommand(IC_JUMP);
			g_pInput->flushCommand(IC_FIRE);
			g_pInput->flushCommand(IC_POGO);
		}

	}
	else
	{
		m_playcontrol[PA_FIRE] = g_pInput->getHoldedCommand(mPlayerNum, IC_FIRE) ? 1 : 0;
	}
}






// Process the touching of certain tile, like items and hazards...
void CPlayerBase::processLevelMiscFlagsCheck()
{
	// Item which are taken must go into a data structure
	// animation should also be triggered

	stItemGalaxy &m_Item = m_Inventory.Item;

	int l_x = getXLeftPos();
	int l_y = getYUpPos();
	int l_w = getXRightPos() - getXLeftPos();
	int l_h = getYDownPos() - getYUpPos();


	// Deadly hazards! Here Keen dying routine will be triggered
	if(hitdetectWithTilePropertyRect(3, l_x, l_y, l_w, l_h, 2<<STC))
	{
		kill();
	}

	// Another property of the tiles may kill keen, also in god mode
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
	// TODO: Workaround! It seems that the deadly tiles are 17 tiles behind. Not sure, why!
	const int tileIDl = mp_Map->getPlaneDataAt(1, l_x, (l_y+l_h)+(1<<STC));
	const int tileIDr = mp_Map->getPlaneDataAt(1, l_x+l_w, (l_y+l_h)+(1<<STC));
	if(Tile[tileIDl].bup == 9 && Tile[tileIDr].bup == 9 )
	{
	    if(!m_Cheatmode.god)
	    {
		kill(true);
	    }
	}


	if(hitdetectWithTilePropertyRect(4, l_x, l_y, l_w, l_h, 2<<STC))
	{
		const int lc_x = l_x>>CSF;
		const int lc_y = l_y>>CSF;
		mp_Map->setTile( lc_x, lc_y, 0, true, 1 );
		CItemEffect *iEffect = new CItemEffect(mp_Map, 0, lc_x<<CSF, lc_y<<CSF, 215, ANIMATE);
		g_pBehaviorEngine->m_EventList.spawnObj( iEffect );
		m_Item.m_drops++;

		if(m_Item.m_drops >= 100)
		{
			m_Item.m_drops = 0;
			getAnotherLife(lc_x, lc_y, true);
		}

		g_pSound->playSound( SOUND_GET_DROP );
	}

	/// Tile Items (Sprite-Items are handled in the CSpriteItem Class)
	// All the collectable items go from 21 to 28
	for( Uint32 i=21 ; i<=28 ; i++ )
	{
		if(hitdetectWithTilePropertyRect(i, l_x, l_y, l_w, l_h, 2<<STC))
		{
			const int lc_x = l_x>>CSF;
			const int lc_y = l_y>>CSF;
			const int ep = g_pBehaviorEngine->getEpisode();
			
			mp_Map->setTile( lc_x, lc_y, 0, true, 1 );
			g_pBehaviorEngine->m_EventList.spawnObj( new CItemEffect(mp_Map, 0, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[ep-4][4+i-21], FADEOUT) );
			switch(i)
			{
			case 21: m_Item.m_points += 100;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 22: m_Item.m_points += 200;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 23: m_Item.m_points += 500;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 24: m_Item.m_points += 1000;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 25: m_Item.m_points += 2000;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 26: m_Item.m_points += 5000;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 27: getAnotherLife(lc_x, lc_y, true);	break;
			case 28: m_Item.m_bullets += 5;	g_pSound->playSound( SOUND_GET_AMMO );	break;
			default: break;
			}

			if(m_Item.m_points >= m_Item.m_lifeAt)
			{
				getAnotherLife(lc_x, lc_y, false);
				m_Item.m_lifeAt *= 2;
			}

		}
	}
}



void CPlayerBase::guideToTarget(const VectorD2<int> &speed)
{
	// Check x and y diff
	const int xDiff = mTarget.x - getXMidPos();
	const int yDiff = mTarget.y - getYMidPos();
	const int xAbsDiff = abs(xDiff);
	const int yAbsDiff = abs(yDiff);

	// obtained speed
	VectorD2<int> obtSpeed = speed;

	while( xAbsDiff < obtSpeed.x )
		obtSpeed.x /= 2;

	while( yAbsDiff < obtSpeed.y )
		obtSpeed.y /= 2;

	// Give the speed its right direction.
	obtSpeed.x *= xDiff;
	obtSpeed.y *= yDiff;
	obtSpeed.x = (xAbsDiff != 0) ? obtSpeed.x / xAbsDiff : 0;
	obtSpeed.y = (yAbsDiff != 0) ? obtSpeed.y / yAbsDiff : 0;

	moveDir(obtSpeed);
}




const int MAX_WALKSOUNDTIMER = 30;

void CPlayerBase::playWalkSound()
{
	const int time = m_walktimer % MAX_WALKSOUNDTIMER;

	// Process walk timer. This is only for the walking sound
	if( time == MAX_WALKSOUNDTIMER/3 || time == (2*MAX_WALKSOUNDTIMER)/3 )
		playSound( SOUND_KEEN_WALK );
	else if( time == 0 )
		playSound( SOUND_KEEN_WALK2 );

	m_walktimer++;
}


void CPlayerBase::playSwimSound()
{
	const int time = m_walktimer % MAX_WALKSOUNDTIMER;

	// Process walk timer. This is only for the walking sound
	if( time == 0 )
		playSound( SOUND_KEEN_SWIM );

	m_walktimer++;
}






void CPlayerBase::getEaten()
{
	// Here were prepare Keen to die, setting the action to die
	if(!m_Cheatmode.god && !m_dying)
	{
		m_dying = true;
		yinertia = 0;
		dontdraw = true;
		solid = false;
		honorPriority = false;
		m_timer = 0;
		g_pSound->playSound( SOUND_KEEN_DIE, PLAY_NORESTART );
		mp_processState = &CPlayerBase::processGetEaten;
	}
}





void CPlayerBase::processDead()
{
	// must be processed only once!
	if(dead)
	    return;
	
	setActionForce(A_KEEN_DIE);
	
	if(m_Inventory.Item.m_lifes <= 0) // Game over?
	{	    
	    CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	    
	    const std::string end_text("GAME OVER!\n");	    
	    EventContainer.add( new EventSendDialog(end_text) );
	    EventContainer.add( new EventEndGamePlay() );
	}
	else // not yet!
	{		
	    m_Inventory.Item.m_lifes--;
	    
	    // Create the Event Selection screen
	    CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	    
	    std::string loosemsg  = "You didn't make it past\n";
	    loosemsg += mp_Map->getLevelName();
	    EventSendSelectionDialogMsg *pdialogevent = new EventSendSelectionDialogMsg(loosemsg);
	    pdialogevent->addOption("Try Again", new EventRestartLevel() );
	    
	    std::string exitMsg = "Exit to " + g_pBehaviorEngine->mapLevelName;
	    pdialogevent->addOption(exitMsg, new EventExitLevel(mp_Map->getLevel(), false) );
	    EventContainer.add( pdialogevent );
	}
	
	m_dying = false;
	dead = true;
}



const int DIE_FALL_SPEED = 7;
const int DIE_FALL_MAX_INERTIA = 150;
const int DIE_RIGHT_INERTIA = 40;

void CPlayerBase::processDying()
{
	moveYDir(yinertia);
	moveRight(DIE_RIGHT_INERTIA);
	yinertia += DIE_FALL_SPEED;

	if( m_camera.outOfSight() )
	{
		exists = false;
		solid = true;
		honorPriority = true;
		m_Inventory.Item.m_gem.empty();
	}
}




const int DIE_GETEATEN_TIME = 120;
void CPlayerBase::processGetEaten()
{
	if(m_timer >= DIE_GETEATEN_TIME)
	{
		exists = false;
		solid = true;
		honorPriority = true;
		m_timer = 0;
	}
	else
	{
		m_timer++;
	}
}






void CPlayerBase::kill(const bool force)
{
    if(getActionNumber(A_KEEN_ENTER_DOOR))
	return;
    
    if(m_Cheatmode.god && !force)
	return;
    
    // Here were prepare Keen to die, setting the action to die
    if(mp_processState == &CPlayerBase::processDying && yinertia < 0)
	return;
	
    m_dying = true;
    yinertia = -DIE_FALL_MAX_INERTIA;
    setAction( A_KEEN_DIE + (rand()%2) );
    solid = false;
    honorPriority = false;
    g_pSound->playSound( SOUND_KEEN_DIE, PLAY_NORESTART );
}


bool CPlayerBase::checkMapBoundaryR(const int x2)
{
	if( solid && x2 >= (int)((mp_Map->m_width-1)<<CSF) )
		return true;

	return false;
}

bool CPlayerBase::checkMapBoundaryL(const int x1)
{
	if( solid && x1 <= (1<<CSF) )
		return true;

	return false;
}

bool CPlayerBase::checkMapBoundaryU(const int y1)
{
	if( y1 <= (2<<CSF) )
		return true;

	return false;
}


bool CPlayerBase::getActionStatus(int16_t ActionNumber)
{
	const int epID = g_pBehaviorEngine->getEpisode()-4;
	const int relOff = mEpisodeActionNumMap[epID][ActionNumber];

	return (m_Action.getActionFormat(m_ActionBaseOffset + 30*relOff));
}


int CPlayerBase::getSpriteIDFromAction(const int16_t ActionNumber)
{
	ActionFormatType action;

	const int epID = g_pBehaviorEngine->getEpisode()-4;
	const int relOff = mEpisodeActionNumMap[epID][ActionNumber];

	action.setActionFormat(m_ActionBaseOffset + 30*relOff);

	if( xDirection < 0 )
		return action.spriteLeft;
	else
		return action.spriteRight;
}



void CPlayerBase::setActionForce(const size_t ActionNumber)
{
	const int epID = g_pBehaviorEngine->getEpisode()-4;
	mEndOfAction = false;
	m_ActionNumber = ActionNumber;
	m_Action.setActionFormat(m_ActionBaseOffset + 30*mEpisodeActionNumMap[epID][m_ActionNumber]);


	if(mActionMap.empty())
		return;

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
		mp_processState = mActionMap[ActionNumber];
	else
	{
		mEndOfAction = false;
		m_ActionNumber = mActionMap.begin()->first;
		m_Action.setActionFormat(m_ActionBaseOffset + 30*mEpisodeActionNumMap[epID][m_ActionNumber]);
	}
}



};
