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
#include "common/CBehaviorEngine.h"
#include "engine/galaxy/ai/CStarRing.h"


namespace galaxy
{

CStunnable::CStunnable(	CMap *pmap,
						Uint32 x,
						Uint32 y,
						object_t type ) :
CObject( pmap, x, y, type )
{
	m_invincible = false;
}

void CStunnable::getTouchedBy(CObject &theObject)
{
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		theObject.setAction(A_KEENSHOT_IMPACT);
		theObject.playSound( SOUND_SHOT_HIT );

		if( !m_invincible )
		{
			CSprite &StarRing = g_pGfxEngine->getSprite( STARRING_SPRITE );

			// Calculate the Position of the Star-Ring. Make it centered and above its head
			const Uint32 star_x = getXMidPos() - ( (StarRing.getWidth()<<STC)/2 );
			const Uint32 star_y = getYUpPos()  - ( StarRing.getHeight()<<STC );

			EventSpawnObject *Ev = new EventSpawnObject( new CStarRing(mp_Map, star_x, star_y, OBJ_NONE) );
			g_pBehaviorEngine->m_EventList.add( Ev );
		}
	}
}

void CStunnable::processStunned()
{ }

};
