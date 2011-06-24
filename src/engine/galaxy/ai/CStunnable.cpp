/*
 * CStunnable.cpp
 *
 *  Created on: 23.06.2011
 *      Author: gerstrong
 *
 *  Those are in the galaxy Engine the stunnable objects
 *  Most of the enemies but not all foes can inherit this object
 *  and will get the stars when stunned or not, depending on how stunnable
 *  the enemies are...
 */

#include "CStunnable.h"
#include "CBullet.h"

namespace galaxy
{

CStunnable::CStunnable(	CMap *pmap,
						Uint32 x,
						Uint32 y,
						object_t type ):
CObject(pmap, x, y, type)
{}

void CStunnable::getTouchedBy(CObject &theObject)
{
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		theObject.setAction(A_KEENSHOT_IMPACT);
		theObject.playSound( SOUND_SHOT_HIT );
	}
}


void CStunnable::processStunned()
{

}

};
