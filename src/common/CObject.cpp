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

const int visibility = 21;

///
// Initialization Routine
///
CObject::CObject(CMap *pmap, int num_players, int index) :
m_index(index),
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

	cansupportplayer.assign(num_players, false);
	
    yinertia = 0;
}

bool CObject::spawn(int x0, int y0, int otype, int Episode)
{
	// find an unused object slot
	if (!exists && otype != OBJ_PLAYER)
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
		cansupportplayer.assign(cansupportplayer.size(), false);
		
		setupObjectType(Episode);
		
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
	// check in x
	Uint32 left = ((player_x-(visibility<<CSF))<0) ? 0 : player_x-(visibility<<CSF);
	Uint32 right = player_x+(visibility<<CSF);
	Uint32 up = ((player_y-(visibility<<CSF))<0) ? 0 : player_y-(visibility<<CSF);
	Uint32 down = player_y+(visibility<<CSF);

	if( right > x && left < x )
	{
		if( down > y && up < y )
		{
			return true;
		}
	}
	return false;
}

/*void CObject::moveTo(int new_x, int new_y)
{
	int check_x, check_y;
	amount_x = new_x-x;
	amount_y = new_y-y;

	if(amount_x < 0) // move left
		moveLeft(-amount_x);
	else if(amount_x > 0) // move right
		moveRight(amount_x);

	if(amount_y < 0) // means up
		moveUp(-amount_y);
	else if(amount_y < 0) // means down
		moveDown(amount_y);
}
void CObject::moveLeft(int amount)
{

}

void CObject::moveRight(int amount)
{
	int x1 = x + bboxX1;
	int x2 = x + bboxX2;
	int y1 = y + bboxY1;
	int y2 = y + bboxY2;

	if(checkSolidR(x2+amount, y1, y2))
	{
		do
		{
			amount_x--;
		} while(checkSolidR(x2+amount, y1, y2));
		blockedr = true;
	}
	else
		blockedr = false;

	x += amount;
}

void CObject::moveUp(int amount)
{}

void CObject::moveDown(int amount)
{}*/

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

void CObject::processFalling(CMap *p_map)
{
	if(m_type == OBJ_NESSIE) return;
	// make object fall if it must
	const int OBJFALLSPEED = 160;

	if (!inhibitfall)
	{
		// So it reaches the maximum of fallspeed
		if (yinertia < OBJFALLSPEED && !blockedd) yinertia+=4;
		else yinertia = 1;

		y+=yinertia;
	}
}

void CObject::performCollision(CMap *p_map)
{
	long x1,y1,x2,y2;

	if(m_type == OBJ_NESSIE) return;
	if(m_type == OBJ_SNDWAVE) return;
	//if(m_type == OBJ_EARTHCHUNK) return;

	// Get Rect values of the object
	x1 = x + bboxX1;
	y1 = y + bboxY1;
	x2 = x + bboxX2;
	y2 = y + bboxY2;

	// first the first col-model can't be applied to scrubs. There are very special
	/*if(m_type != OBJ_SCRUB)
	{
		// Set all blocking to false and test later.
		if( x > new_x )
		{
			blockedr = false;
			do
			{
				if( checkSolidR(TileProperty, p_map, x2, y1, y2) && solid )
				{
					blockedr = true;
					break;
				}
				new_x++;
			}while( x > new_x );
		}
		else if( x < new_x )
		{
			blockedl = false;
			do
			{
				if( checkSolidL(TileProperty, p_map, x1, y1, y2) && solid )
				{
					blockedl = true;
					break;
				}
				new_x--;
			}while( x < new_x );
		}
		x = new_x;

		if( y < new_y )
		{
			blockedu = false;
			do
			{
				if( checkSolidU(TileProperty, p_map, x1, x2, y1) && solid )
				{
					blockedu = true;
					break;
				}
				new_y--;
			}while( y < new_y );
		}
		else if( y > new_y )
		{
			blockedd = false;
			do
			{
				if( checkSolidD(TileProperty, p_map, x1, x2, y2) && solid )
				{
					blockedd = true;
					break;
				}
				new_y++;
			}while( y > new_y );
		}
		y = new_y;
	}
	else
	{*/
		blockedu = checkSolidU(x1, x2, y1);
		blockedd = checkSolidD(x1, x2, y2);
		blockedl = checkSolidL(x1, y1, y2);
		blockedr = checkSolidR(x2, y1, y2);

		if(m_type != OBJ_SCRUB)
		{
			if(blockedu)
				while(checkSolidU(x1, x2, y1+(1<<STC))){
					y++;
					y1 = y + bboxY1;
				}
			if(blockedd)
				while(checkSolidD(x1, x2, y2-(1<<STC))){
					y--;
					y2 = y + bboxY2;
				}
			if(blockedl)
				while(checkSolidL(x1-(1<<STC), y1, y2)){
					x++;
					x1 = x + bboxX1;
				}
			if(blockedr)
				while(checkSolidR(x2+(1<<STC), y1, y2)){
					x--;
					x2 = x + bboxX2;
				}
		}
	//}
}

const int COLISION_RES = 4;
bool CObject::checkSolidR( int x2, int y1, int y2)
{
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	// Check for right from the object
	if(solid)
	{
		for(int c=y1+(1<<STC) ; c<=y2-(1<<STC) ; c += COLISION_RES)
		{
			if(TileProperty[mp_Map->at(x2>>CSF, c>>CSF)].bleft)
				return true;
		}
	}
	if( (Uint16)x2 > ((mp_Map->m_width)<<CSF) ) exists=false; // Out of map?

	return false;
}

bool CObject::checkSolidL( int x1, int y1, int y2)
{
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	// Check for right from the object
	if(solid)
	{
		for(int c=y1+(1<<STC) ; c<=y2-(1<<STC) ; c += COLISION_RES)
		{
			if(TileProperty[mp_Map->at(x1>>CSF, c>>CSF)].bright)
				return true;
		}
	}
	if( x1 == 0 ) exists=false; // Out of map?

	return false;
}

bool CObject::checkSolidU( int x1, int x2, int y1)
{
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	// Check for right from the object
	if(solid)
	{
		for(int c=x1+(1<<STC) ; c<=x2-(1<<STC) ; c += COLISION_RES)
		{
			if(TileProperty[mp_Map->at(c>>CSF, y1>>CSF)].bdown)
				return true;
		}
	}
	if( y1 == 0 ) exists=false; // Out of map?

	return false;
}

bool CObject::checkSolidD( int x1, int x2, int y2)
{
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	// Check for right from the object
	if(solid)
	{
		for(int c=x1+(1<<STC) ; c<=x2-(1<<STC) ; c += COLISION_RES)
		{
			if(TileProperty[mp_Map->at(c>>CSF, y2>>CSF)].bup)
				return true;
		}
	}
	if( (Uint16)y2 > ((mp_Map->m_height)<<CSF) ) exists=false; // Out of map?

	return false;
}

void CObject::kill()
{
	if ( exists && zapped < 500 && canbezapped )
		zapped += 500;
}

///
// Cleanup Routine
///
CObject::~CObject() {
}

