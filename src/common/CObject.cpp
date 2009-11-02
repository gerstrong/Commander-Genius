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

///
// Initialization Routine
///
CObject::CObject() {
	honorPriority = false;
	exists = false;
	cansupportplayers = false;
	blockedu = blockedd = false;
	blockedl = blockedr = false;
	
    yinertia = 0;
}

bool CObject::spawn(int x0, int y0, int otype)
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
		cansupportplayers = false;
		touchPlayer = touchedBy = 0;
		
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
bool CObject::hitdetect(CObject *p_hitobject)
{
	CSprite *spr1, *spr2;
	unsigned int rect1x1, rect1y1, rect1x2, rect1y2;
	unsigned int rect2x1, rect2y1, rect2x2, rect2y2;
	
	// get the sprites used by the two objects
	spr1 = g_pGfxEngine->Sprite.at(sprite);
	spr2 = g_pGfxEngine->Sprite.at(p_hitobject->sprite);
	
	// get the bounding rectangle of the first object
	rect1x1 = x + spr1->m_bboxX1;
	rect1y1 = y + spr1->m_bboxY1;
	rect1x2 = x + spr1->m_bboxX2;
	rect1y2 = y + spr1->m_bboxY2;
	
	// get the bounding rectangle of the second object
	rect2x1 = p_hitobject->x + spr2->m_bboxX1;
	rect2y1 = p_hitobject->y + spr2->m_bboxY1;
	rect2x2 = p_hitobject->x + spr2->m_bboxX2;
	rect2y2 = p_hitobject->y + spr2->m_bboxY2;
	
	// find out if the rectangles overlap
	if ((rect1x1 < rect2x1) && (rect1x2 < rect2x1)) return false;
	if ((rect1x1 > rect2x2) && (rect1x2 > rect2x2)) return false;
	if ((rect1y1 < rect2y1) && (rect1y2 < rect2y1)) return false;
	if ((rect1y1 > rect2y2) && (rect1y2 > rect2y2)) return false;
	
	return true;
}

///
// Cleanup Routine
///
CObject::~CObject() {
	// TODO Auto-generated destructor stub
}

