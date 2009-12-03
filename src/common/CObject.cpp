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

///
// Initialization Routine
///
CObject::CObject(int num_players) {
	honorPriority = false;
	exists = false;
	blockedu = blockedd = false;
	blockedl = blockedr = false;
	sprite=BLANKSPRITE;

	cansupportplayer.assign(num_players, false);
	
    yinertia = 0;
}

bool CObject::spawn(int x0, int y0, int otype)
{
	// find an unused object slot
	if (!exists && otype != OBJ_PLAYER)
	{
		new_x = x = x0;
		new_y = y = y0;
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
		
		setupObjectType();
		
		return true;
	}
	// object could not be created
	g_pLogFile->ftextOut("Object of type %d could not be created at %d,%d (out of object slots)<br>",otype,x,y);
	return false;
}

void CObject::setupObjectType()
{
	switch(m_type)
	{
		case OBJ_AUTORAY: sprite = ENEMYRAYEP3; break;
		case OBJ_AUTORAY_V: sprite = RAY_VERT_EP3; break;
		case OBJ_DOOR: sprite = DOOR_YELLOW_SPRITE; break;
		case OBJ_TELEPORTER: sprite = OBJ_TELEPORTER_DEFSPRITE; break;
		case OBJ_SECTOREFFECTOR: sprite = BLANKSPRITE; break;
		case OBJ_GOTPOINTS: sprite = PT500_SPRITE; break;
		case OBJ_YORP: sprite = OBJ_YORP_DEFSPRITE; break;
		case OBJ_GARG: sprite = OBJ_GARG_DEFSPRITE; break;
		case OBJ_BUTLER: sprite = OBJ_BUTLER_DEFSPRITE; break;
		case OBJ_TANK: sprite = OBJ_BUTLER_DEFSPRITE; break;
		case OBJ_ICECHUNK: sprite = OBJ_ICECHUNK_DEFSPRITE; break;
		case OBJ_ICEBIT: sprite = OBJ_ICEBIT_DEFSPRITE; break;
		case OBJ_ICECANNON: sprite = OBJ_ICECHUNK_DEFSPRITE; break;
		case OBJ_ROPE: sprite = OBJ_ROPE_DEFSPRITE; break;
		case OBJ_RAY: sprite = OBJ_RAY_DEFSPRITE_EP1; break;
		default: sprite = BLANKSPRITE; break;
	}

	/*if(Episode == 2)
		case OBJ_SCRUB: sprite = OBJ_SCRUB_DEFSPRITE; break;
		case OBJ_TANKEP2: sprite = OBJ_TANKEP2_DEFSPRITE; break;
		case OBJ_VORTELITE: sprite = OBJ_VORTELITE_DEFSPRITE; break;
	 
		case OBJ_RAY: sprite = OBJ_RAY_DEFSPRITE_EP2; break;
		case OBJ_VORT: sprite = OBJ_VORT_DEFSPRITE_EP2; break;
		case OBJ_PLATFORM: sprite = OBJ_PLATFORM_DEFSPRITE_EP2; break;
		case OBJ_BABY: sprite = OBJ_BABY_DEFSPRITE_EP2; break;
		case OBJ_SPARK: sprite = OBJ_SPARK_DEFSPRITE_EP2; break;
		default: sprite = BLANKSPRITE; break;
	else if(Episode == 3)
		case OBJ_FOOB: sprite = OBJ_FOOB_DEFSPRITE; break;
		case OBJ_NINJA: sprite = OBJ_NINJA_DEFSPRITE; break;
		case OBJ_MOTHER: sprite = OBJ_MOTHER_DEFSPRITE; break;
		case OBJ_MEEP: sprite = OBJ_MEEP_DEFSPRITE; break;
		case OBJ_BALL: sprite = OBJ_BALL_DEFSPRITE; break;
		case OBJ_JACK: sprite = OBJ_JACK_DEFSPRITE; break;
		case OBJ_NESSIE: sprite = OBJ_NESSIE_DEFSPRITE; break;
	 
		case OBJ_RAY: sprite = OBJ_RAY_DEFSPRITE_EP3; break;
		case OBJ_VORT: sprite = OBJ_VORT_DEFSPRITE_EP3; break;
		case OBJ_PLATFORM: sprite = OBJ_PLATFORM_DEFSPRITE_EP3; break;
		case OBJ_PLATVERT: sprite = OBJ_PLATFORM_DEFSPRITE_EP3; break;
		case OBJ_BABY: sprite = OBJ_BABY_DEFSPRITE_EP3; break;
		default: sprite = BLANKSPRITE; break;
	 		*/
}

void CObject::setScrPos( int px, int py )
{
	scrx = px;
	scry = py;
}


// returns nonzero if object1 overlaps object2
bool CObject::hitdetect(CObject &hitobject)
{
	unsigned int rect1x1, rect1y1, rect1x2, rect1y2;
	unsigned int rect2x1, rect2y1, rect2x2, rect2y2;
	
	// get the sprites used by the two objects
	CSprite &spr1 = *g_pGfxEngine->Sprite.at(sprite);
	CSprite &spr2 = *g_pGfxEngine->Sprite.at(hitobject.sprite);
	
	// get the bounding rectangle of the first object
	rect1x1 = x + spr1.m_bboxX1;
	rect1y1 = y + spr1.m_bboxY1;
	rect1x2 = x + spr1.m_bboxX2;
	rect1y2 = y + spr1.m_bboxY2;
	
	// get the bounding rectangle of the second object
	rect2x1 = hitobject.x + spr2.m_bboxX1;
	rect2y1 = hitobject.y + spr2.m_bboxY1;
	rect2x2 = hitobject.x + spr2.m_bboxX2;
	rect2y2 = hitobject.y + spr2.m_bboxY2;
	
	// find out if the rectangles overlap
	if ((rect1x1 < rect2x1) && (rect1x2 < rect2x1)) return false;
	if ((rect1x1 > rect2x2) && (rect1x2 > rect2x2)) return false;
	if ((rect1y1 < rect2y1) && (rect1y2 < rect2y1)) return false;
	if ((rect1y1 > rect2y2) && (rect1y2 > rect2y2)) return false;
	
	return true;
}

void CObject::performCollision(CMap *p_map)
{
long x1,y1,x2,y2;
stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
CSprite *Sprite = g_pGfxEngine->Sprite.at(sprite);

	// Set all blocking to false and test later.
	blockedu = blockedd = false;
	blockedl = blockedr = false;

	// Get Rect values of the object
	x1 = x + Sprite->m_bboxX1;
	y1 = y + Sprite->m_bboxY1;
	x2 = x + Sprite->m_bboxX2;
	y2 = y + Sprite->m_bboxY2;

	if( x > new_x )
	{
		do
		{
			if( checkSolidR(TileProperty, p_map, x2, y1, y2) )
			{
				blockedr = true;
				break;
			}
			new_x++;
		}while( x > new_x );
	}
	else if( x < new_x )
	{
		do
		{
			if( checkSolidL(TileProperty, p_map, x1, y1, y2) )
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
		do
		{
			if( checkSolidU(TileProperty, p_map, x1, x2, y1) )
			{
				blockedu = true;
				break;
			}
			new_y--;
		}while( y < new_y );
	}
	else if( y > new_y )
	{
		do
		{
			if( checkSolidD(TileProperty, p_map, x1, x2, y2) )
			{
				blockedd = true;
				break;
			}
			new_y++;
		}while( y > new_y );
	}
	y = new_y;
}

bool CObject::checkSolidR(stTile *TileProperty, CMap *p_map, int x2, int y1, int y2)
{
	// Check for right from the object
	for(int c=y1+1 ; c<=y2-(1<<STC) ; c++)
	{
		if(TileProperty[p_map->at(x2>>CSF, c>>CSF)].bleft)
			return true;
	}
	if( x2 > ((p_map->m_width-2)<<CSF) ) return true; // Out of map?

	return false;
}

bool CObject::checkSolidL(stTile *TileProperty, CMap *p_map, int x1, int y1, int y2)
{
	// Check for right from the object
	for(int c=y1+1 ; c<=y2-(1<<STC) ; c++)
	{
		if(TileProperty[p_map->at(x1>>CSF, c>>CSF)].bright)
			return true;
	}
	if( x1 < (2<<CSF) ) return true; // Out of map?

	return false;
}

bool CObject::checkSolidU(stTile *TileProperty, CMap *p_map, int x1, int x2, int y1)
{
	// Check for right from the object
	for(int c=x1+1 ; c<=x2-1 ; c++)
	{
		if(TileProperty[p_map->at(c>>CSF, y1>>CSF)].bdown)
			return true;
	}
	if( y1 < (2<<CSF) ) return true; // Out of map?

	return false;
}

bool CObject::checkSolidD(stTile *TileProperty, CMap *p_map, int x1, int x2, int y2)
{
	// Check for right from the object
	for(int c=x1+1 ; c<=x2-1 ; c++)
	{
		if(TileProperty[p_map->at(c>>CSF, y2>>CSF)].bup)
			return true;
	}
	if( y2 > ((p_map->m_height-2)<<CSF) ) return true; // Out of map?

	return false;
}

void CObject::processFalling()
{
	// make object fall if it must
	#define OBJFALLSPEED   160
	if (!inhibitfall)
	{
		if (blockedd) yinertia = 0;
		else
		{
			if (yinertia < OBJFALLSPEED) yinertia+=4;
			y += yinertia;
		}
	}
}


///
// Cleanup Routine
///
CObject::~CObject() {
}

