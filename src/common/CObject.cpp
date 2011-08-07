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
m_invincible(false),
m_Pos(x,y),
transluceny(0)
{
	m_jumpdown = false;
	falling = false;
	m_number_of_objects++;
	exists = true;
	solid = true;
	inhibitfall = false;
	canbezapped = false;
	onscreen = false;
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
	m_vDir = m_hDir = NONE;
	m_ActionTicker = 0;
	m_canturnaround = false;
	m_climbing = false;
	supportedbyobject = false;

	blockedd = false;
	blockedu = false;
	blockedl = false;
	blockedr = false;

	if(m_type != OBJ_NONE )
	{
		setupObjectType(g_pBehaviorEngine->getEpisode());
		performCollisions();
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
	default: sprite = BLANKSPRITE; break;
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
	if( m_type == OBJ_PLATFORM || m_type == OBJ_PLATVERT )
		return true;

	// If an object is in the mid-air still moves it still,
	// until it gets stuck to ceiling, wall or floor
	if( !blockedd && m_type!=OBJ_SCRUB  ) return true;

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	Uint32 left = (((mp_Map->m_scrollx<<STC)-(visibility<<CSF))<0) ? 0 :
							(mp_Map->m_scrollx<<STC)-(visibility<<CSF);
	Uint32 right = ((mp_Map->m_scrollx+gameres.w)<<STC)+(visibility<<CSF);
	Uint32 up = (((mp_Map->m_scrolly<<STC)-(visibility<<CSF))<0) ? 0 :
							(mp_Map->m_scrolly<<STC)-(visibility<<CSF);
	Uint32 down = ((mp_Map->m_scrolly+gameres.h)<<STC)+(visibility<<CSF);

	bool inscreen = ( right > m_Pos.x && left < m_Pos.x && down > m_Pos.y && up < m_Pos.y );

	// Bullets should disappear when offscreen
	if(m_type == OBJ_SNDWAVE || m_type == OBJ_RAY || m_type == OBJ_FIREBALL)
	{
		if(!inscreen)
			exists=false;
	}

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





bool CObject::verifyForFalling()
{
	if( !blockedd )
	{
		// This will check three points and avoid that keen falls on sloped tiles
		const int &fall1 = mp_Map->getPlaneDataAt(1, getXMidPos(), getYDownPos());
		const int &fall2 = mp_Map->getPlaneDataAt(1, getXMidPos(), getYDownPos()+(1<<(CSF)));
		const int &fall3 = mp_Map->getPlaneDataAt(1, getXMidPos(), getYDownPos()+(2<<(CSF)));
		const CTileProperties &TileProp1 = g_pBehaviorEngine->getTileProperties(1)[fall1];
		const CTileProperties &TileProp2 = g_pBehaviorEngine->getTileProperties(1)[fall2];
		const CTileProperties &TileProp3 = g_pBehaviorEngine->getTileProperties(1)[fall3];
		const bool nothing_on_feet = (TileProp1.bup == 0);
		const bool nothing_below_feet = (TileProp2.bup == 0) && (TileProp3.bup == 0);
		const bool can_fall = (nothing_on_feet && nothing_below_feet);

		if(can_fall)
		{
			return true;
		}
		else
		{
			// Force the player a bit down, so he will never fall from sloped tiles
			moveDown(100);
		}
	}

	return false;
}



void CObject::performCliffStop(const int &speed)
{
	if(verifyForFalling())
	{
		if( m_hDir == RIGHT )
		{
			m_hDir = LEFT;
			moveLeft( speed );
		}
		else
		{
			m_hDir = RIGHT;
			moveRight( speed );
		}
	}
}






// Used in some setup mode, like putting the player to
// the current map position
void CObject::moveToForce(const VectorD2<int> &dir)
{
	m_Pos = dir;
}

void CObject::moveToForce(const int& new_x, const int& new_y)
{
	moveToForce(VectorD2<int>(new_x, new_y));
}

// For the vector functions
void CObject::moveDir(const VectorD2<int> &dir)
{
	moveXDir(dir.x);
	moveYDir(dir.y);
}

void CObject::moveToHorizontal(const int& new_x)
{
	const int pos_x = new_x - m_Pos.x;
	moveXDir(pos_x);
}

void CObject::moveToVertical(const int& new_y)
{
	const int pos_y = new_y - m_Pos.y;
	moveYDir(pos_y);
}

void CObject::moveTo(const VectorD2<Uint32> &new_loc)
{
	VectorD2<int> amount = new_loc - m_Pos;

	moveXDir(amount.x);
	moveYDir(amount.y);
}

void CObject::moveTo(const int new_x, const int new_y)
{
	moveTo(VectorD2<Uint32>(new_x, new_y));
}

void CObject::moveXDir(const int amount, const bool force)
{
	if(amount<0)
		moveLeft(-amount, force);
	else if(amount>0)
		moveRight(amount, force);
}
void CObject::moveYDir(const int amount)
{
	if(amount<0)
		moveUp(-amount);
	else if(amount>0)
		moveDown(amount);
}

void CObject::moveLeft(const int amnt, const bool force)
{
	if(amnt <= 0)
		return;

	m_EventCont.add(new ObjMove(-amnt,0));
}

void CObject::moveRight(const int amnt, const bool force)
{
	if(amnt <= 0)
		return;

	m_EventCont.add(new ObjMove(amnt,0));
}

void CObject::moveUp(const int amnt)
{
	if(amnt <= 0)
		return;

	m_EventCont.add(new ObjMove(0,-amnt));
}

void CObject::moveDown(const int amnt)
{
	// If zero was given as argument return.
	if(amnt <= 0)
		return;

	m_EventCont.add(new ObjMove(0, amnt));
}

// This decreases the inertia we have of the object in X-direction.
// It should be used for objects, where it must be assured, that the inertia can get
// zero and not pass that limit
void CObject::decreaseXInertia(const int& value)
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

VectorD2<Uint32> CObject::getPosition() const
{ return m_Pos; }
Uint32 CObject::getXPosition() const
{ return m_Pos.x; }
Uint32 CObject::getYPosition() const
{ return m_Pos.y; }
Uint32 CObject::getXLeftPos()
{ return m_Pos.x+m_BBox.x1; }
Uint32 CObject::getXRightPos()
{ return m_Pos.x+m_BBox.x2; }
Uint32 CObject::getXMidPos()
{ return m_Pos.x+(m_BBox.x2-m_BBox.x1)/2; }
Uint32 CObject::getYUpPos()
{ return m_Pos.y+m_BBox.y1; }
Uint32 CObject::getYDownPos()
{ return m_Pos.y+m_BBox.y2; }
Uint32 CObject::getYMidPos()
{ return m_Pos.y+(m_BBox.y2-m_BBox.y1)/2; }


/**
 * processes falling of an object. Can be player or any other foe
 */
void CObject::processFalling()
{
	// CAUTION: There is a difference between falling and going down with the gravity...

	if(m_type == OBJ_MESSIE) return;

	// So it reaches the maximum of fallspeed
	if(!inhibitfall && !m_climbing)
	{
		CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();

		// In this case foe is jumping?
		// Not sure here. We should use another variable...
		if(yinertia<0 && !blockedu)
		{
			moveUp(-yinertia);
			yinertia += Physics.fallspeed_increase;
		}
		else if( yinertia>=0 && !blockedd )
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

		// If object is not falling (yinertia >= 0) and blocked he cannot be falling
		if(blockedd && yinertia>=0)
			falling = false;
	}
	else
	{
		moveYDir(yinertia);
	}

	// sometimes, due to mistakes on the map, some foe are embedded into blocks!
	// In order to avoid, that they can't get out, pull them out of there!
}

void CObject::getShotByRay(object_t &obj_type)
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
	 {
		 if (xpix-(1<<CSF) <= x && xpix+(1<<CSF) >= x)
		 {
			 if (ypix <= y && ypix+(1<<CSF) >= y)
			 {
				 theObject.kill();
				 theObject.dontdraw = true;
			 }
		 }
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

void CObject::playSound( const GameSound snd,
			    const SoundPlayMode mode )
{
	g_pSound->playStereofromCoord(snd, mode, scrx);
}

////
// Action format (Galaxy only now...)
////
/**
 * So far only used in Galaxy. Here we performs some stuff for the Action format
 */

bool CObject::getActionNumber(int16_t ActionNumber)
{	return (m_ActionNumber==ActionNumber);	}

bool CObject::getActionStatus(int16_t ActionNumber)
{	return (m_Action.getActionFormat(m_ActionBaseOffset + 30*ActionNumber));	}

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

// Sets the proper sprite of action format to the local object
void CObject::setActionSprite()
{
	if(m_hDir == LEFT || m_hDir == NONE)
		sprite = m_Action.Left_sprite-124;
	else if(m_hDir == RIGHT)
		sprite = m_Action.Right_sprite-124;
}

// This new function will setup the sprite based on the Action format
void CObject::processActionRoutine()
{
	setActionSprite();

	// Check the Movement Parameter
	/*
	 *	This is how the game handles the sprite's movement;
	 *	and is important only with sprites that have movement
	 *	not associated with animation (See below.)The variable
	 *	can have any value between 0-4; zero is used for
	 *	sprites that don't move (Most common occurrence.),
	 *	1 for nonanimating sprites that may still need to move or hover in place,
	 *	2 is the stunned sprite value and is also used where smooth movement is needed
	 *	3 is used for 'fall' moves such as the Bounder or Mad Mushroom and
	 *	4 is used for sprites that must hit or land on the ground.
	 */

	if( m_Action.Movement_parameter )
	{
		if(m_hDir == LEFT )
			moveLeft( m_Action.H_anim_move_amount<<1 );
		else if(m_hDir == RIGHT )
			moveRight( m_Action.H_anim_move_amount<<1 );

		if(m_vDir == UP)
			moveUp( m_Action.V_anim_move_amount<<1 );
		else if(m_vDir == DOWN)
			moveDown( m_Action.V_anim_move_amount<<1 );
	}

	if( m_ActionTicker > m_Action.Delay )
	{
		if( m_Action.Delay != 0 )
		{
			if(m_Action.Next_action != 0)
				m_Action.setNextActionFormat();
			else
				exists = false;
		}
		m_ActionTicker = 0;
	}
	else
	{
		m_ActionTicker += 2;
	}
}

////
// For drawing
////

// Functions finally draws the object also considering that there could be a masked
// or priority tile!
void CObject::draw()
{
	if( sprite == BLANKSPRITE || dontdraw )
		return;

	CSprite &Sprite = g_pGfxEngine->getSprite(sprite);
    SDL_Surface *sfc = g_pVideoDriver->getBlitSurface();

	scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
	scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	if( scrx < gameres.w && scry < gameres.h && exists )
	{
		Uint16 showX = scrx+Sprite.getXOffset();
		Uint16 showY = scry+Sprite.getYOffset();
		if(m_blinktime > 0)
		{
			Sprite.drawBlinkingSprite( sfc, showX, showY );
			m_blinktime--;
		}
		else
			Sprite.drawSprite( sfc, showX, showY, (255-transluceny) );
		hasbeenonscreen = true;

		#ifdef DEBUG_COLLISION
			// In this special section we draw the colision rectangle to do further checks. Only in this special Debug mode.
			SDL_Rect col_rect;

			col_rect.x = showX + (m_BBox.x1>>STC);
			col_rect.y = showY + (m_BBox.y1>>STC);
			col_rect.w = (m_BBox.x2-m_BBox.x1)>>STC;
			col_rect.h = (m_BBox.y2-m_BBox.y1)>>STC;

			SDL_FillRect(sfc, &col_rect, 0xFFFF0000);
		#endif

	}
}

///
// Cleanup Routine
///
CObject::~CObject() {
	if(m_number_of_objects > 0)
		m_number_of_objects--;
}

