/*
 * gotpoints.cpp
 *
 *  Created on: 23.08.2009
 *      Author: gerstrong
 */
#include "CRisingPoints.h"
#include "graphics/GsGraphics.h"

// GotPoints object (rising numbers when you get a bonus item)
// (this wasn't in original Keen)

#define GOTPOINTS_SPEED         16
#define GOTPOINTS_LIFETIME      25

CRisingPoints::CRisingPoints(CMap *p_map, Uint32 x, Uint32 y) :
CVorticonSpriteObject(p_map, x, y, OBJ_GOTPOINTS)
{
	offscreentime = GOTPOINTS_LIFETIME;
	inhibitfall = true;
	solid = false;
	honorPriority = false;
}


void CRisingPoints::process()
{
	moveUp(GOTPOINTS_SPEED);

	// delete it after it's existed for a certain amount of time
	if (!offscreentime)
	{
		transluceny+=10;

		if(transluceny >= 250)
		{
			exists = false;
			return;
		}
	}
	else offscreentime--;
}
