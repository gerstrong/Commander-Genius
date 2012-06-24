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
#include "SmartPointer.h"

namespace galaxy {

CPlayerBase::CPlayerBase(
		CMap *pmap,
		Uint32 x,
		Uint32 y,
		std::vector< SmartPointer<CSpriteObject> >& ObjectPtrs,
		direction_t facedir,
		CInventory &l_Inventory,
		stCheat &Cheatmode) :
CGalaxySpriteObject(pmap, x, y),
m_Inventory(l_Inventory),
m_camera(pmap,x,y,this),
mPlayerNum(0),
m_ObjectPtrs(ObjectPtrs),
m_Cheatmode(Cheatmode),
mp_processState(NULL)
{
	m_walktimer = 0;
	m_timer = 0;
	m_dying = false;
	m_hDir = facedir;

	memset(m_playcontrol, 0, PA_MAX_ACTIONS);
	m_camera.setPosition(m_Pos);
}







void CPlayerBase::getAnotherLife(const int &lc_x, const int &lc_y)
{
	m_Inventory.Item.m_lifes++;
	g_pSound->playSound( SOUND_EXTRA_LIFE );
	m_ObjectPtrs.push_back(new CItemEffect(mp_Map, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[10]));
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
	if(hitdetectWithTilePropertyRect(3, l_x, l_y, l_w, l_h, 1<<STC))
		kill();

	if(hitdetectWithTilePropertyRect(4, l_x, l_y, l_w, l_h, 1<<STC))
	{
		const int lc_x = l_x>>CSF;
		const int lc_y = l_y>>CSF;
		mp_Map->setTile( lc_x, lc_y, 0, true, 1 );
		m_ObjectPtrs.push_back(new CItemEffect(mp_Map, lc_x<<CSF, lc_y<<CSF, 215, ANIMATE));
		m_Item.m_drops++;

		if(m_Item.m_drops >= 100)
		{
			m_Item.m_drops = 0;
			getAnotherLife(lc_x, lc_y);
		}

		g_pSound->playSound( SOUND_GET_DROP );
	}

	/// Tile Items (Sprite-Items are handled in the CSpriteItem Class)
	// All the collectable items go from 21 to 28
	for( Uint32 i=21 ; i<=28 ; i++ )
	{
		if(hitdetectWithTilePropertyRect(i, l_x, l_y, l_w, l_h, 1<<STC))
		{
			const int lc_x = l_x>>CSF;
			const int lc_y = l_y>>CSF;
			mp_Map->setTile( lc_x, lc_y, 0, true, 1 );
			m_ObjectPtrs.push_back(new CItemEffect(mp_Map, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[4+i-21]));

			switch(i)
			{
			case 21: m_Item.m_points += 100;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 22: m_Item.m_points += 200;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 23: m_Item.m_points += 500;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 24: m_Item.m_points += 1000;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 25: m_Item.m_points += 2000;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 26: m_Item.m_points += 5000;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 27: getAnotherLife(lc_x, lc_y);	break;
			case 28: m_Item.m_bullets += 5;	g_pSound->playSound( SOUND_GET_AMMO );	break;
			default: break;
			}

			if(m_Item.m_points >= m_Item.m_lifeAt)
			{
				getAnotherLife(lc_x, lc_y);
				m_Item.m_lifeAt *= 2;
			}

		}
	}
}




const int MAX_WALKSOUNDTIMER = 20;

void CPlayerBase::makeWalkSound()
{
	const int time = m_walktimer % MAX_WALKSOUNDTIMER;

	// Process walk timer. This is only for the walking sound
	if( time == MAX_WALKSOUNDTIMER/2 )
		playSound( SOUND_KEEN_WALK2 );
	else if( time == 0 )
		playSound( SOUND_KEEN_WALK );

	m_walktimer++;
}


void CPlayerBase::makeSwimSound()
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

	m_Inventory.Item.m_lifes--;
	setActionForce(A_KEEN_DIE);

	// Create the Event Selection screen
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	/*std::string loosemsg = "You didn't make it past\n";
	loosemsg 			+= mp_Map->getLevelName();
	EventSendSelectionDialogMsg *pdialogevent = new EventSendSelectionDialogMsg(loosemsg);
	pdialogevent->addOption("Try Again", new EventRestartLevel() );
	pdialogevent->addOption("Exit to World Map", new EventExitLevel(mp_Map->getLevel(), false) );
	EventContainer.add( pdialogevent );*/

	EventContainer.add( new EventExitLevel(mp_Map->getLevel(), false) );

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






void CPlayerBase::kill()
{
	// Here were prepare Keen to die, setting the action to die
	if(!m_Cheatmode.god)
	{
		if(mp_processState == &CPlayerBase::processDying && yinertia < 0)
			return;

		m_dying = true;
		yinertia = -DIE_FALL_MAX_INERTIA;
		setAction( A_KEEN_DIE + (rand()%2) );
		solid = false;
		honorPriority = false;
		g_pSound->playSound( SOUND_KEEN_DIE, PLAY_NORESTART );
		mp_processState = &CPlayerBase::processDying;
	}
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




};
