/*
 * CObject.cpp
 *
 *  Created on: 17.05.2009
 *      Author: gerstrong
 */

#include "CObject.h"
#include "CLogFile.h"
#include "engine/spritedefines.h"
#include "sdl/CVideoDriver.h"
#include "keen.h"

int CObject::m_number_of_objects = 0; // The current number of total objects we have within the game!

///
// Initialization Routine
///
CObject::CObject(CMap *pmap, Uint32 x, Uint32 y, object_t type) :
m_type(type),
m_index(m_number_of_objects),
HealthPoints(1),
sprite(BLANKSPRITE),
mp_object(NULL),
mp_Map(pmap),
m_blinktime(0),
m_invincible(false)
{
	bboxX1 = 0;
	bboxX2 = 0;
	bboxY1 = 0;
	bboxY2 = 0;
	falling = false;
	m_number_of_objects++;
	exists = true;
	solid = true;
	inhibitfall = false;

	this->x = x;
	this->y = y;
	canbezapped = false;
	onscreen = false;

	cansupportplayer = false;
	
    yinertia = 0;
	xinertia = 0;
	onslope = false;

	scrx = scry = 0;
	dead = false;
	hasbeenonscreen = false;
	honorPriority = true;
	dontdraw = false;
	touchPlayer = touchedBy = 0;
	cansupportplayer = false;
	dying = false;
	m_ActionBaseOffset = 0x0;
	m_direction = NONE;
	m_ActionTicker = 0;
	m_canturnaround = false;

	if(m_type != OBJ_NONE )
	{
		setupObjectType(g_pBehaviorEngine->getEpisode());

		performCollisions(true);
	}
}

void CObject::setupObjectType(int Episode)
{
	switch(m_type)
	{
	// Mainly Episode 1
	case OBJ_GARG: sprite = OBJ_GARG_DEFSPRITE; break;
	case OBJ_BUTLER: sprite = OBJ_BUTLER_DEFSPRITE; break;
	case OBJ_TANK: sprite = OBJ_BUTLER_DEFSPRITE; break;
	case OBJ_ICECHUNK: sprite = OBJ_ICECHUNK_DEFSPRITE; break;
	case OBJ_ICEBIT: sprite = OBJ_ICEBIT_DEFSPRITE; break;
	case OBJ_ICECANNON: sprite = OBJ_ICECHUNK_DEFSPRITE; break;
	case OBJ_ROPE: sprite = OBJ_ROPE_DEFSPRITE; break;

	// Mainly Episode 2
	case OBJ_SCRUB: sprite = OBJ_SCRUB_DEFSPRITE; break;
	case OBJ_GUARDROBOT: sprite = OBJ_TANKEP2_DEFSPRITE; break;
	case OBJ_VORTELITE: sprite = OBJ_VORTELITE_DEFSPRITE; break;
	case OBJ_SPARK: sprite = OBJ_SPARK_DEFSPRITE_EP2; break;

	// Mainly Episode 3
	case OBJ_FOOB: sprite = OBJ_FOOB_DEFSPRITE; break;
	case OBJ_NINJA: sprite = OBJ_NINJA_DEFSPRITE; break;
	case OBJ_MOTHER: sprite = OBJ_MOTHER_DEFSPRITE; break;
	case OBJ_MEEP: sprite = OBJ_MEEP_DEFSPRITE; break;
	case OBJ_BALL: sprite = OBJ_BALL_DEFSPRITE; break;
	case OBJ_JACK: sprite = OBJ_JACK_DEFSPRITE; break;
	case OBJ_MESSIE: sprite = OBJ_NESSIE_DEFSPRITE; break;
	case OBJ_AUTORAY_V: sprite = RAY_VERT_EP3; break;
	case OBJ_SNDWAVE: sprite = OBJ_SNDWAVE_DEFSPRITE; break;

	case OBJ_VORT:
		{
			if(Episode == 1) sprite = OBJ_VORT_DEFSPRITE_EP1;
			else if(Episode == 2) sprite = OBJ_VORT_DEFSPRITE_EP2;
			else if(Episode == 3) sprite = OBJ_VORT_DEFSPRITE_EP3;
		}break;

	case OBJ_BABY:
	{
		if(Episode == 2) sprite = OBJ_BABY_DEFSPRITE_EP2;
		else sprite = OBJ_BABY_DEFSPRITE_EP3;
	}break;

	case OBJ_PLATFORM:
	case OBJ_PLATVERT:
	{
		if(Episode == 2) sprite = OBJ_PLATFORM_DEFSPRITE_EP2;
		else sprite = OBJ_PLATFORM_DEFSPRITE_EP3;
	}break;


	case OBJ_AUTORAY: {
		if(Episode == 1) sprite = ENEMYRAY;
		else if(Episode == 2) sprite = ENEMYRAYEP2;
		sprite = ENEMYRAYEP3;
	}break;

	case OBJ_DOOR: sprite = DOOR_YELLOW_SPRITE; break;
	case OBJ_TELEPORTER: sprite = OBJ_TELEPORTER_DEFSPRITE; break;
	case OBJ_SECTOREFFECTOR: sprite = BLANKSPRITE; break;
	case OBJ_GOTPOINTS: sprite = PT500_SPRITE; break;
	default: sprite = BLANKSPRITE;
	}

}

void CObject::setScrPos( int px, int py )
{
	scrx = px;
	scry = py;
}

// This functions checks, if the enemy is near to the player. In case, that it is
// it will return true. Other case it will return false.
// This used for objects that only can trigger, when it's really worth to do so.
bool CObject::calcVisibility()
{
	int &visibility = g_pBehaviorEngine->getPhysicsSettings().misc.visibility;

	// Platform are always active
	if(m_type == OBJ_PLATFORM || m_type == OBJ_PLATVERT) return true;

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	Uint32 left = (((mp_Map->m_scrollx<<STC)-(visibility<<CSF))<0) ? 0 :
							(mp_Map->m_scrollx<<STC)-(visibility<<CSF);
	Uint32 right = ((mp_Map->m_scrollx+gameres.w)<<STC)+(visibility<<CSF);
	Uint32 up = (((mp_Map->m_scrolly<<STC)-(visibility<<CSF))<0) ? 0 :
							(mp_Map->m_scrolly<<STC)-(visibility<<CSF);
	Uint32 down = ((mp_Map->m_scrolly+gameres.h)<<STC)+(visibility<<CSF);

	bool inscreen = ( right > x && left < x && down > y && up < y );

	// Bullets should disappear when offscreen
	if(m_type == OBJ_SNDWAVE || m_type == OBJ_RAY || m_type == OBJ_FIREBALL)
		if(!inscreen)
			exists=false;

	return inscreen;
}

/**
 * This function will check if the enemy is in the limited scenario,
 * so it will triggered. Happens normally when the Object is seen on the screen.
 */
bool CObject::checkforScenario()
{
	if ( !exists || m_type==OBJ_PLAYER ) return false;

	if( m_type==OBJ_EXPLOSION || m_type==OBJ_EARTHCHUNK
			|| m_type == OBJ_BRIDGE || m_type == OBJ_NONE ) return true;

	// Check if enemy is near enough. If he isn't, don't make him perform. Exception is on the map
	if(!mp_Map->m_worldmap)
		if(!calcVisibility()) return false;

   	onscreen = true;

   	if (hasbeenonscreen ||
		m_type==OBJ_RAY || m_type==OBJ_ROPE ||
		m_type==OBJ_ICECANNON ||
		m_type==OBJ_ICECHUNK || m_type==OBJ_PLATFORM ||
		m_type==OBJ_PLATVERT || m_type==OBJ_YORP ||
		m_type==OBJ_FOOB || m_type==OBJ_SCRUB ||
		m_type == OBJ_SECTOREFFECTOR)
	{
		return true;
    }

   	return false;
}


// Used in some setup mode, like putting the player to
// the current map position
void CObject::moveToForce(int new_x, int new_y)
{
	bool laststate = solid;

	solid = false;
	moveTo(new_x, new_y);
	solid = laststate;
}

// For the vector functions
void CObject::moveDir(const VectorD2<int> &dir)
{
	moveXDir(dir.x);
	moveYDir(dir.y);
}

void CObject::moveTo(const VectorD2<Uint32> &new_loc)
{
	moveTo(new_loc.x, new_loc.y);
}

void CObject::moveTo(int new_x, int new_y)
{
	int amount_x = new_x-x;
	int amount_y = new_y-y;

	if(amount_x < 0) // move left
		moveLeft(-amount_x);
	else if(amount_x > 0) // move right
		moveRight(amount_x);

	if(amount_y < 0) // means up
		moveUp(-amount_y);
	else if(amount_y > 0) // means down
		moveDown(amount_y);
}

void CObject::moveXDir(int amount, bool force)
{
	if(amount<0)
		moveLeft(-amount, force);
	else if(amount>0)
		moveRight(amount, force);
}
void CObject::moveYDir(int amount)
{
	if(amount<0)
		moveUp(-amount);
	else if(amount>0)
		moveDown(amount);
}

void CObject::moveLeft(int amount, bool force)
{
	if(amount <= 0)
		return;

	int y1 = y + bboxY1;
	int y2 = y + bboxY2;

	blockedr = false;
	// If it is forced don't check for collision
	if(force) {
		x -= amount;
		return;
	}

	if( y-amount < 0 )
		return;

	// If object isn't solid it won't be stopped anyway
	if(!solid)
	{
		blockedr = blockedl = false;
		blockedu = blockedd = false;
		x -= amount;
		return;
	}

	do
	{
		performCollisionsSameBox();
		if(!blockedl)
		{
			if(amount > (1<<STC))
			{
				x -= (1<<STC);
				moveSlopedTiles(getXMidPos()-64, y1, y2, -(1<<STC));
				amount -= (1<<STC);
			}
			else
			{
				x -= amount;
				moveSlopedTiles(getXMidPos()-64, y1, y2, -amount);
				amount = 0;
			}
		}
		else break;
	} while( amount > 0 );
}

void CObject::moveRight(int amount, bool force)
{
	if(amount <= 0)
		return;

	int y1 = y + bboxY1;
	int y2 = y + bboxY2;

	blockedl = false;
	if(force) {
		x += amount;
		return;
	}

	if(!solid)
	{
		blockedr = blockedl = false;
		blockedu = blockedd = false;
		x += amount;
		return;
	}

	// process the walking on tiles
	do
	{
		performCollisionsSameBox();
		if(!blockedr)
		{
			if(amount > (1<<STC))
			{
				x += (1<<STC);
				moveSlopedTiles(getXMidPos()+64, y1, y2, (1<<STC));
				amount -= (1<<STC);
			}
			else
			{
				x += amount;
				moveSlopedTiles(getXMidPos()+64, y1, y2, amount);
				amount = 0;
			}
		}
		else break;
	} while( amount > 0 );
}

void CObject::moveUp(int amount)
{
	if(amount <= 0)
		return;

	int y1 = y + bboxY1;

	if( y1-amount < 0 )
		return;

	if(!solid)
	{
		blockedr = blockedl = false;
		blockedu = blockedd = false;
		y -= amount;
		return;
	}

	do
	{
		performCollisionsSameBox();
		if(!blockedu)
		{
			if(amount > (1<<STC))
			{
				y -= (1<<STC);
				amount -= (1<<STC);
			}
			else
			{
				y -= amount;
				amount = 0;
			}
		}
		else break;
	} while( amount > 0 );
}

void CObject::moveDown(int amount)
{
	if(amount <= 0)
		return;

	blockedu = false;

	if(!solid)
	{
		blockedr = blockedl = false;
		blockedu = blockedd = false;
		y += amount;
		return;
	}

	do
	{
		performCollisionsSameBox();
		if(!blockedd)
		{
			if(amount > (1<<STC))
			{
				y += (1<<STC);
				amount -= (1<<STC);
			}
			else
			{
				y += amount;
				amount = 0;
			}
		}
		else break;
	} while(amount > 0);
}

// This decreases the inertia we have of the object in X-direction.
// It should be used for objects, where it must be assured, that the inertia can get
// zero and not pass that limit
void CObject::decreaseXInertia(int value)
{
	if(xinertia < 0) {
		if(xinertia+value > 0) xinertia = 0;
		else xinertia += value;
	}
	else if(xinertia > 0) {
		if(xinertia-value < 0) xinertia = 0;
		else xinertia -= value;
	}
}

// handles inertia and friction for the X direction
// (this is where the xinertia is actually applied to playx)
void CObject::InertiaAndFriction_X()
{
	const int friction_rate = 10;

	int dx=xinertia;
	// check first if the player is not blocked
	if( (!blockedr and dx>0) or (!blockedl and dx<0) )
		moveXDir(dx);
	else
		xinertia = 0;

	// and apply friction to xinertia
	// when pogoing apply friction till we get down to PFASTINCMAXSPEED
	// then stop the friction
	decreaseXInertia(friction_rate);
}

unsigned int CObject::getXPosition()
{ return x; }
unsigned int CObject::getYPosition()
{ return y; }
unsigned int CObject::getXLeftPos()
{ return x+bboxX1; }
unsigned int CObject::getXRightPos()
{ return x+bboxX2; }
unsigned int CObject::getXMidPos()
{ return x+(bboxX2-bboxX1)/2; }
unsigned int CObject::getYUpPos()
{ return y+bboxY1; }
unsigned int CObject::getYDownPos()
{ return y+bboxY2; }
unsigned int CObject::getYMidPos()
{ return y+(bboxY2-bboxY1)/2; }


/**
 * processes falling of an object. Can be player or any other foe
 */
void CObject::processFalling()
{
	if(m_type == OBJ_MESSIE) return;

	// So it reaches the maximum of fallspeed
	if(!inhibitfall)
	{
		CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();

		if( yinertia>0 && !onslope )
			falling = true;
		else
			falling = false;

		// In this case foe is jumping
		if(yinertia<0 && !blockedu)
		{
			moveUp(-yinertia);

			yinertia += Physics.fallspeed_increase;

			if(yinertia > 0) yinertia = 0;
		}
		else if(yinertia>=0 && !blockedd )
		{
			moveDown(yinertia);

			// gradually increase the fall speed up to maximum rate
			if (yinertia>Physics.max_fallspeed)
				yinertia = Physics.max_fallspeed;
			else if (yinertia<Physics.max_fallspeed)
				yinertia += Physics.fallspeed_increase;
		}

		// hit floor or ceiling? set inertia to zero
		if( (blockedd && yinertia>0) || (blockedu && yinertia<0) )
			yinertia = 0;
	}
	else
	{
		moveYDir(yinertia);
	}

	// sometimes, due to mistakes on the map, some foe are embedded into blocks!
	// In order to avoid, that they can't get out, pull them out of there!
}

void CObject::getShotByRay()
{
	if( !m_invincible && HealthPoints>0)
	{
		if(HealthPoints>1 && g_pVideoDriver->getSpecialFXConfig())
			blink(10);
		HealthPoints--;
	}
}

// anything (players/enemies) occupying the map tile at [mpx,mpy] is killed
void CObject::kill_intersecting_tile(int mpx, int mpy, CObject &theObject)
{
	 unsigned int xpix,ypix;
	 unsigned int x, y;
	 xpix = mpx<<CSF;
	 ypix = mpy<<CSF;

	 x = theObject.getXMidPos();
	 y = theObject.getYUpPos();
	 if (theObject.exists)
		 if (xpix-(1<<CSF) <= x && xpix+(1<<CSF) >= x)
			 if (ypix <= y && ypix+(1<<CSF) >= y)
			 {
				 theObject.kill();
				 theObject.dontdraw = true;
			 }
}



// Just kills the object
void CObject::kill()
{
	if ( exists && canbezapped )
	{
		HealthPoints = 0;
		dead = true;
	}
}

/**
 * \brief This function triggers the blinking behavior of an object.
 * 		  Normally this happens, when an enemy gets shot and can get
 * 		  multiple hits.
 * \param frametime	amount of drawn blinking frames during the blitting.
 * 					Every draw cycle performs one
 */
void CObject::blink(Uint16 frametime)
{	m_blinktime = frametime; }

////
// Action format (Galaxy only now...)
////
/**
 * So far only used in Galaxy. Here we performs some stuff for the Action format
 */

bool CObject::getActionNumber(int16_t ActionNumber)
{	return (m_ActionNumber==ActionNumber);	}

int16_t CObject::getActionNumber()
{	return m_ActionNumber;	}


void CObject::setActionForce(size_t ActionNumber)
{
	m_ActionNumber = ActionNumber;
	m_Action.setActionFormat(m_ActionBaseOffset + 30*m_ActionNumber);
}

void CObject::setAction(size_t ActionNumber)
{
	if(m_ActionNumber == ActionNumber) return;
	setActionForce(ActionNumber);
}

// This new function will setup the sprite based on the Action format
void CObject::processActionRoutine()
{

	if(m_direction == LEFT)
		sprite = m_Action.Left_sprite-124;
	else if(m_direction == RIGHT)
		sprite = m_Action.Right_sprite-124;

	calcBouncingBoxes();

	if( m_ActionTicker > m_Action.Delay )
	{
		if( m_Action.Delay != 0 && m_Action.Next_action != 0 )
			m_Action.setNextActionFormat();
		m_ActionTicker = 0;
	}
	else
		m_ActionTicker+=2;
}

////
// For drawing
////

// Functions finally draws the object also considering that there could be a masked
// or priority tile!
void CObject::draw()
{
	if( sprite == BLANKSPRITE )
		return;

	CSprite &Sprite = g_pGfxEngine->getSprite(sprite);
    SDL_Surface *sfc = g_pVideoDriver->getBlitSurface();

	scrx = (x>>STC)-mp_Map->m_scrollx;
	scry = (y>>STC)-mp_Map->m_scrolly;

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	if(scry < gameres.w && scry < gameres.h && exists)
	{
		if(m_blinktime > 0)
		{
			Sprite.drawBlinkingSprite( sfc, scrx, scry );
			m_blinktime--;
		}
		else
			Sprite.drawSprite( sfc, scrx, scry );
		hasbeenonscreen = true;
	}
}

///
// Cleanup Routine
///
CObject::~CObject() {
	if(m_number_of_objects > 0)
		m_number_of_objects--;
}

