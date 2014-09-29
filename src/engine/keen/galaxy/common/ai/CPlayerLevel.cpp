/*
 * CPlayerLevel.cpp
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#include "CPlayerLevel.h"
#include "CBullet.h"
//#include "engine/core/CBehaviorEngine.h"
#include "platform/CPlatform.h"
#include "../CGalaxySpriteObject.h"
#include "../../ep5/ai/CSecurityDoor.h"
#include <base/CInput.h>
#include "sdl/audio/music/CMusic.h"
#include "sdl/audio/Audio.h"
#include "graphics/effects/CColorMerge.h"

#include <base/GsTimer.h>
#include <base/utils/CVec.h>
#include <base/GsLogging.h>

#include "../dialog/CMessageBoxBitmapGalaxy.h"

namespace galaxy {


const int MAX_JUMPHEIGHT = 10;
const int MIN_JUMPHEIGHT = 5;

const int MAX_POGOHEIGHT = 20;
const int MIN_POGOHEIGHT = 5;
const int POGO_SLOWDOWN = 4;

const int POGO_START_INERTIA_VERT = -100;
const int POGO_START_INERTIA_MAX_VERT = -168;
const int POGO_START_INERTIA_IMPOSSIBLE_VERT = -180;
const int POGO_INERTIA_HOR_MAX = 64;
const int POGO_INERTIA_HOR_REACTION = 2;

const int FIRE_RECHARGE_TIME = 5;

const int MAX_POLE_GRAB_TIME = 19;

const int MAX_SCROLL_VIEW = (8<<CSF);

const int BELT_SPEED = 16;

const int STUN_TIME = 30;


CPlayerLevel::CPlayerLevel(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y,
                        std::vector< std::shared_ptr<CGalaxySpriteObject> > &ObjectPtrs, direction_t facedir,
                        CInventory &l_Inventory, stCheat &Cheatmode,
                        const size_t offset, const int playerID) :
CPlayerBase(pmap, foeID, x, y, facedir, l_Inventory, Cheatmode, playerID),
mPlacingGem(false),
mPoleGrabTime(0),
mExitDoorTimer(0),
mStunTimer(0),
mObjectPtrs(ObjectPtrs)
{
	mActionMap[A_KEEN_STAND] = (void (CPlayerBase::*)()) &CPlayerLevel::processStanding;
	mActionMap[A_KEEN_ON_PLAT] = (void (CPlayerBase::*)()) &CPlayerLevel::processStanding;
	mActionMap[A_KEEN_QUESTION] = (void (CPlayerBase::*)()) &CPlayerLevel::processStanding;
	mActionMap[A_KEEN_BORED] = (void (CPlayerBase::*)()) &CPlayerLevel::processStanding;
	mActionMap[A_KEEN_BOOK_OPEN] = (void (CPlayerBase::*)()) &CPlayerLevel::processReadingBook;
	mActionMap[A_KEEN_BOOK_READ] = (void (CPlayerBase::*)()) &CPlayerLevel::processReadingBook;
	mActionMap[A_KEEN_BOOK_CLOSE] = (void (CPlayerBase::*)()) &CPlayerLevel::processReadingBook;	
	mActionMap[A_KEEN_MOON] = (void (CPlayerBase::*)()) &CPlayerLevel::processPants;
	mActionMap[A_KEEN_LOOKUP] = (void (CPlayerBase::*)()) &CPlayerLevel::processLookingUp;
	mActionMap[A_KEEN_LOOKDOWN] = (void (CPlayerBase::*)()) &CPlayerLevel::processLookingDown;
	mActionMap[A_KEEN_SHOOT] = (void (CPlayerBase::*)()) &CPlayerLevel::processShootWhileStanding;
	mActionMap[A_KEEN_SHOOT_UP] = (void (CPlayerBase::*)()) &CPlayerLevel::processShootWhileStanding;
	mActionMap[A_KEEN_SLIDE] = (void (CPlayerBase::*)()) &CPlayerLevel::processSliding;
	mActionMap[A_KEEN_ENTER_DOOR] = static_cast<void (CPlayerBase::*)()>(&CPlayerLevel::processEnterDoor);
	mActionMap[A_KEEN_POLE] = (void (CPlayerBase::*)()) &CPlayerLevel::processPoleClimbingSit;
	mActionMap[A_KEEN_POLE_CLIMB] = (void (CPlayerBase::*)()) &CPlayerLevel::processPoleClimbingUp;
	mActionMap[A_KEEN_POLE_SLIDE] = (void (CPlayerBase::*)()) &CPlayerLevel::processPoleClimbingDown;
	mActionMap[A_KEEN_POLE_SHOOT] = (void (CPlayerBase::*)()) &CPlayerLevel::processPoleClimbingSit;
	mActionMap[A_KEEN_POLE_SHOOTUP] = (void (CPlayerBase::*)()) &CPlayerLevel::processPoleClimbingUp;
	mActionMap[A_KEEN_POLE_SHOOTDOWN] = (void (CPlayerBase::*)()) &CPlayerLevel::processPoleClimbingDown;
	mActionMap[A_KEEN_RUN] = (void (CPlayerBase::*)()) &CPlayerLevel::processRunning;
	mActionMap[A_KEEN_POGO_START] = (void (CPlayerBase::*)()) &CPlayerLevel::processPogoBounce;
	mActionMap[A_KEEN_POGO_UP] = (void (CPlayerBase::*)()) &CPlayerLevel::processPogo;
	mActionMap[A_KEEN_POGO_HIGH] = (void (CPlayerBase::*)()) &CPlayerLevel::processPogo;
	mActionMap[A_KEEN_JUMP] = (void (CPlayerBase::*)()) &CPlayerLevel::processJumping;
	mActionMap[A_KEEN_JUMP_DOWN] = (void (CPlayerBase::*)()) &CPlayerLevel::processJumping;
	mActionMap[A_KEEN_FALL] = (void (CPlayerBase::*)()) &CPlayerLevel::processJumping;
	mActionMap[A_KEEN_JUMP_SHOOT] = (void (CPlayerBase::*)()) &CPlayerLevel::processJumping;
	mActionMap[A_KEEN_JUMP_SHOOTUP] = (void (CPlayerBase::*)()) &CPlayerLevel::processJumping;
	mActionMap[A_KEEN_JUMP_SHOOTDOWN] = (void (CPlayerBase::*)()) &CPlayerLevel::processJumping;
	mActionMap[A_KEEN_HANG] = (void (CPlayerBase::*)()) &CPlayerLevel::processCliffHanging;
	mActionMap[A_KEEN_CLIMB] = (void (CPlayerBase::*)()) &CPlayerLevel::processCliffClimbingUp;
	mActionMap[A_KEEN_STUNNED] = (void (CPlayerBase::*)()) &CPlayerLevel::processStunned;


	m_fire_recharge_time = 0;
	m_EnterDoorAttempt = false;
	m_ptogglingswitch = false;

	m_jumpheight = 0;
	dontdraw = false;
	m_climbing = false;
	m_pogotoggle = false;
	m_jumped = false;
	m_hangtime = 0;
	mExitTouched = false;

	/*for(size_t add = offset ; add <= offset+200*0x1E ; add += 0x02 )
	{
		m_Action.spriteLeft = 0;
		m_Action.spriteRight = 0;
		m_Action.setActionFormat(add);
		setActionSprite();

		for(int sp = 46+39 ; sp <= 46+39 ; sp++)
		{
			if( m_Action.spriteLeft == sp && m_Action.spriteRight == sp)
			{
				printf("sprite %i and %i found at %x\n", m_Action.spriteLeft, m_Action.spriteRight, add);
			}
		}
	}*/

	setupGalaxyObjectOnMap(offset, A_KEEN_STAND);

	performCollisions();
	if(!processActionRoutine())
			exists = false;
}






bool CPlayerLevel::verifyforPole()
{
	// Timeout before Keen can grab again after he jumped off.
	if ( mPoleGrabTime < MAX_POLE_GRAB_TIME )
		return false;

	mPoleGrabTime = 0;

	Uint32 l_x_r = getXRightPos();
	Uint32 l_x_l = getXLeftPos();
	Uint32 l_x = ( l_x_l + l_x_r ) / 2;	
	l_x = (l_x>>CSF)<<CSF;
	const int l_y_up = ( getYUpPos() ) + (4<<STC);
	const int l_y_down = ( ( getYDownPos() >> CSF ) + 1 ) << CSF;

	const int yDir = (m_playcontrol[PA_Y] < 0) ? -1 : 1;

	// Now check if Player has the chance to climb a pole or something similar
	if( ( yDir < 0 && hitdetectWithTileProperty(1, l_x, l_y_up)  ) ||
	    ( yDir > 0 && hitdetectWithTileProperty(1, l_x, l_y_down)  ) ) // 1 -> stands for pole Property
	{
		// Move to the proper X Coordinates, so Keen really grabs it!
        moveTo(Vector2D<int>(l_x - (7<<STC), getYPosition()));

		xinertia = 0;

		//next.x = 0;
		//next.y = 64*yDir;

		// Set Keen in climb mode
		setAction(A_KEEN_POLE);
		m_climbing = true;
		mClipped = false;
		solid = false;
		return true;
	}
	return false;
}


const int POGO_START_INERTIA = 125; // 48 In K5 Disassemble

void CPlayerLevel::makeHimStand()
{	
	if(pSupportedbyobject && g_pBehaviorEngine->getEpisode() == 5)
		setAction(A_KEEN_ON_PLAT);
	else
		setAction(A_KEEN_STAND);
}


void CPlayerLevel::processRunning()
{
	prepareToShoot();

	int px = m_playcontrol[PA_X];
	int py = m_playcontrol[PA_Y];

	// Most of the walking routine is done by the action script itself

	// Center the view after Keen looked up or down
	centerView();

	// He could stand again, if player doesn't move the dpad
	if( px == 0 )
	{
		makeHimStand();
		yDirection = 0;
		handleInputOnGround();
		return;
	}
	else if( !blockedl && !blockedr )
	{
		playWalkSound();
	}


	if ( py == -1)
	{
		if ( verifyforPole() ) return;
	}
	else if ( py == 1)
	{
		if ( verifyforPole() ) return;
	}


	xDirection = 0;
	if( px < 0 )
	{
		xDirection = -1;
	}
	else if( px > 0 )
	{
		xDirection = 1;
	}


	if (state.jumpIsPressed && !state.jumpWasPressed)
	{
		state.jumpWasPressed = true;
		xinertia = xDirection * 16;
		yinertia = -90;
		nextX = nextY = 0;
		state.jumpTimer = 18;
		setAction(A_KEEN_JUMP);
		playSound( SOUND_KEEN_JUMP );
		return;
	}

	if (state.pogoIsPressed && !state.pogoWasPressed)
	{
		state.pogoWasPressed = true;
		xinertia = xDirection * 16;
		yinertia = -POGO_START_INERTIA;
		nextX = 0;
		state.jumpTimer = 24;
		playSound( SOUND_KEEN_POGO );
		setAction(A_KEEN_POGO_START);
		return;
	}


	if (verifyForFalling())
	{
		xinertia = xDirection * 8;
		yinertia = 0;
		setAction(A_KEEN_FALL);
		playSound( SOUND_KEEN_FALL );
		state.jumpTimer = 0;
	}

	if ( (blockedl && xDirection == -1) || (blockedr && xDirection == 1))
	{
		makeHimStand();
		yDirection = 0;
	}

	// He could place a gem
	for( Uint32 i=7 ; i<=10 ; i++ )
	{
		const int l_x = getXMidPos();
		const int l_y = getYDownPos()-(3<<STC);

		// This will change the gemholder to a holder with the gem
		if( hitdetectWithTileProperty(i, l_x, l_y) )
		{
			stItemGalaxy &m_Item = m_Inventory.Item;

			if(i == 7 && m_Item.m_gem.red > 0)
				m_Item.m_gem.red--;
			else if(i == 8 && m_Item.m_gem.yellow > 0)
				m_Item.m_gem.yellow--;
			else if(i == 9 && m_Item.m_gem.blue > 0)
				m_Item.m_gem.blue--;
			else if(i == 10 && m_Item.m_gem.green > 0)
				m_Item.m_gem.green--;
			else
				break;

			moveToHorizontal((l_x>>CSF)<<CSF);
			mPlacingGem = true;
			setAction(A_KEEN_SLIDE);
		}
	}

}


void CPlayerLevel::prepareToShoot()
{
	// He could shoot
	if( m_playcontrol[PA_FIRE] && !m_fire_recharge_time )
	{
		const int newx = getXPosition() + ((xDirection == LEFT) ? -(16<<STC) : (16<<STC));
		const int newy = getYPosition()+(6<<STC);

        const Vector2D<int> newVec(newx, newy);
		tryToShoot(newVec, xDirection, yDirection);

		setAction(A_KEEN_SHOOT);
		mp_processState = (void (CPlayerBase::*)()) &CPlayerLevel::processShootWhileStanding;
		m_fire_recharge_time = FIRE_RECHARGE_TIME;
		return;
	}
}

void CPlayerLevel::handleInputOnGround()
{
	int px = m_playcontrol[PA_X];
	int py = m_playcontrol[PA_Y];


	if(px)
	{
		setAction(A_KEEN_RUN);
		processRunning();

		nextX = (xDirection * m_Action.velX)/4;
		return;
	}

	if( state.jumpIsPressed && !state.jumpWasPressed)
	{
		state.jumpWasPressed = true;
		xinertia = 0;
		yinertia = -90;
		nextY = 0;
		state.jumpTimer = 18;
		setAction(A_KEEN_JUMP);
		playSound( SOUND_KEEN_JUMP );
		return;
	}

	if( state.pogoIsPressed && !state.pogoWasPressed)
	{
		state.pogoWasPressed = true;
		xinertia = 0;
		yinertia = -POGO_START_INERTIA;
		setAction(A_KEEN_POGO_START);
		playSound( SOUND_KEEN_POGO );
		nextY = 0;
		state.jumpTimer = 24;
		return;
	}

	// He could duck or use the pole
	if( py > 0 )
	{
		if(verifyforPole()) return;

		setAction(A_KEEN_LOOKDOWN);
		return;
	}

	// He could press up and do further actions
	if( py < 0 )
	{
		if(!verifyforPole())
		{
			processPressUp();
		}
	}


}


bool CPlayerLevel::moonTiledetected()
{
    if( g_pBehaviorEngine->getEpisode() != 4 )
	return false;
    
    int lx = getXLeftPos()-(1<<STC);
    int ly = getYUpPos()-(1<<STC);
    int lw = getXRightPos()-getXLeftPos()+(2<<STC);
    int lh = getYDownPos()+getYUpPos()+(2<<STC);
    
    if( hitdetectWithTilePropertyRect(16, lx, ly, lw, lh, 1<<STC) )
	return true;
    
    return false;
}



/// Keen standing routine
void CPlayerLevel::processStanding()
{
	prepareToShoot();

	verifyFalling();

	int px = m_playcontrol[PA_X];
	int py = m_playcontrol[PA_Y];

	if( px || py || state.jumpIsPressed || state.pogoIsPressed )
	{
		user1 = user2 = 0;
		handleInputOnGround();

		return;
	}

	if( getActionStatus(A_KEEN_STAND) && !pSupportedbyobject )
	{
		user1++;

		if (user2 == 0 && user1 > 200)
		{
			user2++;
			setAction(A_KEEN_QUESTION);
			user1 = 0;
			return;
		}

		if (user2 == 1 && user1 > 300)
		{
			user2++;
			setAction(A_KEEN_BORED);
			user1 = 0;
			return;
		}

		if (user2 == 2 && user1 > 700)
		{
			user2++;
			
			if(moonTiledetected())
			{
			    setAction(A_KEEN_MOON);			    
			}
			else 
			{
			    setAction(A_KEEN_BOOK_OPEN);
			}
			user1 = 0;
			return;
		}
	}

	// Center the view after Keen looked up or down
	centerView();
}



void CPlayerLevel::processReadingBook()
{
    if( m_playcontrol[PA_X] )
    {
	user1 = user2 = 0;
	setAction(A_KEEN_BOOK_CLOSE);
	return;
    }
    
    if( getActionStatus(A_KEEN_STAND) )
	setAction(A_KEEN_STAND);
}


void CPlayerLevel::processPants()
{
    if( getActionStatus(A_KEEN_STAND) )
	setAction(A_KEEN_STAND);
}



void CPlayerLevel::processLookingDown()
{
	verifyFalling();

	//Try to jump down
	if (state.jumpIsPressed && !state.jumpWasPressed && (blockedd&7) == 1)
	{
		state.jumpWasPressed = true;

		//printf("Tryin' to jump down\n");
		//If the tiles below the player are blocking on any side but the top, they cannot be jumped through
		/*int tile1 = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2, 1);
		int tile2 = CA_TileAtPos(obj->clipRects.tileXmid, obj->clipRects.tileY2+1, 1);

		if (TI_ForeLeft(tile1) || TI_ForeBottom(tile1) || TI_ForeRight(tile1))
			return;

		if (TI_ForeLeft(tile2) || TI_ForeBottom(tile2) || TI_ForeRight(tile2))
			return;

		#define max(a,b) ((a>b)?a:b)

		int deltay = max(gTimer.getTicksPerFrame(),4) << 4;

		//TODO: Moving platforms

		clipRects.unitY2 += deltay;
		posY += deltay;
		nextX = 0;
		nextY = 0;
		g_pSound->play
		setAction(A_KEEN_FALL);
		xinertia = yinertia = 0;*/
		//playSound( SOUND_KEEN_FALL );
		return;
	}


	if ( m_playcontrol[PA_Y] <= 0 || m_playcontrol[PA_X] != 0
			|| (state.jumpIsPressed && !state.jumpWasPressed)
			|| (state.pogoIsPressed && !state.pogoWasPressed))
	{
		makeHimStand();
		yDirection = 0;
		return;
	}


	if( m_playcontrol[PA_Y]>0 )
	{

		if ( m_playcontrol[PA_JUMP] > 0 && !onslope  )
		{
			const bool jumpdowntile = canFallThroughTile();
			if( pSupportedbyobject || jumpdowntile )
			{				
				m_jumpdown = jumpdowntile;				
				blockedd = false;
				
				yinertia = 0;
			
				// This is for platform which might be going way down...
				if( (m_jumpdownfromobject = (pSupportedbyobject != nullptr)) == true )
				{
					if(pSupportedbyobject->yDirection == DOWN )
					{
					    yinertia = 60;
					}
				}
				
				xinertia = 0;
				
				pSupportedbyobject = nullptr;
				setAction( A_KEEN_JUMP_DOWN );
				playSound( SOUND_KEEN_FALL );
			}
		}

		if( m_camera.m_relcam.y < MAX_SCROLL_VIEW )
			m_camera.m_relcam.y += (2<<STC);

		return;
	}

	makeHimStand();
}


// This special code is important, so platforms in all cases will catch Keen when he is falling on them
void CPlayerLevel::processMoveBitDown()
{
    auto obj = mObjectPtrs.begin();
    for(  ; obj != mObjectPtrs.end() ; obj++ )
    {
        if( CPlatform *platform = dynamic_cast<CPlatform*>(obj->get()) )
        {
            if(platform->hitdetect(*this))
            {
                platform->getTouchedBy(*this);
            }
        }
    }

    CSpriteObject::processMoveBitDown();
}



void CPlayerLevel::getTouchedBy(CSpriteObject &theObject)
{
    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
        if(player->getYDownPos() < getYUpPos()+(6<<STC) )
        {
            // The other keen might be able use this one to reach higher places
            const int myAction = getActionNumber();

            if( (myAction < A_KEEN_BOOK_OPEN ||  myAction > A_KEEN_BOOK_CLOSE) &&
                (myAction < A_KEEN_POLE ||  myAction > A_KEEN_POLE_SHOOTDOWN))

            {
               player->blockedd = true;
            }
        }
    }
}






void CPlayerLevel::processInput()
{
	CPlayerBase::processInput();

	if(m_playcontrol[PA_Y] >= 0)
		m_EnterDoorAttempt = false;

	state.jumpWasPressed = state.jumpIsPressed;
	state.pogoWasPressed = state.pogoIsPressed;

	state.jumpIsPressed = m_playcontrol[PA_JUMP];
	state.pogoIsPressed = m_playcontrol[PA_POGO];

	if (!state.jumpIsPressed) state.jumpWasPressed = false;
	if (!state.pogoIsPressed) state.pogoWasPressed = false;
}






void CPlayerLevel::tryToShoot( const Vector2D<int> &pos, const int xDir, const int yDir )
{
	if(m_Inventory.Item.m_bullets > 0)
	{
        spawnObj(new CBullet(mp_Map, 0, pos.x, pos.y, xDir, yDir, mSprVar));
		m_Inventory.Item.m_bullets--;
	}
	else
	{
		playSound( SOUND_GUN_CLICK );
	}
}




void CPlayerLevel::shootInAir()
{
	// process simple shooting
	if( m_playcontrol[PA_Y] < 0 )
	{
		setActionForce(A_KEEN_JUMP_SHOOTUP);
        const Vector2D<int> newVec(getXMidPos()-(3<<STC), getYUpPos()-(16<<STC));
		tryToShoot(newVec, 0, -1);
	}
	else if( m_playcontrol[PA_Y] > 0 )
	{
		setActionForce(A_KEEN_JUMP_SHOOTDOWN);
        const Vector2D<int> newVec(getXMidPos()-(3<<STC), getYDownPos());
		tryToShoot(newVec, 0, 1);
	}
	else
	{
		setActionForce(A_KEEN_JUMP_SHOOT);

        const Vector2D<int> newVec(getXPosition() + ((xDirection < 0) ? -(16<<STC) : (16<<STC)),
									getYPosition()+(4<<STC));
        tryToShoot(newVec, xDirection, 0);
	}
	m_fire_recharge_time = FIRE_RECHARGE_TIME;
}





const int MAX_CLIFFHANG_TIME = 10;

bool CPlayerLevel::checkandtriggerforCliffHanging()
{    
    if(PoleCollision())
	return false;
    
    if(mp_processState == (void (CPlayerBase::*)()) &CPlayerLevel::processPogo)
	return false;
    
    std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
            
    const bool floorNearBy = TileProperty[mp_Map->at((getXMidPos()>>CSF), (getYDownPos()>>CSF)+1)].bup;
    
    if(floorNearBy)
	return false;
    
    const int yUp = getYUpPos()>>CSF;
            
    if( m_playcontrol[PA_X]<0 && blockedl )
    {
	const int xLeft = (getXLeftPos()>>CSF)-1;
	bool check_block = TileProperty[mp_Map->at(xLeft, yUp-1)].bup;
	check_block |= TileProperty[mp_Map->at(xLeft, yUp-1)].bright;
	const bool check_block_lower = TileProperty[mp_Map->at(xLeft, yUp)].bright;
	
	if( !check_block && check_block_lower )
	{
	    setAction(A_KEEN_HANG);
	    setActionSprite();
	    calcBoundingBoxes();
	    
	    Uint32 x = (xLeft+1)<<CSF;
	    Uint32 y = yUp<<CSF;
	    
	    x -= m_BBox.x1;
	    x -= (4<<STC);
	    y -= m_BBox.y1;
	    
	    moveTo(x,y);
	    solid = false;
	    xinertia = 0;
	    yinertia = 0;
	    m_hangtime = MAX_CLIFFHANG_TIME;
	    return true;
	}
    }
    else if( m_playcontrol[PA_X]>0 && blockedr )
    {
	const int xRight = (getXRightPos()>>CSF)+1;	
	bool check_block = TileProperty[mp_Map->at(xRight, yUp-1)].bup;
	check_block |= TileProperty[mp_Map->at(xRight, yUp-1)].bleft;
	bool check_block_lower = TileProperty[mp_Map->at(xRight, yUp)].bleft;
	
	if( !check_block && check_block_lower )
	{
	    setAction(A_KEEN_HANG);
	    setActionSprite();
	    calcBoundingBoxes();
	    
    	    Uint32 x = (xRight)<<CSF;
	    Uint32 y = yUp<<CSF;
	    
	    x -= m_BBox.x2;
	    y -= m_BBox.y1;

	    
	    moveTo(x,y);
	    solid = false;
	    xinertia = 0;
	    yinertia = 0;
	    m_hangtime = MAX_CLIFFHANG_TIME;
	    return true;
	}
    }
    return false;
}







void CPlayerLevel::processCliffHanging()
{
    
    
    	const int yUp = (getYUpPos()+(5<<STC))>>CSF;
	const int xLeft = getXLeftPos()>>CSF;
	const int xRight = getXRightPos()>>CSF;

	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	if( xDirection == LEFT )
	{	    
		bool check_block = TileProperty[mp_Map->at(xLeft-1, yUp)].bup;
		check_block |= TileProperty[mp_Map->at(xLeft, yUp)].bup;

		if( !check_block )
		{
			setAction(A_KEEN_FALL);
			playSound( SOUND_KEEN_FALL );
			solid = true;
			return;
		}
	}
	else if( xDirection == RIGHT )
	{
		bool check_block = TileProperty[mp_Map->at(xRight+1, yUp)].bup;
		check_block |= TileProperty[mp_Map->at(xRight, yUp)].bup;

		if( !check_block )
		{
			setAction(A_KEEN_FALL);
			playSound( SOUND_KEEN_FALL );
			solid = true;
			return;
		}
	}

    
	// In case you released the direction
	if( m_playcontrol[PA_Y] == 0 && m_playcontrol[PA_X] == 0)
	{
		m_hangtime = 0;
	}

	if(m_hangtime > 0)
	{
	    m_hangtime--;
	    return;
	}

	// If you want to climb up
	if( ((xDirection == LEFT)  && (m_playcontrol[PA_X] < 0)) ||
            ((xDirection == RIGHT) && (m_playcontrol[PA_X] > 0))  )
	{
		// This will reset the target whenever the process object of climbing is launched
		mTarget.x = -1;
		mTarget.y = -1;
		setAction(A_KEEN_CLIMB);
	}

	// If you want to fall down.
	else if( m_playcontrol[PA_Y] > 0 ||
		((xDirection == LEFT) && (m_playcontrol[PA_X] > 0)) ||
		((xDirection == RIGHT) && (m_playcontrol[PA_X] < 0))  )
	{
		setAction( A_KEEN_FALL );
		playSound( SOUND_KEEN_FALL );
		solid = true;
		const int dx = 8<<STC;
		moveXDir( (xDirection == LEFT) ? dx : -dx, true);
		setActionSprite();
		calcBoundingBoxes();
	}
}




const int PLAYER_CLIMB_SPEED_Y = 15;

void CPlayerLevel::processCliffClimbingUp()
{
	if(mTarget.x < 0 || mTarget.y < 0)
	{
		std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
		const int spriteID = getSpriteIDFromAction(A_KEEN_STAND);
        GsSprite &standSpr = gGraphics.getSprite(mSprVar,spriteID);


		if( xDirection < 0 ) // left upper
		{
			const int height = (standSpr.getHeight() << (CSF-STC)) - (5<<STC);

			mClimbSpeedX = (5*PLAYER_CLIMB_SPEED_Y)/8;

			const int xMove = (((xDirection)<<CSF)*6)/6;
			int yPos = getYPosition();

			mTarget.x = getXMidPos() + xMove;
			mTarget.y = ((yPos>>CSF)<<CSF);

			int block = TileProperty[mp_Map->at((mTarget.x>>CSF), (mTarget.y>>CSF))].bup;

			if(block)
				mTarget.y -= (1<<CSF);

			mTarget.y -= height;
		}
		else // right upper
		{
			const int height = (standSpr.getHeight() << (CSF-STC)) - (8<<STC);

			mClimbSpeedX = (3*PLAYER_CLIMB_SPEED_Y)/8;

			const int xMove = (((xDirection)<<CSF)*2)/6;
			const int yPos = getYPosition();

			mTarget.x = getXMidPos() + xMove;
			mTarget.y = ((yPos>>CSF)<<CSF);

			int block = TileProperty[mp_Map->at((mTarget.x>>CSF), (mTarget.y>>CSF))].bup;

			if(block)
				mTarget.y -= (1<<CSF);

			mTarget.y -= height;
		}
		
		return;
	}

	// This is where Keen climbs the cliff up.
    guideToTarget( Vector2D<int>(0, 4*PLAYER_CLIMB_SPEED_Y) );

	yDirection = 0;
	const int yDiff = mTarget.y - getYMidPos();

	const int abs_y = abs(yDiff);
	if( abs_y < 24 )
	{	    
	    mp_processState = (void (CPlayerBase::*)()) &CPlayerLevel::processCliffClimbingOntoFloor;
	}
}


void CPlayerLevel::processCliffClimbingOntoFloor()
{
	// This is where Keen climbs the cliff towards the floor.
    guideToTarget( Vector2D<int>(10*mClimbSpeedX, PLAYER_CLIMB_SPEED_Y) );

	if( getActionStatus(A_KEEN_STAND) || getActionStatus(A_KEEN_ON_PLAT) )
	{
	    yDirection = 0;
	    const int xDiff = mTarget.x - getXMidPos();

	    const int abs_x = abs(xDiff);
	    if(abs_x < mClimbSpeedX)
	    {				
            solid = true;
            setActionSprite();
            calcBoundingBoxes();
            makeHimStand();
            calcBoundingBoxes();
            processMove(0,-(1<<CSF));
            processMove(0,(1<<CSF));
	    }
	}
}

bool CPlayerLevel::stun()
{
    bool rejectCriteria = false;

    rejectCriteria |= !blockedd;
    rejectCriteria |= getActionStatus(A_KEEN_SLIDE);

    if(rejectCriteria)
        return false;

    setAction(A_KEEN_STUNNED);
    playSound(SOUND_KEEN_STUNNED);

    mEndOfAction = false;
    m_Action.setActionFormat(0x1868);

    mStunTimer = 0;
    return true;
}


void CPlayerLevel::processStunned()
{
    mStunTimer++;

    if(mStunTimer < STUN_TIME)
	return;

    mStunTimer = 0;

    setAction(A_KEEN_STAND);
}


void CPlayerLevel::processMovingHorizontal()
{
	// Verify facing directions
	if(  m_playcontrol[PA_X]<0  ) // left
		xDirection = LEFT;
	else if( m_playcontrol[PA_X]>0  ) // right
		xDirection = RIGHT;

	moveXDir(m_playcontrol[PA_X]>>1);
}


void CPlayerLevel::processPogoCommon()
{
	if (blockedl && xDirection == -1)
		xinertia = 0;
	if (blockedr && xDirection == 1)
		xinertia = 0;


	if (blockedu)
	{
		//TODO: Something to do with poles (push keen into the centre)
        if (blockedu == 17)	//Pole
		{
			//obj->posY -= 32;
			//obj->clipRects.unitY1 -= 32;
			xinertia = 0;
			//obj->posX = (obj->clipRects.tileXmid << 8) - 32;
		}
		else
		{
			playSound( SOUND_KEEN_BUMPHEAD );
			if (blockedu > 1)
			{
				yinertia += 16;
				if (yinertia < 0)
					yinertia = 0;
			}
			else
			{
				if( yinertia < 0 )
				{
					yinertia = 0;
				}
			}
			state.jumpTimer = 0;
		}
	}



	// Houston, we've landed!
	if(blockedd)
	{
		//yinertia = 0; // Not sure if that's correct
		if (state.jumpTimer == 0)
		{
			yinertia = -POGO_START_INERTIA;
			playSound( SOUND_KEEN_POGO );
			state.jumpTimer = 24;
			setAction(A_KEEN_POGO_UP);
		}
	}
	
	// Let's see if Keen breaks the fuse
	if(mp_Map->mFuseInLevel)
	{
	    int x = getXMidPos();
	    int y = getYDownPos();
	    const int tileID = mp_Map->getPlaneDataAt(1, x, y);
	    
	    if(tileID == 0x078A)
	    {		
		const int t1 = mp_Map->getPlaneDataAt(1, 0, 0);
		const int t2 = mp_Map->getPlaneDataAt(1, 0, (1<<CSF));
		
		x >>= CSF; y >>= CSF;
			
		mp_Map->setTile(x, y, t1, true);
		mp_Map->setTile(x, y+1, t2, true);
		mp_Map->mNumFuses--;
	    }
	}
}


void CPlayerLevel::processPogoBounce()
{
	processPogoCommon();

	yinertia = -POGO_START_INERTIA;
	//nextY = 6 * yinertia;
	state.jumpTimer = 24;

	if( !getActionStatus(A_KEEN_POGO_START) )
		setAction(A_KEEN_POGO_UP);
}



// Here all the pogo code is processed
void CPlayerLevel::processPogo()
{
	processPogoCommon();

	if (!state.jumpTimer)
	{
		if (g_pBehaviorEngine->mDifficulty == EASY)
		{
			performGravityMid();
		}
		else
		{
			performGravityHigh();
		}
	}
	else
	{
		if (state.jumpIsPressed || state.jumpTimer <= 9)
			performGravityLow();
		else
			performGravityHigh();

		if (state.jumpTimer < 2)
			state.jumpTimer = 0;
		else
			state.jumpTimer--;

		if (!state.jumpTimer && m_Action.Next_action )
        {
			setAction(A_KEEN_POGO_HIGH);
        }
	}


	if (m_playcontrol[PA_X])
	{
		//if (!xinertia)
		{
			if(m_playcontrol[PA_X]<0)
				xDirection = -1;
			else if(m_playcontrol[PA_X]>0)
				xDirection = 1;
		}
		//performPhysAccelHor(xDirection, 48); // This value was in omnispeak. 56 might fit better. Please report in case of problems!
		performPhysAccelHor(xDirection, 56);
	}
	else
	{
		//obj->nextX += obj->velX * CK_GetTicksPerFrame();
		if (xinertia < 0) xDirection = -1;
		else if (xinertia > 0) xDirection = 1;
	}
	
	
	
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
            
	const bool ceilNearBy = TileProperty[mp_Map->at((getXMidPos()>>CSF), (getYUpPos()>>CSF)-1)].bup;
    

	if (state.pogoIsPressed && !state.pogoWasPressed)
	{
		state.pogoWasPressed = true;
		setAction(A_KEEN_FALL);
		
		
		
		if(blockedu || ceilNearBy) // This might fix some collision issues
		{
		    yinertia = 0;
		    //moveDown(1<<CSF);
		    blockedu = true;
		}
	}

	moveXDir(xinertia);

	// process Shooting in air
	if( m_playcontrol[PA_FIRE] && !m_fire_recharge_time )
	{
		xinertia = 0;
		shootInAir();
		if(blockedu || ceilNearBy) // This might fix some collision issues
		{
		    yinertia = 0;
		    //moveDown(1<<CSF);
		    blockedu = true;
		}
		
		return;
	}
}





void CPlayerLevel::verifyJumpAndFall()
{
	if ( blockedl && xDirection == -1)
	{
		xinertia = 0;
	}
	if ( blockedr && xDirection == 1)
	{
		xinertia = 0;
	}

	// Did we hit our head on the ceiling?
	if (blockedu)
	{
		//TODO: Something to do with poles (push keen into the centre)
		if (blockedu == 17)	//Pole
		{
			/*obj->posY -= 32;
			obj->clipRects.unitY1 -= 32;*/
			xinertia = 0;
			//obj->posX = (obj->clipRects.tileXmid << 8) - 32;
		}
		else if(!m_Cheatmode.jump)
		{
			playSound( SOUND_KEEN_BUMPHEAD );						
			
			if (blockedu > 1)
			{
				yinertia += 16;
				if (yinertia < 0)
					yinertia = 0;
			}
			else
			{
				if( yinertia<0 )
					yinertia = 0;
			}
			state.jumpTimer = 0;
		}
	}


	// If keen is jumping down, not because he did from an object like a platform,
	// but a tile where Keen can fall through, process this part of code and
	// check if Keen is still jumpinto through any object
	if(!pSupportedbyobject && m_jumpdown)
	{
		if(!canFallThroughTile())
			m_jumpdown = false;
	}

	// Have we landed?
	if (blockedd)
	{
		//obj->deltaPosY = 0;
		//TODO: Check if deadly.

		//TODO: Check if fuse.

		if (state.jumpTimer == 0) // Or standing on a platform.
		{
			user1 = user2 = 0;	// Being on the ground is boring.
			yDirection = 0;
			playSound( SOUND_KEEN_LAND );

			//TODO: Finish these
			if( m_playcontrol[PA_X] != 0 )
			{
				setAction(A_KEEN_RUN);
			}
			else
			{
				makeHimStand();
			}

		}
	}
	/*else if (deltaPosY > 0)
	{
		int temp6 = obj->clipRects.unitY1 - obj->deltaPosY;
		int temp8 = ((obj->clipRects.unitY1 - 64) & 0xFF00) + 64;
		int temp10 = (temp8 >> 8) -1;

		if (temp6 < temp8 && obj->clipRects.unitY1 >= temp8)
		{
			printf("Preparing to hang!\n");
			if (ck_inputFrame.xDirection == -1)
			{
				int tileX = obj->clipRects.tileX1 - ((obj->rightTI)?1:0);
				int tileY = temp10;
				int upperTile = CA_TileAtPos(tileX, tileY, 1);
				int lowerTile = CA_TileAtPos(tileX, tileY+1, 1);
				printf("[%d,%d]: RightTI: %d, UpperTile = %d [%d], LowerTile = %d, [%d]\n",tileX, tileY, obj->rightTI, upperTile, TI_ForeRight(upperTile), lowerTile, TI_ForeRight(lowerTile));
				if ( (!TI_ForeLeft(upperTile) && !TI_ForeRight(upperTile) && !TI_ForeTop(upperTile) && !TI_ForeBottom(upperTile)) &&
					TI_ForeRight(lowerTile) && TI_ForeTop(lowerTile))
				{
					obj->xDirection = -1;
					obj->clipped = false;
					obj->posX = (obj->posX & 0xFF00) + 128;
					obj->posY = (temp8 - 64);
					obj->velY = obj->deltaPosY = 0;
					CK_SetAction2(obj, &CK_ACT_keenHang1);
					printf("Hung left!\n");
				} else printf("Couldn't hang left!\n");
			}
			else if (ck_inputFrame.xDirection == 1)
			{
				int tileX = obj->clipRects.tileX2 + ((obj->leftTI)?1:0);
				int tileY = temp10;
				int upperTile = CA_TileAtPos(tileX, tileY, 1);
				int lowerTile = CA_TileAtPos(tileX, tileY+1, 1);

				if (!TI_ForeLeft(upperTile) && !TI_ForeRight(upperTile) && !TI_ForeTop(upperTile) && !TI_ForeBottom(upperTile) &&
					TI_ForeLeft(lowerTile) && TI_ForeTop(lowerTile))
				{
					obj->xDirection = 1;
					obj->clipped = false;
					obj->posX = (obj->posX & 0xFF00) + 256;
					obj->posY = (temp8 - 64);
					obj->velY = obj->deltaPosY = 0;
					CK_SetAction2(obj, &CK_ACT_keenHang1);
				}
			}
		}
	}*/

	//RF_AddSpriteDraw(&obj->sde, obj->posX, obj->posY, obj->gfxChunk, 0, obj->zLayer);
}


// Processes the jumping of the player
void CPlayerLevel::processJumping()
{    
	if(checkandtriggerforCliffHanging())
	    return;	
		
	verifyJumpAndFall();
	if (state.jumpTimer)
	{
		if(state.jumpTimer > 0 && !m_Cheatmode.jump)
			state.jumpTimer--;

		// Stop moving up if we've let go of control.
		if (!state.jumpIsPressed)
			state.jumpTimer = 0;

		moveYDir(yinertia);

	}
	else
	{
		if(g_pBehaviorEngine->mDifficulty == EASY)
		{
			performGravityMid();
		}
		else	// Normal or Hard
		{
			performGravityHigh();
		}

		if( yinertia >0 && !getActionStatus(A_KEEN_FALL) && !getActionStatus(A_KEEN_FALL+1)  )
		{
			m_Action.setNextActionFormat();
		}
	}
	
	//Move horizontally
	if ( m_playcontrol[PA_X] != 0 )
	{
		xDirection = (m_playcontrol[PA_X] < 0) ? -1 : 1;
		//performPhysAccelHor(xDirection*4, 48); 
		// This was taken from the omnispeak and recalculated. Check if the new 56 is more appropriate. It seems to be.
		performPhysAccelHor(xDirection*4, 56);
	}
	else performPhysDampHorz();

	moveXDir(xinertia);

	if (state.pogoIsPressed && !state.pogoWasPressed)
	{
		state.pogoWasPressed = true;
		setAction(A_KEEN_POGO_UP);
		state.jumpTimer = 0;
		return;
	}

	if (m_playcontrol[PA_Y] < 0)
	{
		verifyforPole();
	}


	if( m_Cheatmode.jump && (state.jumpIsPressed && !state.jumpWasPressed) )
	{
		state.jumpWasPressed = true;
		yinertia = -90;
		state.jumpTimer = 18;
		setAction(A_KEEN_JUMP);
		return;
	}

	// process Shooting in air
	if( m_playcontrol[PA_FIRE] && !m_fire_recharge_time )
		shootInAir();
}






bool CPlayerLevel::canFallThroughTile()
{
	const int tile = mp_Map->getPlaneDataAt(1, getXMidPos(), getYDownPos()+(5<<STC));
	const CTileProperties &TileProp = g_pBehaviorEngine->getTileProperties(1)[tile];
	return ( TileProp.bdown == 0 && TileProp.bup != 0 );
}






void CPlayerLevel::processLookingUp()
{
	// While looking up, Keen could shoot up!
	if( m_playcontrol[PA_FIRE] && !m_fire_recharge_time )
	{
		setActionForce(A_KEEN_SHOOT_UP);
        const Vector2D<int> newVec(getXMidPos()-(3<<STC), getYUpPos()-(16<<STC));
		tryToShoot(newVec, 0, -1);
		m_fire_recharge_time = FIRE_RECHARGE_TIME;
		return;
	}

	if( m_camera.m_relcam.y > -MAX_SCROLL_VIEW )
		m_camera.m_relcam.y -= (2<<STC);

	if( m_playcontrol[PA_Y]<0 )
		return;

	makeHimStand();
	yDirection = 0;
}







// Processes the exiting of the player. Here all cases are held
void CPlayerLevel::processExiting()
{
	Uint32 x = getXMidPos();
	if( ((mp_Map->m_width-2)<<CSF) < x || (2<<CSF) > x )
	{
		g_pSound->playSound( SOUND_LEVEL_DONE );
		g_pMusicPlayer->stop();

        gEventManager.add( new EventExitLevel(mp_Map->getLevel(), true, false, mSprVar) );
        m_Inventory.Item.m_gem.clear();
		mExitTouched = true;
	}
}








#define		MISCFLAG_SWITCHPLATON 5
#define 	MISCFLAG_SWITCHPLATOFF 6
#define		MISCFLAG_SWITCHBRIDGE 15
#define		MISCFLAG_DOOR		2
#define		MISCFLAG_KEYCARDDOOR		32

void CPlayerLevel::processPressUp() {

	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
	const int x_left = getXLeftPos();
	const int x_right = getXRightPos();
	const int x_mid = (x_left+x_right)/2;
	const int up_y = getYUpPos()+(3<<STC);

	Uint32 tile_no = mp_Map->getPlaneDataAt(1, x_mid, up_y);
	int flag = Tile[tile_no].behaviour;

	// pressing a switch
	if (flag == MISCFLAG_SWITCHPLATON || 
	  flag == MISCFLAG_SWITCHPLATOFF ||
	  flag == MISCFLAG_SWITCHBRIDGE)
	{
	  playSound( SOUND_GUN_CLICK );
	  
	  if(flag == MISCFLAG_SWITCHBRIDGE  || flag == 18 )
	  {	    
	      Uint32 newtile;
	      if(Tile[tile_no+1].behaviour == MISCFLAG_SWITCHBRIDGE || 
		 Tile[tile_no+1].behaviour == 18 )
		newtile = tile_no+1;
	      else
		newtile = tile_no-1;
	      
	      mp_Map->setTile( x_mid>>CSF, up_y>>CSF, newtile, true, 1); // Wrong tiles, those are for the info plane
	      PressBridgeSwitch(x_mid, up_y);	      
	  }
	  else
	  {
	    const Uint32 newtile = (flag == MISCFLAG_SWITCHPLATON) ? tile_no+1 : tile_no-1 ;
	    mp_Map->setTile( x_mid>>CSF, up_y>>CSF, newtile, true, 1); // Wrong tiles, those are for the info plane
	    PressPlatformSwitch(x_mid, up_y);
	  }
	  
	  setAction(A_KEEN_SLIDE);
	  m_timer = 0;
	  return;
	}
	/*		var2 = o->boxTXmid*256-64;
	 *		if (o->xpos == var2) {
	 *			setAction(ACTION_KEENENTERSLIDE);
	 *			setAction(A_KEEN_SLIDE);
	 } else {
	   o->time = var2;
	   //setAction(ACTION_KEENPRESSSWITCH);
	   setAction(ACTION_KEENENTERSLIDE);
	 }
	 EnterDoorAttempt = 1;
	 return 1;
	 } */
	
	
	// entering a door
	int flag_left = Tile[mp_Map->getPlaneDataAt(1, x_left, up_y)].behaviour;
	
	if ( !m_EnterDoorAttempt && 
	  (flag_left == MISCFLAG_DOOR || 
	   flag_left == MISCFLAG_KEYCARDDOOR) )
	{
	  //int var2 = mid_x * 256+96;
	  
	  tile_no = mp_Map->getPlaneDataAt(1, x_left+(1<<CSF), up_y);
	  const int info = mp_Map->getPlaneDataAt(2, x_left+(1<<CSF), up_y);
	  int flag_right = Tile[tile_no].behaviour;
	  //if (flag2 == MISCFLAG_DOOR || flag2 == MISCFLAG_KEYCARDDOOR) var2-=256;
	  //if (getXPosition() == var2) {
	      
	      
	    if(flag_right == MISCFLAG_DOOR || 
	       flag_right == MISCFLAG_KEYCARDDOOR) 
	    {
			if (flag == MISCFLAG_KEYCARDDOOR) 
			{
				if (m_Inventory.Item.m_keycards) 
				{
				    m_Inventory.Item.m_keycards = 0;
				    playSound(SOUND_OPEN_EXIT_DOOR);
				    //GetNewObj(0);
				    //new_object->xpos = o->boxTXmid-2;
				    //new_object->ypos = o->boxTY2-4;
				    //new_object->active = 2;
				    //new_object->clipping = 0;
				    //new_object->type = 1;
				    //new_object->action = ACTION_SECURITYDOOROPEN;
				    //check_ground(new_object, ACTION_SECURITYDOOROPEN);
				    //o->action = ACTION_KEENENTERDOOR0;
				    //o->int16 = 0;
                    spawnObj( new CSecurityDoor(getMapPtr(), 0, x_left-(1<<CSF), up_y-(3<<CSF) ) );
				    
				    mTarget = getPosition();
				    mTarget.y -= (1<<CSF);

				    setAction(A_KEEN_ENTER_DOOR);
				    
				    setActionSprite();
                    GsSprite &rSprite = gGraphics.getSprite(mSprVar,sprite);

				    // Here the Player will be snapped to the center

				    const int x_l = (x_left>>CSF);
				    const int x_r = x_l+1;
				    const int x_mid = ( ((x_l+x_r)<<CSF) - (rSprite.getWidth()<<STC)/2 )/2;

				    moveToHorizontal(x_mid);
				    mExitDoorTimer = 110;

				    m_EnterDoorAttempt = true;
				    return;
				} 
				else 
				{	
				    playSound(SOUND_CANT_DO);
				    //SD_PlaySound(SOUND_NOOPENSECURITYDOOR);
				    setAction(A_KEEN_STAND);
				    m_EnterDoorAttempt = true;
				    return;
				}
			} 
			else 
			{
				mTarget = getPosition();
								
				// Illusion for going into the backgroung does not apply on teleporters
				if(info || tile_no != 0x401)
				{
				  mTarget.y -= (1<<CSF);
				}
				
				solid=false;
				
				
				setAction(A_KEEN_ENTER_DOOR);
				
				setActionSprite();
                GsSprite &rSprite = gGraphics.getSprite(mSprVar,sprite);

				// Here the Player will be snapped to the center

				const int x_l = (x_left>>CSF);
				const int x_r = x_l+1;
				const int x_mid = ( ((x_l+x_r)<<CSF) - (rSprite.getWidth()<<STC)/2 )/2;

				moveToHorizontal(x_mid);

				m_EnterDoorAttempt = true;				
				
				return;				
				//PlayLoopTimer = 110;
				//o->action = ACTION_KEENENTERDOOR1
				//o->int16 = 0;
				//if (*MAPSPOT(o->boxTXmid, o->boxTY1, INFOPLANE) == 0) sub_1FE94();
			}
		}// else {
			//o->time = var2;
			//o->action = ACTION_KEENENTERSLIDE;
		//}
		//EnterDoorAttempt = 1;
	}

	// If the above did not happen, then just look up
	setAction(A_KEEN_LOOKUP);
}













void CPlayerLevel::processSliding()
{
	if(mPlacingGem)
	{
		if(m_timer == 0 || m_timer == 5 || m_timer == 8)
		{
			openDoorsTile();
		}
	}

	if(m_timer < 10)
	{
		m_timer++;
		return;
	}


	m_timer = 0;

	if(mPlacingGem)
	{
		int lx = getXMidPos();
		int ly = getYDownPos()-(3<<STC);

		const Uint32 tileno = mp_Map->getPlaneDataAt(1, lx, ly);
		mp_Map->setTile(lx>>CSF, ly>>CSF, tileno+18, true, 1);
		mPlacingGem = false;
		playSound( SOUND_DOOR_OPEN );
	}

    makeHimStand();
}







void CPlayerLevel::processEnterDoor()
{
    // This happens in Keen 5 when Keen enters the exit door and it still has to open.        
    if(mExitDoorTimer > 0)
    {
	mExitDoorTimer--;
	return;    
    }
    
	moveUp(16);
		
	if( getActionStatus(A_KEEN_STAND) )
	    dontdraw = true;

	if( m_Pos.y-mTarget.y > 16 )
	    return;
	
	yDirection = 0;
	setAction(A_KEEN_STAND);
	solid = true;
	
	dontdraw = false;

	int xmid = getXMidPos();
	int y1 = getYDownPos();

	Uint32 t = mp_Map->getPlaneDataAt(2, xmid, y1);
	
	if(t==0)
	    t = mp_Map->getPlaneDataAt(2, xmid, y1-(1<<CSF));

	if(t==0)
	    t = mp_Map->getPlaneDataAt(2, xmid, y1-(2<<CSF));
	
	if(t==0)
	    t = mp_Map->getPlaneDataAt(2, xmid, y1-(3<<CSF));
	
	if (t == 0) 
	{	  
	  
	  bool mustTeleportOnMap = false;
	  
	  // Check if there is a teleporter. In Keen 5 there might be one!
	  if(g_pBehaviorEngine->getEpisode() == 5)
	  {
	    Uint32 teletile = mp_Map->getPlaneDataAt(1, xmid, y1);
	
	    if(teletile==0)
	      teletile = mp_Map->getPlaneDataAt(1, xmid, y1-(1<<CSF));

	    if(teletile==0)
	      teletile = mp_Map->getPlaneDataAt(1, xmid, y1-(2<<CSF));
	
	    if(teletile==0)
	      teletile = mp_Map->getPlaneDataAt(1, xmid, y1-(3<<CSF));
	    
	    // Code for the teleport tile
	    if(teletile == 0x0401)
	    {
	      // There is one!
	      mustTeleportOnMap = true;
	    }
	    
	  }
	  
		//level_state = 13;
		//o->action = ACTION_KEENENTEREDDOOR;
		// TODO: Figure out what this does
		g_pMusicPlayer->stop();
		g_pSound->playSound( SOUND_LEVEL_DONE );

        gEventManager.add( new EventExitLevel(mp_Map->getLevel(), true, mustTeleportOnMap, mSprVar) );
				
		dontdraw = true;
        m_Inventory.Item.m_gem.clear();
		return;
	}

	if (t == 0xB1B1) 
	{
		//level_state = 2;
		//o->action = ACTION_KEENENTEREDDOOR;
		g_pMusicPlayer->stop();
		g_pSound->playSound( SOUND_LEVEL_DONE );		
        gEventManager.add( new EventExitLevel(mp_Map->getLevel(), true, false, mSprVar) );
		dontdraw = true;
        m_Inventory.Item.m_gem.clear();
		return;
	}

	const int ypos = ((t%256 - 1))<<CSF;
	const int xpos = (t >> 8)<<CSF;

    Vector2D<int> new_pos(xpos, ypos);
	moveToForce(new_pos);
	new_pos.x += ((m_BBox.x2-m_BBox.x1)/2);
	new_pos.y += ((m_BBox.y2-m_BBox.y1)/2);

    mp_Map->mGamePlayPos = new_pos;
	m_camera.setPosition(new_pos);    

	//o->ypos = TILE2MU(*t%256 - 1) + 15;
	//o->xpos = (*t >> 8 << 8);
	//o->int16 = 1;
	//o->clipping = 0;
	//set_sprite_action(o, (o->action)->next);
	//o->clipping = 1;
	//sub_183F1(o);
	return;

}







void CPlayerLevel::toggleBridge(const Uint32 newX, const Uint32 newY)
{
	const int start_tile = mp_Map->getPlaneDataAt(1, newX<<CSF, newY<<CSF)-1;
	int end_tile = start_tile+3;
	
	const int ep = g_pBehaviorEngine->getEpisode();
	std::vector<CTileProperties> &tileProp = g_pBehaviorEngine->getTileProperties(1); 
	
	int x = newX;

	if(ep > 4) // In keen 5 and 6 if there is no bridge property break
	{    		    	
	    for(int t = start_tile+1 ;  ; x++ )
	    {
    		t = mp_Map->getPlaneDataAt(1, x<<CSF, newY<<CSF);

		if(tileProp[t].behaviour != 18)
		{
		    end_tile = mp_Map->getPlaneDataAt(1, (x-1)<<CSF, newY<<CSF);;
		    break;
		}
	    }
	}

	/// We found the start of the row, that need to be changed.
	/// Let apply it to the rest of the bridge
	// Apply to the borders

	// bridge opened or closed?
	const bool b_opened = ((start_tile%18)%8 < 4) ? true : false;
	
	x = newX;
	
	bool endReached = false;
	
	for(int t = start_tile ;  ; x++ )
	{
		// Now decide whether the tile is a piece or borders of the bridge
		const Uint32 type = t%18;
		
		if(t == 0)
		  break;

		// These the tiles
		if(type < 16) // not sure here, because I totally don't get how bridges in Keen 5 and 6 works. 
			      // Nevertheless it gets the job done!
		{
			if(b_opened)
				t += 4;
			else
				t -= 4;			
		}
		else
		{
			// It is just a normal piece to remove
			t = (t/18)*18;
			if(b_opened)
				t+=16;
			else
				t+=17;
		}
		const Uint32 NewTile = t;
		t = mp_Map->getPlaneDataAt(1, x<<CSF, newY<<CSF);

		mp_Map->setTile(x-1, newY, NewTile, true, 1);
		mp_Map->setTile(x-1, newY+1, NewTile+18, true, 1);

		if(t == end_tile || endReached)
		{
			if(t%8 < 4)
				// This bridge is opened, close it!
				t += 4;
			else
				// This bridge is closed, open it!
				t -= 4;

			Uint32 new_lasttile = end_tile;
			if(b_opened)
				new_lasttile += 4;
			else
				new_lasttile -= 4;

			mp_Map->setTile(x-1, newY+1, new_lasttile+17, true, 1);
			mp_Map->setTile(x, newY, new_lasttile, true, 1);
			mp_Map->setTile(x, newY+1, new_lasttile+18, true, 1);
			break;
		}
	}
}




/**
 * This function checks whether a bridge must be opened or closed and does this kind of work
 * I'm not really happy with that part of the code and I know that it works for Keen 4. Not sure about the
 * other episodes, but it's well though and should...
 */

void CPlayerLevel::PressBridgeSwitch(const Uint32 lx, const Uint32 ly)
{
	Uint32 targetXY = mp_Map->getPlaneDataAt(2, lx, ly);

	Uint32 newX = targetXY >> 8;
	Uint32 newY = targetXY & 0xFF;
	
	toggleBridge(newX, newY);
}




/**
 * This function will put/release the blockers of some platforms used in Keen Galaxy
 */
void CPlayerLevel::PressPlatformSwitch(const Uint32 lx, const Uint32 ly)
{
  Uint32 targetXY = mp_Map->getPlaneDataAt(2, lx, ly);
  
  Uint32 newX = targetXY >> 8;
  Uint32 newY = targetXY & 0xFF;
  
  const int oldInfoTile = mp_Map->getPlaneDataAt(2, newX<<CSF, newY<<CSF);
  int newTile = 0x0;
  
  if(oldInfoTile == 31) // 31 stands for the plat blocker
  {
    newTile = 0;    
  }
  else if(oldInfoTile >= 0x5B && oldInfoTile <= 0x62 )
  {
    switch( oldInfoTile )
    {
      case 0x5B:
	newTile = 0x5D; break;
      case 0x5C:
	newTile = 0x5E; break;
      case 0x5D:
	newTile = 0x5B; break;	      
      case 0x5E:
	newTile = 0x5C; break;	      
      case 0x5F:
	newTile = 0x61; break;
      case 0x60:
	newTile = 0x62; break;
      case 0x61:
	newTile = 0x5F; break;
      case 0x62:
	newTile = 0x60; break;
      default:
	break;
    }        
  }
  else if(oldInfoTile == 0)
  {
    newTile = 31;
  }
  
  mp_Map->setTile(newX, newY, newTile, true, 2);
  
  return;
}



void CPlayerLevel::exchangeZapper(const int mapx, const int mapy, const int offset, const int stopZapperTile)
{
    // Exchange the tiles until stopTile makes the operation cancelling
    int nextTile = 0;
    int x = mapx; 
    int y = mapy; 
    while( nextTile != stopZapperTile )
    {
      const int curTile = mp_Map->at(x,y);
      nextTile = curTile + offset;
      mp_Map->setTile(x, y, nextTile, true);
      y++;       
    }
} 



void CPlayerLevel::disableZapper(const Uint32 lx, const Uint32 ly)
{
    // Find the inactive zapper tile, if you don't find it, cancel the operation!
    int startZapTile = mp_Map->at(lx, ly);
    int iZapperTile = startZapTile;
    for( ; iZapperTile < startZapTile+4 ; iZapperTile++ )
    {
      if(iZapperTile == 0xA6B) // The inactive Zapper
	break;
    }
    
    if(iZapperTile >= startZapTile+4) // Did not find that zapper. I have to quit sorry!
      return;
          
    // Get the tile where exchanging stops. It's just 3 rows down the tilemap. The width of the tilemap is 18!
    const int stopZapperTile = iZapperTile+2*18;
    
    // Disable all the zapping vertically until the stopping zap tile is exchanged being the last one
    const int offset = iZapperTile-startZapTile;
    exchangeZapper(lx, ly, offset, stopZapperTile);  
}


void CPlayerLevel::enableZapper(const Uint32 lx, const Uint32 ly)
{
    // Find the active zapper tile, if you don't find it, cancel the operation!
    int startZapTile = mp_Map->at(lx, ly);
    int iZapperTile = startZapTile;
    for( ; iZapperTile > startZapTile-4 ; iZapperTile-- )
    {
      if(iZapperTile == 0xA68) // The inactive Zapper
	break;
    }
    
    if(iZapperTile <= startZapTile-4) // Did not find that zapper. I have to quit sorry!
      return;
          
    // Get the tile where exchanging stops. It's just 3 rows down the tilemap. The width of the tilemap is 18!
    const int stopZapperTile = iZapperTile+2*18;
    
    // Disable all the zapping vertically until the stopping zap tile is exchanged being the last one
    const int offset = iZapperTile-startZapTile;
    exchangeZapper(lx, ly, offset, stopZapperTile);          
}


void CPlayerLevel::openDoorsTile()
{
	int lx = getXMidPos();
	int ly = getYDownPos()-(3<<STC);
	Uint32 targetXY = mp_Map->getPlaneDataAt(2, lx, ly);

	Uint32 newX = targetXY >> 8;
	Uint32 newY = targetXY & 0xFF;
	Uint32 tileno, next_tileno;

    // Get the tile position of the last tile to change. on the right of
    std::vector<CTileProperties> &tilePropVec = g_pBehaviorEngine->getTileProperties(1);

    while(1)
	{
		tileno = mp_Map->getPlaneDataAt(1, newX<<CSF, newY<<CSF);
		mp_Map->setTile(newX, newY, tileno+1, true, 1);
		newY++;
		next_tileno = mp_Map->getPlaneDataAt(1, newX<<CSF, newY<<CSF);

        const int freeTileno = mp_Map->getPlaneDataAt(1, (newX+1)<<CSF, newY<<CSF);
        const bool isBlock = tilePropVec[freeTileno].bup;

        if( !isBlock ) // Freeshot for some doors, which might have strange
            continue;   // tiles setup. v.g. Keen 7 Krodacia

        if( next_tileno != tileno && next_tileno != tileno+18 && next_tileno != tileno+2*18)
            break;
    }
}




/*

void CK_KeenHangThink(CK_object *obj)
{
	printf("Just hangin'...\n");
	if (ck_inputFrame.yDirection == -1 || ck_inputFrame.xDirection == obj->xDirection)
	{
		printf("Goin' up!\n");
		obj->currentAction = &CK_ACT_keenPull1;

		obj->clipped = false;

		if(obj->xDirection == 1)
		{
			obj->nextY = -256;
		}
		else
		{
			obj->nextY = -128;
		}
		//TODO: Set keen->zlayer 3

		//if (obj->xDirection == 1)
		//{


	}
	else if (ck_inputFrame.yDirection == 1 || (ck_inputFrame.xDirection && ck_inputFrame.xDirection != obj->xDirection))
	{
		printf("Goin' Down!\n");
		// Drop down.
		obj->currentAction = &CK_ACT_keenFall1;
		obj->clipped = true;
	}
}

void CK_KeenPullThink1(CK_object *obj)
{
	if (obj->xDirection == 1)
		obj->nextX = 128;
	else
		obj->nextY = -128;
}

void CK_KeenPullThink2(CK_object *obj)
{
	printf("PullThink2\n");
	obj->nextX = obj->xDirection * 128;
	obj->nextY = -128;
}

void CK_KeenPullThink3(CK_object *obj)
{
	printf("PullThink3\n");
	obj->nextY = -128;
}

void CK_KeenPullThink4(CK_object *obj)
{
	printf("PullThink4\n");
	obj->nextY = 128;
	obj->clipped = true;
	obj->zLayer = 1;
}

 */


void CPlayerLevel::performPoleHandleInput()
{
	const int px = m_playcontrol[PA_X];
	const int py = m_playcontrol[PA_Y];
	
	if ( px )
		xDirection = (px>0) ? 1 : -1;

	// Shooting things. *ZAP!*
	if( py < 0 )
	{
		// First check player pressed shoot button
		if( m_playcontrol[PA_FIRE] && !m_fire_recharge_time )
		{
			m_fire_recharge_time = FIRE_RECHARGE_TIME;
			setActionForce(A_KEEN_POLE_SHOOTUP);
            const Vector2D<int> newVec(getXMidPos()-(3<<STC), getYUpPos()-(16<<STC));
			tryToShoot(newVec, 0, -1);
			return;
		}

	}
	else if( py > 0 )
	{
		// First check player pressed shoot button
		if( m_playcontrol[PA_FIRE] && !m_fire_recharge_time )
		{
			m_fire_recharge_time = FIRE_RECHARGE_TIME;
			setActionForce(A_KEEN_POLE_SHOOTDOWN);
            const Vector2D<int> newVec(getXMidPos()-(3<<STC), getYDownPos());
			tryToShoot(newVec, 0, 1);
			return;
		}
	}
	else if(!m_fire_recharge_time)
	{
		// First check player pressed shoot button
		if( m_playcontrol[PA_FIRE] )
		{
			m_fire_recharge_time = FIRE_RECHARGE_TIME;
			setActionForce(A_KEEN_POLE_SHOOT);
            const Vector2D<int> newVec(getXPosition() + ((xDirection == LEFT) ? -(16<<STC) : (16<<STC)),
										getYPosition()+(4<<STC));
			tryToShoot(newVec, xDirection, yDirection);
			return;
		}

		setAction(A_KEEN_POLE);
		yDirection = 0;
	}


	if (state.jumpIsPressed && !state.jumpWasPressed)
	{
		state.jumpWasPressed = false;

		if ( px )
		    xinertia = 8*xDirection;
		else
		    xinertia = 0;
		
		yinertia = -80;
		state.jumpTimer = 10;
		setAction(A_KEEN_JUMP);
		playSound( SOUND_KEEN_JUMP );
		yDirection = 1;
		m_climbing = false;
		m_jumped = true;
		mPoleGrabTime = 0;
		solid = true;
		state.poleGrabTime = 0;
	}

	return;
}


void CPlayerLevel::processPoleClimbingSit()
{
	const int px = m_playcontrol[PA_X];
	const int py = m_playcontrol[PA_Y];

	Uint32 l_x_l = getXLeftPos();
	Uint32 l_x = getXMidPos();
	Uint32 l_x_r = getXRightPos();
	Uint32 l_y_up = getYUpPos();
	Uint32 l_y_down = getYDownPos();


	if ( py > 0 )
	{
		setAction(A_KEEN_POLE_SLIDE);
		yDirection = 1;
		processPoleClimbingDown();
		return;
	}
	else if ( py < 0 )
	{

		// Check for the upper side and don't let him move if the pole ends
		if( hitdetectWithTileProperty(1, l_x_l, l_y_up) ||
		    hitdetectWithTileProperty(1, l_x, l_y_up) ||
		    hitdetectWithTileProperty(1, l_x_r, l_y_up) )
		{
			setAction(A_KEEN_POLE_CLIMB);
			yDirection = UP;
		}
		else
		{
			setAction(A_KEEN_POLE);
			yDirection = 0;
		}

		return;
	}


	if ( px )
	{
		// This will check three points and avoid that keen falls on sloped tiles
		const int fall1 = mp_Map->getPlaneDataAt(1, l_x, l_y_down+(1<<CSF));
		const CTileProperties &TileProp1 = g_pBehaviorEngine->getTileProperties(1)[fall1];
		const bool leavePole = (TileProp1.bup != 0);

		if ( leavePole )
		{
			state.jumpWasPressed = false;
			state.jumpIsPressed = true;

			int dir = 0;
			if(px < 0)
				dir = -1;
			else if(px > 0)
				dir = 1;

			xinertia = 8*dir;
			yinertia = -20;
			state.jumpTimer = 10;
			solid = true;
			setAction(A_KEEN_JUMP);
			yDirection = 1;
			m_climbing = false;
			m_jumped = true;
			mPoleGrabTime = 0;
			state.poleGrabTime = 0;

			return;
		}

	}

	performPoleHandleInput();
}



void CPlayerLevel::processPoleClimbingUp()
{
	Uint32 l_x_l = getXLeftPos();
	Uint32 l_x = getXMidPos();
	Uint32 l_x_r = getXRightPos();
	Uint32 l_y_up = getYUpPos();	

	// Check for the upper side and don't let him move if the pole ends
	if( hitdetectWithTileProperty(1, l_x_l, l_y_up) ||
	    hitdetectWithTileProperty(1, l_x, l_y_up) ||
	    hitdetectWithTileProperty(1, l_x_r, l_y_up) )
	{
		setAction(A_KEEN_POLE_CLIMB);
		yDirection = UP;
	}
	else
	{
		setAction(A_KEEN_POLE);
		yDirection = 0;
	}

	performPoleHandleInput();
}



void CPlayerLevel::processPoleClimbingDown()
{
	Uint32 l_x_l = getXLeftPos();
	Uint32 l_x = getXMidPos();
	Uint32 l_x_r = getXRightPos();
	Uint32 l_y_up = getYUpPos()+(16<<STC);
	Uint32 l_y_down = getYDownPos()+(16<<STC);

	if(!hitdetectWithTileProperty(1, l_x, l_y_down))
		solid = true;
	else
		solid = false;

	// Check for the and upper and lower side, upper because the hand can touch the edge in that case
	bool up = false;
	up |= hitdetectWithTileProperty(1, l_x_l, l_y_up);
	up |= hitdetectWithTileProperty(1, l_x, l_y_up);
	up |= hitdetectWithTileProperty(1, l_x_r, l_y_up);
	if( up && !blockedd )
	{
		// Slide down if there is more of the pole
		setAction(A_KEEN_POLE_SLIDE);
		yDirection = DOWN;
	}
	else
	{
		// Fall down if there isn't any pole to slide down
		m_climbing = false;
		yDirection = 0;
		yinertia = 0;
		solid = true;

		bool down = false;
		down |= mp_Map->at(l_x_l>>CSF, l_y_down>>CSF);
		down |= mp_Map->at(l_x>>CSF, l_y_down>>CSF);
		down |= mp_Map->at(l_x_r>>CSF, l_y_down>>CSF);


		// Check if keen is trying to climb through the floor.
		// It's quite a strange clipping error if he succeeds.
		if( !blockedd && !down )
		{
			state.jumpTimer = 0;
			xinertia = 8*xDirection;
			yinertia = 0;

			setAction(A_KEEN_FALL);
			playSound( SOUND_KEEN_FALL );
		}
		else
		{
			blockedd = true;
			moveUp(1<<CSF);
			moveDown(1<<CSF);

			makeHimStand();
			yDirection = 0;

			/*int yReset = -(obj->clipRects.unitY2 & 255) - 1;
			obj->posY += yReset;
			obj->clipRects.unitY2 += yReset;
			obj->clipRects.tileY2 += -1;
			obj->clipped = true;*/
			setAction(A_KEEN_LOOKDOWN);
			//CK_SetAction2(obj, &CK_ACT_keenLookDown1);

		}
		
		return;
	}

	if (m_playcontrol[PA_Y] == 0)
	{
		setAction(A_KEEN_POLE);
		yDirection = 0;
	}
	else if (m_playcontrol[PA_Y] < 0)
	{
		setAction(A_KEEN_POLE_CLIMB);
		yDirection = -1;
	}

	performPoleHandleInput();

}



void CPlayerLevel::processShootWhileStanding()
{
	// while until player releases the button and get back to stand status
	if( !m_playcontrol[PA_FIRE] )
	{
		yDirection = 0;
		if(getActionNumber(A_KEEN_SHOOT_UP))
			setAction(A_KEEN_LOOKUP);
		else
			makeHimStand();
	}
}

void CPlayerLevel::verifyFalling()
{
	if (CSpriteObject::verifyForFalling())
	{
		xinertia = xDirection * 16;
		yinertia = 0;
		setAction(A_KEEN_FALL);
		playSound( SOUND_KEEN_FALL );
		state.jumpTimer = 0;
	}
}




// Falling code
void CPlayerLevel::processFalling()
{
       	// Check Keen could hang on a cliff and do so if possible
	if(checkandtriggerforCliffHanging())
		return;

    
	// If keen is jumping down, not because he did from an object like a platform,
	// but a tile where Keen can fall through, process this part of code and
	// check if Keen is still jumpinto through any object
	if(!pSupportedbyobject && m_jumpdown)
	{
		if(!canFallThroughTile())
			m_jumpdown = false;
	}

	if(blockedd)
	{
		playSound( SOUND_KEEN_LAND );
		makeHimStand();
		yDirection = 0;
	}

	processMovingHorizontal();

	// If Jump mode is enabled he can jump again
	// This will cancel the pole process and make Keen jump
	if( m_Cheatmode.jump && m_playcontrol[PA_JUMP] > 0 )
	{
		setAction(A_KEEN_JUMP);
		m_climbing = false;
		m_jumped = true;
		yinertia = 0;
		yDirection = 0;
		return;
	}

	/// While falling Keen could switch to pogo again anytime
	// but first the player must release the pogo button
	if( !m_playcontrol[PA_POGO] )
		m_pogotoggle = false;

	// Now we can check if player wants to use it again
	if( !m_pogotoggle && m_playcontrol[PA_POGO] )
	{
		m_jumpheight = 0;
		yinertia = 0;
		setAction(A_KEEN_POGO_START);
		m_pogotoggle = true;
	}

	// Check if keen should stick to the pole
	if( m_playcontrol[PA_Y] < 0 )
	{
		verifyforPole();
	}
	
	if( m_playcontrol[PA_FIRE] && !m_fire_recharge_time )
		shootInAir();
}



void CPlayerLevel::centerView()
{
	// If keen looked up or down, this will return the camera to initial position
	if( m_camera.m_relcam.y < 0 )
	{
		m_camera.m_relcam.y += (4<<STC);
		if( m_camera.m_relcam.y > 0 )
			m_camera.m_relcam.y = 0;
	}
	else if( m_camera.m_relcam.y > 0 )
	{
		m_camera.m_relcam.y -= (4<<STC);
		if( m_camera.m_relcam.y < 0 )
			m_camera.m_relcam.y = 0;
	}
}

void CPlayerLevel::push(CGalaxySpriteObject& theObject)
{
  if(dying)
    return;
  
  CPlayerBase::push(theObject);
  
  if(onPole())
  {
    m_climbing = false;
    yDirection = 0;
    yinertia = 0;
    solid = true;
    setAction(A_KEEN_FALL);
  }
}


bool CPlayerLevel::checkConveyorBelt()
{
    if(g_pBehaviorEngine->getEpisode() == 6)
    {
	Uint32 l_x_l = getXLeftPos();
	Uint32 l_x = getXMidPos();
	Uint32 l_x_r = getXRightPos();
	Uint32 l_y_down = getYDownPos()+(1<<STC);
	
	int tileID1 = mp_Map->getPlaneDataAt(1, l_x_l, l_y_down);
	int tileID2 = mp_Map->getPlaneDataAt(1, l_x, l_y_down);
	int tileID3 = mp_Map->getPlaneDataAt(1, l_x_r, l_y_down);
	
	for(int j=0 ; j<4 ; j++) // This will take all the conveyor belts
	{
	    if( (tileID1>=0x83A && tileID1<=0x843) ||
		(tileID2>=0x83A && tileID2<=0x843) ||
		(tileID3>=0x83A && tileID3<=0x843) )
	    {
		return true;
	    }
	    
	    tileID1 -= 36;	tileID2 -= 36;	tileID3 -= 36;
	}
    }	
    return false;
}



void CPlayerLevel::process()
{
    if(m_dying)
    {
        const bool dieStatus1 = getActionStatus(A_KEEN_DIE);

        if( !dieStatus1 )
        {
           setActionForce( A_KEEN_DIE );
        }
        setActionSprite();

    }

    if(dead) // If this player is dead, the camera still runs,
    {       // in case a second player inherits the camera lead
        processCamera();
        return;
    }

    if(mExitTouched)
        return;
    
	if(!m_dying)
	{
		processInput();

		// If no clipping was triggered change solid state of keen
		if(m_Cheatmode.noclipping)
		{
			solid = !solid;
			m_Cheatmode.noclipping = false;
		}

		if(pSupportedbyobject)
		{
			blockedd = true;
		}
	

		if ( mPoleGrabTime < MAX_POLE_GRAB_TIME )
		    mPoleGrabTime++;

        (this->*mp_processState)();
	}
    else // Otherwise keen is dying
    {
        // Might happen if dopefish eats keen
        if(mp_processState == &CPlayerBase::processGetEaten)
        {
            processGetEaten();
        }
        else
        {
            processDying();
        }
    }
	
	
	if(mp_Map->mFuseInLevel && mp_Map->mNumFuses == 0)
	{
	    // TODO: Need to spawn other messages here!
	    g_pMusicPlayer->stop();
	    g_pSound->playSound( SOUND_LEVEL_DONE );
        gEventManager.add( new EventExitLevel(mp_Map->getLevel(), true, false, mSprVar) );
        m_Inventory.Item.m_gem.clear();
        m_Inventory.Item.fuse_levels_completed++;
	    mp_Map->mFuseInLevel = false;
	    return;
	}
	    

	// make the fire recharge time decreased if player is not pressing firing button
	if(m_fire_recharge_time && !m_playcontrol[PA_FIRE])
	{
	    m_fire_recharge_time--;
	}

	processLevelMiscFlagsCheck();

    if(m_dying)
        return;

	if(!mExitDoorTimer)
	{
	    if(!processActionRoutine())
		exists = false;
	}


	if(!m_dying && !mExitTouched)
	{
		processExiting();

        processCamera();

		performCollisions();

		void (CPlayerBase::*PogoProcess)() = static_cast<void (CPlayerBase::*)()>(&CPlayerLevel::processPogo);
		void (CPlayerBase::*FallProcess)() = static_cast<void (CPlayerBase::*)()>(&CPlayerLevel::processFalling);

		// It's not always desired to push out
		if( (mp_processState != PogoProcess) &&
			(mp_processState != FallProcess) )
		{
			processPushOutCollision();
		}
	}
	
	
	// Conveyor Belt in Keen 6
	if(checkConveyorBelt())
	{
	    moveRight(BELT_SPEED);
	}
}

bool CPlayerLevel::verifyAndToggleZapper(const int lx, const int ly)
{
    Uint32 targetXY = mp_Map->getPlaneDataAt(2, lx, ly);
  
    Uint32 newX = targetXY >> 8;
    Uint32 newY = targetXY & 0xFF;
  
    const int zapperTile = mp_Map->getPlaneDataAt(1, newX<<CSF, newY<<CSF);
  
    if(zapperTile >= 0xA68 && zapperTile <= 0xA6A )
    {
	disableZapper(newX, newY);
    }    
    else if(zapperTile == 0xA6B ) // On Keen 6 this seems to be the zapper
    {
	enableZapper(newX, newY);
    }
    else
    {
      return false;
    }
    
    return true;
}

bool CPlayerLevel::verifyAndToggleBridge(const int lx, const int ly)
{
    Uint32 targetXY = mp_Map->getPlaneDataAt(2, lx, ly);
  
    Uint32 newX = targetXY >> 8;
    Uint32 newY = targetXY & 0xFF;
  
    auto &Tile = g_pBehaviorEngine->getTileProperties(1);
    const int zapperTile = mp_Map->getPlaneDataAt(1, newX<<CSF, newY<<CSF);        
    const int flag = Tile[zapperTile].behaviour;
  
    if(flag == 18 )
    {
      toggleBridge(newX, newY);
    }    
    else
    {
      return false;
    }
    
    return true;
}



void CPlayerLevel::TurnGiantSwitchOff(const int x, const int y)
{
    const int tile = mp_Map->at(x, y);
    
    if(tile != 0x43C || yinertia > 0)
	return;
    
    for(int i=-1 ; i<=0 ; i++)
    {
	for(int j=-1 ; j<=1 ; j++)
	{
	    const int tile = mp_Map->at(x+i, y+j);
	    mp_Map->setTile( x+i, y+j, tile-3, true );
	}
    }    
    
    std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
    const int x_csf = x<<CSF;
    const int y_csf = y<<CSF;
    
    Uint32 tile_no = mp_Map->getPlaneDataAt(1, x_csf, y_csf);
    int flag = Tile[tile_no].behaviour;
    
    // pressing a switch
    playSound( SOUND_GUN_CLICK );
	
    if(flag == 18)
    {	    
	PressBridgeSwitch(x_csf, y_csf);	      
    }
    else
    {
      if( verifyAndToggleZapper(x_csf, y_csf) )
	return;
      
      if( verifyAndToggleBridge(x_csf, y_csf) )
	  return;

      PressPlatformSwitch(x_csf, y_csf);
    }    
}

void CPlayerLevel::TurnGiantSwitchOn(const int x, const int y)
{
    const int tile = mp_Map->at(x, y);
    
    if(tile != 0x439 || yinertia < 0)
	return;    
    
    playSound( SOUND_GUN_CLICK );
    
    for(int i=-1 ; i<=0 ; i++)
    {
	for(int j=-1 ; j<=1 ; j++)
	{
	    const int tile = mp_Map->at(x+i, y+j);
	    mp_Map->setTile( x+i, y+j, tile+3, true );
	}
    }
    
    std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
    const int x_csf = x<<CSF;
    const int y_csf = y<<CSF;
    
    Uint32 tile_no = mp_Map->getPlaneDataAt(1, x_csf, y_csf);
    int flag = Tile[tile_no].behaviour;
    
    // pressing a switch
    playSound( SOUND_GUN_CLICK );
	
    if(flag == 18 )
    {	    
	PressBridgeSwitch(x_csf, y_csf);	      
    }
    else
    {
      if( verifyAndToggleZapper(x_csf, y_csf) )
	return;

      if( verifyAndToggleBridge(x_csf, y_csf) )
	  return;      
      
      PressPlatformSwitch(x_csf, y_csf);
    }    
}



int CPlayerLevel::checkSolidU(int x1, int x2, int y1, const bool push_mode )
{
	if(hitdetectWithTilePropertyHor(1, x1, x2, y1-COLISION_RES, 1<<CSF))
	    return 0;
    
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	y1 -= COLISION_RES;

	// Check for sloped tiles here. They must be handled differently
	if(solid)
	{
		int8_t blocked;

		if(m_climbing)
		{
			x1 += 4*COLISION_RES;
			x2 -= 4*COLISION_RES;
		}

		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blocked = TileProperty[mp_Map->at(c>>CSF, y1>>CSF)].bdown;

			if(blocked == 33)
			{
			    TurnGiantSwitchOff(c>>CSF, y1>>CSF);
			    
			    if(!m_Cheatmode.jump)
			    {
				playSound( SOUND_KEEN_BUMPHEAD );
				
				if (blockedu > 1)
				{
				    yinertia += 16;
				    if (yinertia < 0)
					yinertia = 0;
				}
				else
				{
				    if( yinertia<0 )
					yinertia = 0;
				}
				state.jumpTimer = 0;
			    }		    
			    
			    return 1;
			}			
			
			if(blocked == 17 && m_climbing)
				return 0;

			if( blocked >= 2 && blocked <= 7 && checkslopedU(c, y1, blocked))
				return blocked;
		}

		blocked = TileProperty[mp_Map->at(x2>>CSF, y1>>CSF)].bdown;
		
		if( blocked >= 2 && blocked <= 7 && checkslopedU(x2, y1, blocked ))
			return 1;

		if(blocked == 17 && m_climbing)
			return 0;
	}

	return CSpriteObject::checkSolidU(x1, x2, y1+COLISION_RES, push_mode);
}


int CPlayerLevel::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	y2 += COLISION_RES;

	// Check for sloped tiles here. They must be handled differently
	if(solid)
	{
		int8_t blockedu;

		if(m_climbing)
		{
			x1 += 4*COLISION_RES;
			x2 -= 4*COLISION_RES;
		}

		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blockedu = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup;

			if(blockedu == 33)
			{
			    TurnGiantSwitchOn(c>>CSF, y2>>CSF);
			    
			    const int action = getActionNumber();
			    
			    if ( action >= A_KEEN_POGO_START && action <= A_KEEN_POGO_HIGH && state.jumpTimer == 0)
			    {
				yinertia = -POGO_START_INERTIA;
				playSound( SOUND_KEEN_POGO );
				state.jumpTimer = 24;
				setAction(A_KEEN_POGO_UP);
				return 1;
			    }		    
			}						
			
			if( blockedu == 17 && m_climbing)
				return 0;

			if( blockedu >= 2 && blockedu <= 7 && checkslopedD(c, y2, blockedu) )
				return blockedu;
		}

		blockedu = TileProperty[mp_Map->at(x2>>CSF, y2>>CSF)].bup;		

		if(blockedu == 17 && m_climbing)
			return 0;

		if( blockedu >= 2 && blockedu <= 7 && checkslopedD(x2, y2, blockedu)  )
			return blockedu;
	}


	if( ( (y2>>STC) != ((y2>>CSF)<<TILE_S) ) && !push_mode )
		return 0;


	// Check for down from the object
	if(solid)
	{
		int8_t blocked;
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blocked = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup;

			if(blocked)
			{
				if( blocked < 2 || blocked > 7 )
				{
					int8_t blockedd = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bdown;

					if(blockedd == 0 && m_jumpdown)
						return 0;
					
					if(blockedd == 33) // keen 6 Giant switch
					    return 0;

					return blocked;
				}
			}
		}

		blocked = TileProperty[mp_Map->at((x2-(1<<STC))>>CSF, y2>>CSF)].bup;
		if(blocked)
		{
			if( blocked < 2 || blocked > 7 )
				return blocked;
		}
	}

	if( (Uint32)y2 > ((mp_Map->m_height)<<CSF) )
		exists=false; // Out of map?

	return 0;
}



}
