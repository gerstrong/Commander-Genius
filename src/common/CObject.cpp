/*
 * CObject.cpp
 *
 *  Created on: 17.05.2009
 *      Author: gerstrong
 */

#include "CObject.h"
#include "../CLogFile.h"
#include "../engine/spritedefines.h"
#include "../sdl/CVideoDriver.h"
#include "../keen.h"
#include <string.h>

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
	m_number_of_objects++;
	honorPriority = true;
	exists = true;
	solid = true;
	inhibitfall = false;

	this->x = x;
	this->y = y;
	canbezapped = false;
	onscreen = false;

	cansupportplayer = false;
	
    yinertia = 0;

	scrx = scry = 0;
	dead = false;
	hasbeenonscreen = false;
	honorPriority = true;
	touchPlayer = touchedBy = 0;
	cansupportplayer = false;
	dying = false;

	if(m_type != OBJ_NONE )
	{
		setupObjectType(g_pBehaviorEngine->getEpisode());

		setupinitialCollisions();
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

// This is needed when a new object is created, because the collision
// per tile, really checks per tile and not pixel based
void CObject::setupinitialCollisions()
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	blockedr = blockedl = false;
	blockedu = blockedd = false;

	if ( sprite != BLANKSPRITE )
	{
		CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
		bboxX1 = rSprite.m_bboxX1;		bboxX2 = rSprite.m_bboxX2;
		bboxY1 = rSprite.m_bboxY1;		bboxY2 = rSprite.m_bboxY2;

		// Check initial collision. This will avoid that ray go through the first blocking element
		for(size_t j=bboxY1; j<=bboxY2 ; j+=(1<<STC))
		{
			for(size_t i=bboxX1; i<=bboxX2 ; i+=(1<<STC))
			{
				blockedr |= TileProperty[mp_Map->at((x+i)>>CSF,(y+j)>>CSF)].bleft;
				blockedl |= TileProperty[mp_Map->at((x+i)>>CSF,(y+j)>>CSF)].bright;
				blockedu |= TileProperty[mp_Map->at((x+i)>>CSF,(y+j)>>CSF)].bdown;
				blockedd |= TileProperty[mp_Map->at((x+i)>>CSF,(y+j)>>CSF)].bup;
			}
		}
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

	// Also  Bullets
	if(m_type == OBJ_SNDWAVE || m_type == OBJ_RAY || m_type == OBJ_FIREBALL) return true;

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	Uint32 left = (((mp_Map->m_scrollx<<STC)-(visibility<<CSF))<0) ? 0 :
							(mp_Map->m_scrollx<<STC)-(visibility<<CSF);
	Uint32 right = ((mp_Map->m_scrollx+gameres.w)<<STC)+(visibility<<CSF);
	Uint32 up = (((mp_Map->m_scrolly<<STC)-(visibility<<CSF))<0) ? 0 :
							(mp_Map->m_scrolly<<STC)-(visibility<<CSF);
	Uint32 down = ((mp_Map->m_scrolly+gameres.h)<<STC)+(visibility<<CSF);

	if( right > x && left < x )
		if( down > y && up < y )
			return true;

	return false;
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
	int x1 = x + bboxX1;
	int y1 = y + bboxY1;
	int y2 = y + bboxY2;

	blockedr = false;

	if(force) {
		x -= amount;
		return;
	}

	if( y-amount < 0 )
		return;

	if(!solid)
	{
		blockedr = blockedl = false;
		blockedu = blockedd = false;
		x -= amount;
		return;
	}

	// check if we walked into other tiles
	int tile_x_old = (x1>>CSF)<<CSF;
	int tile_x_new = ((x1-amount)>>CSF)<<CSF;

	if( tile_x_old != tile_x_new ) // Did he pass the tile border?
	{ // Yes, we have to check the collision
		if(checkSolidL(x1-amount, y1, y2))
		{
			x = tile_x_old - bboxX1;
			blockedl = true;
		}
		else
		{
			x -= amount;
			blockedl = false;
		}
	}
	else
		x -= amount;

	x1 = x + bboxX1;
	int x2 = x + bboxX2;

	if(y2+1 == (((y2+1)>>CSF)<<CSF))
		blockedd = checkSolidD(x1, x2, y2+1);
	else
		blockedd = false;

	if(y1 == ((y1>>CSF)<<CSF))
		blockedu = checkSolidU(x1, x2, y1-1);
	else
		blockedu = false;

	if(!performSlopedTileDown(x, y2, -amount))
		performSlopedTileUp(x, y1, -amount);
}

void CObject::moveRight(int amount, bool force)
{
	int x2 = x + bboxX2;
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

	// check if we walked into other tiles
	int tile_x_old = (x2>>CSF)<<CSF;
	int tile_x_new = ((x2+amount)>>CSF)<<CSF;

	if( tile_x_old != tile_x_new ) // Did he pass the tile border?
	{ // Yes, we have to check the collision
		if(checkSolidR(x2+amount, y1, y2))
		{
			blockedr = true;
			x = tile_x_new - 1 - bboxX2;
		}
		else
		{
			blockedr = false;
			x += amount;
		}
	}
	else
		x += amount;

	int x1 = x + bboxX1 + 1;
	x2 = x + bboxX2 - 1;
	if(y2+1 == (((y2+1)>>CSF)<<CSF))
		blockedd = checkSolidD(x1, x2, y2+1);
	else
		blockedd = false;

	if(y1 == ((y1>>CSF)<<CSF))
		blockedu = checkSolidU(x1, x2, y1-1);
	else
		blockedu = false;

	if(!performSlopedTileDown(x, y2, amount));
		performSlopedTileUp(x, y1, amount);
}

void CObject::moveUp(int amount)
{
	int x1 = x + bboxX1;
	int x2 = x + bboxX2;
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

	blockedd = false;

	// check if we walked into other tiles
	int tile_y_old = (y1>>CSF)<<CSF;
	int tile_y_new = (((y1-amount)>>CSF))<<CSF;

	if( tile_y_old != tile_y_new ) // Did he pass the tile border?
	{ // Yes, we have to check the collision
		if(checkSolidU(x1, x2, y1-amount))
		{
			y = tile_y_old - bboxY1;
			blockedu = true;
		}
		else
		{
			y -= amount;
			blockedu = false;
		}
	}
	else
		y -= amount;

	y1 = y + bboxY1;
	int y2 = y + bboxY2;

	if(x2+1 == (((x2+1)>>CSF)<<CSF))
		blockedr = checkSolidR(x2+1,y1,y2);
	else
		blockedr = false;

	if(x1 == ((x1>>CSF)<<CSF))
		blockedl = checkSolidL(x1-1,y1,y2);
	else
		blockedl = false;
}

void CObject::moveDown(int amount)
{
	int x1 = x + bboxX1;
	int x2 = x + bboxX2;
	int y2 = y + bboxY2;

	blockedu = false;

	if(!solid)
	{
		blockedr = blockedl = false;
		blockedu = blockedd = false;
		y += amount;
		return;
	}

	// check if we walked into other tiles
	int tile_y_old = (y2>>CSF)<<CSF;
	int tile_y_new = (((y2+amount)>>CSF))<<CSF;

	if( tile_y_old != tile_y_new ) // Did he pass the tile border?
	{ // Yes, we have to check the collision
		if(checkSolidD(x1, x2, y2+amount))
		{
			y = tile_y_new - 1 - bboxY2;
			blockedd = true;
		}
		else
		{
			y += amount;
			blockedd = false;
		}
	}
	else
		y += amount;

	int y1 = y + bboxY1;
	y2 = y + bboxY2;

	if(x2+1 == (((x2+1)>>CSF)<<CSF))
		blockedr = checkSolidR(x2+1,y1,y2);
	else
		blockedr = false;

	if(x1 == ((x1>>CSF)<<CSF))
		blockedl = checkSolidL(x1-1,y1,y2);
	else
		blockedl = false;
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

// returns nonzero if object1 overlaps object2
bool CObject::hitdetect(CObject &hitobject)
{
	unsigned int rect1x1, rect1y1, rect1x2, rect1y2;
	unsigned int rect2x1, rect2y1, rect2x2, rect2y2;
	
	// get the bounding rectangle of the first object
	rect1x1 = x + bboxX1;
	rect1y1 = y + bboxY1;
	rect1x2 = x + bboxX2;
	rect1y2 = y + bboxY2;
	
	// get the bounding rectangle of the second object
	rect2x1 = hitobject.x + hitobject.bboxX1;
	rect2y1 = hitobject.y + hitobject.bboxY1;
	rect2x2 = hitobject.x + hitobject.bboxX2;
	rect2y2 = hitobject.y + hitobject.bboxY2;
	
	// find out if the rectangles overlap
	if ((rect1x1 <= rect2x1) && (rect1x2 <= rect2x1)) return false;
	if ((rect1x1 >= rect2x2) && (rect1x2 >= rect2x2)) return false;
	if ((rect1y1 <= rect2y1) && (rect1y2 <= rect2y1)) return false;
	if ((rect1y1 >= rect2y2) && (rect1y2 >= rect2y2)) return false;
	
	return true;
}

void CObject::processFalling()
{
	if(m_type == OBJ_MESSIE) return;

	// make object fall if it must
	const int OBJFALLSPEED = 160;

	// For jumping or flying objects. Yorps sometimes uses that for small jumps
	if(yinertia < 0)
		moveUp(-yinertia);

	if (!inhibitfall)
	{
		// So it reaches the maximum of fallspeed
		if(!blockedd)
		{
			if (yinertia < OBJFALLSPEED) yinertia+=4;
			else if(yinertia > OBJFALLSPEED) yinertia = OBJFALLSPEED;
		}
		else
		{
			if (yinertia > 0) yinertia-=4;
			else if(yinertia < 0) yinertia = 0;
		}

		// gradually increase the fall speed up to maximum rate
		moveDown(yinertia);
	}
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
				 theObject.kill();
}

const int COLISION_RES = 4;
bool CObject::checkSolidR( int x2, int y1, int y2)
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	// Check for right from the object
	if(solid)
	{
		for(int c=y1 ; c<=y2 ; c += COLISION_RES)
		{
			if(TileProperty[mp_Map->at(x2>>CSF, c>>CSF)].bleft)
				return true;
		}

		if(TileProperty[mp_Map->at(x2>>CSF, y2>>CSF)].bleft)
			return true;
	}

	if( m_type == OBJ_PLAYER )
	{
		if( x2 >= (int)((mp_Map->m_width-2)<<CSF) ) return true;
	}
	else
	{
		if( (Uint16)x2 > ((mp_Map->m_width)<<CSF) )
		{
			exists=false; // Out of map?
			return true;
		}
	}
	return false;
}

bool CObject::checkSolidL( int x1, int y1, int y2)
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	// Check for right from the object
	if(solid)
	{
		for(int c=y1 ; c<=y2 ; c += COLISION_RES)
		{
			if(TileProperty[mp_Map->at(x1>>CSF, c>>CSF)].bright)
				return true;
		}
	}

	if( m_type == OBJ_PLAYER )
	{
		if( x1 <= (2<<CSF) ) return true;
	}
	else
	{
		if( x1 == 0 )
		{
			exists=false; // Out of map?
			return true;
		}
	}

	return false;
}

bool CObject::checkSolidU(int x1, int x2, int y1)
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	// Check for right from the object
	if(solid)
	{
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			char blocked = TileProperty[mp_Map->at(c>>CSF, y1>>CSF)].bdown;

			if(blocked)
			{
				if(g_pBehaviorEngine->getEpisode() <= 3 or checkslopedU(c, y1, blocked))
					return true;
			}
		}
	}

	if( y1 <= ( ((m_type == OBJ_PLAYER) ? 2 : 1)<<CSF) )
		return true;

	return false;
}

bool CObject::checkSolidD( int x1, int x2, int y2 )
{
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	// Check for down from the object
	if(solid)
	{
		char blocked;
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			blocked = TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup;
			if(blocked)
			{
				if(g_pBehaviorEngine->getEpisode() <= 3 or checkslopedD(c, y2, blocked))
					return true;
			}
		}

		blocked = TileProperty[mp_Map->at((x2-(1<<STC))>>CSF, y2>>CSF)].bup;
		if(blocked)
		{
			if(g_pBehaviorEngine->getEpisode() <= 3 or checkslopedD(x2-(1<<STC), y2, blocked))
				return true;
		}
	}

	if( (Uint16)y2 > ((mp_Map->m_height)<<CSF) )
		exists=false; // Out of map?

	return false;
}

/**
 * So far only used in Galaxy. This is the code for sloped tiles downside
 *
 * 0	Fall through		1	Flat
 * 2	Top -> Middle		3	Middle -> bottom
 * 4	Top -> bottom		5	Middle -> top
 * 6	Bottom -> middle	7	Bottom -> top
 * 8	Unused			9	Deadly, can't land on in God mode
 */
bool CObject::performSlopedTileDown( int x, int y, int xspeed )
{
	if(g_pBehaviorEngine->getEpisode() <= 3)
		return true;

	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	char slope = TileProperty[mp_Map->at(x>>CSF, y>>CSF)].bup;

	if(slope == 2 or slope == 3)
		moveYDir(xspeed/2);
	else if(slope == 4)
		moveYDir(xspeed);
	else if(slope == 5)
		moveYDir(-xspeed);
	else if(slope == 6)
		moveYDir(-xspeed/2);
	else if(slope == 7)
		moveYDir(-xspeed);
	else
		return false;

	return true;
}

/**
 * So far only used in Galaxy. This is the code for sloped tiles upside
 *
 * 0	Jump through		1	Flat bottom
 * 2	Bottom-> Middle		3	Middle -> top
 * 4	Bottom -> top		5	Middle -> bottom
 * 6	Top -> middle		7	Top -> bottom
 *
 */
void CObject::performSlopedTileUp( int x, int y, int xspeed )
{
	if(g_pBehaviorEngine->getEpisode() <= 3)
		return;

	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	char slope = TileProperty[mp_Map->at(x>>CSF, y>>CSF)].bdown;

	if(slope == 2 or slope == 3)
		moveYDir(-xspeed/2);
	else if(slope == 4)
		moveYDir(-xspeed);
	else if(slope == 5 or slope == 6)
		moveYDir(xspeed/2);
	else if(slope == 7)
		moveYDir(xspeed);
}

/* Spezial slope function for galaxy maps
	0	Jump through		1	Flat bottom
	2	Bottom-> Middle		3	Middle -> top
	4	Bottom -> top		5	Middle -> bottom
	6	Top -> middle		7	Top -> bottom
 */
bool CObject::checkslopedU( int c, int y1, char blocked)
{
	int yb1, yb2;

	if( blocked == 2 )
		yb1 = 512,	yb2 = 256;
	else if( blocked == 3 )
		yb1 = 256,	yb2 = 0;
	else if( blocked == 4 )
		yb1 = 512,	yb2 = 0;
	else if( blocked == 5 )
		yb1 = 256,	yb2 = 512;
	else if( blocked == 6 )
		yb1 = 0,	yb2 = 256;
	else if( blocked == 7 )
		yb1 = 0,	yb2 = 512;
	else
		yb1 = 512,	yb2 = 512;

	int yh = (yb1+yb2)*(c%512)/512;
	return ( y1%512 < yh );
}

/* Spezial slope function for galaxy maps
0	Fall through		1	Flat
2	Top -> Middle		3	Middle -> bottom
4	Top -> bottom		5	Middle -> top
6	Bottom -> middle	7	Bottom -> top
8	Unused				9	Deadly, can't land on in God mode
 */
bool CObject::checkslopedD( int c, int y2, char blocked)
{
	int yb1, yb2;

	if( blocked == 2 )
		yb1 = 0,	yb2 = 256;
	else if( blocked == 3 )
		yb1 = 256,	yb2 = 512;
	else if( blocked == 4 )
		yb1 = 0,	yb2 = 512;
	else if( blocked == 5 )
		yb1 = 256,	yb2 = 0;
	else if( blocked == 6 )
		yb1 = 0,	yb2 = 256;
	else if( blocked == 7 )
		yb1 = 0,	yb2 = 512;
	else
		yb1 = 0,	yb2 = 0;

	int yh = (yb1+yb2)*(c%512)/512;
	return ( y2%512 > yh );
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

	    if (honorPriority)
	    {
	        // handle priority tiles and tiles with masks
	        // get the upper-left coordinates to start checking for tiles
    		drawMask(sfc, Sprite, (x>>CSF), (y>>CSF));
	    }
	}

	// Define the bouncing boxes again, because sprite could have changed meanwhile
	bboxX1 = Sprite.m_bboxX1;
	bboxX2 = Sprite.m_bboxX2;
	bboxY1 = Sprite.m_bboxY1;
	bboxY2 = Sprite.m_bboxY2;
}

////
// Functions finally draws the object also considering that there could be a masked
// or priority tile!
void CObject::drawMask(SDL_Surface *dst, CSprite &Sprite, int mx, int my)
{
	mp_Map->drawMaskonSprite( dst, mx, my, Sprite.getWidth(), Sprite.getHeight(), dead );
}


///
// Cleanup Routine
///
CObject::~CObject() {
	if(m_number_of_objects > 0)
		m_number_of_objects--;
}

