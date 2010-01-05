/*
 * CObject.cpp
 *
 *  Created on: 17.05.2009
 *      Author: gerstrong
 */

#include "CObject.h"
#include "../CLogFile.h"
#include "../vorticon/spritedefines.h"
#include "../sdl/CVideoDriver.h"
#include "../graphics/CGfxEngine.h"
#include "../keen.h"
#include <string.h>

#define SAFE_DELETE_ARRAY(x) if(x) { delete [] x; x = NULL; }
#define SAFE_DELETE(x) if(x) { delete x; x = NULL; }

const int VISIBILITY = 21;

///
// Initialization Routine
///
CObject::CObject(CMap *pmap, int num_players, int index) :
m_index(index),
mp_object(NULL),
mp_Map(pmap)
{
	honorPriority = false;
	exists = false;
	blockedu = blockedd = false;
	blockedl = blockedr = false;
	sprite=BLANKSPRITE;
	solid = true;

	x = 0;
	y = 0;
	bboxX1 = bboxX2 = 0;
	bboxY1 = bboxY2 = 0;
	canbezapped = false;
	onscreen = false;

	memset(&ai, 0, sizeof(ai));

	cansupportplayer = false;
	
    yinertia = 0;
}

bool CObject::spawn(int x0, int y0, int otype, int Episode, char dirof)
{
	// find an unused object slot
	if (!exists)
	{
		x = x0;
		y = y0;
		m_type = otype;
		exists = true;
		needinit = true;
		dead = false;
		onscreen = false;
		hasbeenonscreen = false;
		zapped = false;
		canbezapped = 0;
		inhibitfall = false;
		honorPriority = true;
		touchPlayer = touchedBy = 0;
		cansupportplayer = false;
		
		setupObjectType(Episode);
		
		// check if the objects has to spawn left-off
		if(dirof == LEFT)
		{
			CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
			int dx = rSprite.m_bboxX2-rSprite.m_bboxX1;
			x-=dx;
		}

		return true;
	}

	// Check for collision points when object is spawn. Later only collision basing on movements will be checked
	// object could not be created
	g_pLogFile->ftextOut("Object of type %d could not be created at %d,%d (out of object slots)<br>",otype,x,y);
	return false;
}

void CObject::setIndex(int index)
{ m_index = index; }

void CObject::setupObjectType(int Episode)
{
	switch(m_type)
	{
	// Mainly Episode 1
	case OBJ_YORP: sprite = OBJ_YORP_DEFSPRITE; break;
	case OBJ_GARG: sprite = OBJ_GARG_DEFSPRITE; break;
	case OBJ_BUTLER: sprite = OBJ_BUTLER_DEFSPRITE; break;
	case OBJ_TANK: sprite = OBJ_BUTLER_DEFSPRITE; break;
	case OBJ_ICECHUNK: sprite = OBJ_ICECHUNK_DEFSPRITE; break;
	case OBJ_ICEBIT: sprite = OBJ_ICEBIT_DEFSPRITE; break;
	case OBJ_ICECANNON: sprite = OBJ_ICECHUNK_DEFSPRITE; break;
	case OBJ_ROPE: sprite = OBJ_ROPE_DEFSPRITE; break;

	// Mainly Episode 2
	case OBJ_SCRUB: sprite = OBJ_SCRUB_DEFSPRITE; break;
	case OBJ_TANKEP2: sprite = OBJ_TANKEP2_DEFSPRITE; break;
	case OBJ_VORTELITE: sprite = OBJ_VORTELITE_DEFSPRITE; break;
	case OBJ_SPARK: sprite = OBJ_SPARK_DEFSPRITE_EP2; break;

	// Mainly Episode 3
	case OBJ_FOOB: sprite = OBJ_FOOB_DEFSPRITE; break;
	case OBJ_NINJA: sprite = OBJ_NINJA_DEFSPRITE; break;
	case OBJ_MOTHER: sprite = OBJ_MOTHER_DEFSPRITE; break;
	case OBJ_MEEP: sprite = OBJ_MEEP_DEFSPRITE; break;
	case OBJ_BALL: sprite = OBJ_BALL_DEFSPRITE; break;
	case OBJ_JACK: sprite = OBJ_JACK_DEFSPRITE; break;
	case OBJ_NESSIE: sprite = OBJ_NESSIE_DEFSPRITE; break;
	case OBJ_AUTORAY_V: sprite = RAY_VERT_EP3; break;

	// Common Elements and some are Episode dependent
	case OBJ_RAY:
		{
			ai.ray.shotbyplayer = false;
			if(Episode == 1) sprite = OBJ_RAY_DEFSPRITE_EP1;
			else if(Episode == 2) sprite = OBJ_RAY_DEFSPRITE_EP2;
			else if(Episode == 3) sprite = OBJ_RAY_DEFSPRITE_EP3;
		}break;

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
bool CObject::calcVisibility( int player_x, int player_y )
{
	if(m_type == OBJ_PLATFORM || m_type == OBJ_PLATVERT) return true;

	// check in x
	Uint32 left = ((player_x-(VISIBILITY<<CSF))<0) ? 0 : player_x-(VISIBILITY<<CSF);
	Uint32 right = player_x+(VISIBILITY<<CSF);
	Uint32 up = ((player_y-(VISIBILITY<<CSF))<0) ? 0 : player_y-(VISIBILITY<<CSF);
	Uint32 down = player_y+(VISIBILITY<<CSF);

	if( right > x && left < x )
	{
		if( down > y && up < y )
		{
			return true;
		}
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

void CObject::moveXDir(int amount)
{
	if(amount<0)
		moveLeft(-amount);
	else if(amount>0)
		moveRight(amount);
}
void CObject::moveYDir(int amount)
{
	if(amount<0)
		moveUp(-amount);
	else if(amount>0)
		moveDown(amount);
}

void CObject::moveLeft(int amount)
{
	int x1 = x + bboxX1;
	int y1 = y + bboxY1;
	int y2 = y + bboxY2;

	blockedr = false;

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
}

void CObject::moveRight(int amount)
{
	int x2 = x + bboxX2;
	int y1 = y + bboxY1;
	int y2 = y + bboxY2;

	blockedl = false;

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
	if ((rect1x1 < rect2x1) && (rect1x2 < rect2x1)) return false;
	if ((rect1x1 > rect2x2) && (rect1x2 > rect2x2)) return false;
	if ((rect1y1 < rect2y1) && (rect1y2 < rect2y1)) return false;
	if ((rect1y1 > rect2y2) && (rect1y2 > rect2y2)) return false;
	
	return true;
}

void CObject::processFalling()
{
	if(m_type == OBJ_NESSIE) return;
	// make object fall if it must
	const int OBJFALLSPEED = 160;

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

const int COLISION_RES = 4;
bool CObject::checkSolidR( int x2, int y1, int y2)
{
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

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
		if( x2 >= ((mp_Map->m_width-2)<<CSF) ) return true;
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
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

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
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	// Check for right from the object
	if(solid)
	{
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			if(TileProperty[mp_Map->at(c>>CSF, y1>>CSF)].bdown)
				return true;
		}
	}

	if( m_type == OBJ_PLAYER )
	{
		if( y1 <= (2<<CSF) )
			return true;
	}
	else
	{
		if( y1 == 0 )
		{
			exists=false; // Out of map?
			return true;
		}
	}

	return false;
}

bool CObject::checkSolidD( int x1, int x2, int y2)
{
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	// Check for right from the object
	if(solid)
	{
		for(int c=x1 ; c<=x2 ; c += COLISION_RES)
		{
			if(TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup)
				return true;
		}

		if(x2>(1<<STC))
			if(TileProperty[mp_Map->at((x2-(1<<STC))>>CSF, y2>>CSF)].bup)
				return true;
	}

	if( (Uint16)y2 > ((mp_Map->m_height)<<CSF) )
		exists=false; // Out of map?

	return false;
}

void CObject::kill()
{
	if ( exists && zapped < 500 && canbezapped )
		zapped += 500;
}

////
// For drawing
////
// Functions finally draws the object also considering that there could be a masked
// or priority tile!
void CObject::draw()
{
	int mx, my;
	int tl,xsize,ysize;
	int xa,ya;

	CSprite &Sprite = g_pGfxEngine->getSprite(sprite);
    SDL_Surface *sfc = g_pVideoDriver->getBlitSurface();

	scrx = (x>>STC)-mp_Map->m_scrollx;
	scry = (y>>STC)-mp_Map->m_scrolly;

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	if(scry < gameres.w && scry < gameres.h && exists)
	{
		Sprite.drawSprite( sfc, scrx, scry );
		hasbeenonscreen = true;

	    if (honorPriority)
	    {
	        // handle priority tiles and tiles with masks
	        // get the upper-left coordinates to start checking for tiles
	        mx = (x>>CSF);
	        my = (y>>CSF);

	        // get the xsize/ysize of this sprite--round up to the nearest 16
	        xsize = ((Sprite.getWidth()>>4)<<4);
	        if (xsize != Sprite.getWidth()) xsize+=16;

	        ysize = ((Sprite.getHeight()>>4)<<4);
	        if (ysize != Sprite.getHeight()) ysize+=16;

	        tl = mp_Map->at(mx,my);
	        mx<<=4;
	        my<<=4;

	        // now redraw any priority/masked tiles that we covered up
	        // with the sprite
	        SDL_Rect sfc_rect;
	        sfc_rect.w = sfc_rect.h = 16;

	        for(ya=0;ya<=ysize;ya+=16)
	        {
				for(xa=0;xa<=xsize;xa+=16)
				{
					tl = mp_Map->at((mx+xa)>>4,(my+ya)>>4);
					if(mp_Map->mp_tiles[tl].behaviour == -2)
						mp_Map->drawAnimatedTile(sfc, mx+xa-mp_Map->m_scrollx, my+ya-mp_Map->m_scrolly, tl+1);
					else if (mp_Map->mp_tiles[tl].behaviour == -1)
						mp_Map->drawAnimatedTile(sfc, mx+xa-mp_Map->m_scrollx, my+ya-mp_Map->m_scrolly, tl);
				}
	        }
	    }
	}

	// Define the bouncing boxes again, because sprite could have changed meanwhile
	bboxX1 = Sprite.m_bboxX1;
	bboxX2 = Sprite.m_bboxX2;
	bboxY1 = Sprite.m_bboxY1;
	bboxY2 = Sprite.m_bboxY2;
}

///
// Cleanup Routine
///
CObject::~CObject() {
}

