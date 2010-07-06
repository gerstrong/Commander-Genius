/*
 * gotpoints.cpp
 *
 *  Created on: 23.08.2009
 *      Author: gerstrong
 */
#include "CRisingPoints.h"

// GotPoints object (rising numbers when you get a bonus item)
// (this wasn't in original Keen)

#define GOTPOINTS_SPEED         16
#define GOTPOINTS_LIFETIME      25
#define YORPSHIELD_LIFETIME     20

CRisingPoints::CRisingPoints(CMap *p_map) :
CObject(p_map)
{}

void CRisingPoints::process()
{

}

/*
void CObjectAI::gotpoints_ai(CObject &Object)
{
	if (Object.needinit)
	{
		Object.ai.ray.offscreentime = GOTPOINTS_LIFETIME;
		Object.inhibitfall = true;
		Object.solid = false;
		Object.honorPriority = false;
		Object.needinit = 0;
	}

	Object.moveUp(GOTPOINTS_SPEED);

	// delete it after it's existed for a certain amount of time
	if (!Object.ai.ray.offscreentime)
	{
		deleteObj(Object);
		return;
	}
	else Object.ai.ray.offscreentime--;
}
*/
