/*
 * CVorticonSpriteObject.cpp
 *
 *  Created on: 21.06.2012
 *      Author: gerstrong
 */

#include "CVorticonSpriteObject.h"
#include "engine/core/spritedefines.h"

#include <engine/core/CBehaviorEngine.h>

CVorticonSpriteObject::CVorticonSpriteObject(std::shared_ptr<CMap> pMap,
                                             const int x, const int y,
                                             const object_t type, const int sprVar) :
CSpriteObject(pMap, x, y, sprVar),
m_type(type),
touchPlayer(0),
touchedBy(0)
{
	if(m_type != OBJ_NONE )
	{
		setupObjectType(gBehaviorEngine.getEpisode());
		performCollisions();
	}

}

void CVorticonSpriteObject::setupObjectType(const int Episode)
{
	switch(m_type)
	{
	// Mainly Episode 1
	case OBJ_GARG: mSpriteIdx = OBJ_GARG_DEFSPRITE; break;
	case OBJ_BUTLER: mSpriteIdx = OBJ_BUTLER_DEFSPRITE; break;
	case OBJ_TANK: mSpriteIdx = OBJ_TANK_DEFSPRITE; break;
	case OBJ_ICECHUNK: mSpriteIdx = OBJ_ICECHUNK_DEFSPRITE; break;
	case OBJ_ICEBIT: mSpriteIdx = OBJ_ICEBIT_DEFSPRITE; break;
	case OBJ_ICECANNON: mSpriteIdx = OBJ_ICECHUNK_DEFSPRITE; break;
	case OBJ_ROPE: mSpriteIdx = OBJ_ROPE_DEFSPRITE; break;

	// Mainly Episode 2
	case OBJ_SCRUB: mSpriteIdx = OBJ_SCRUB_DEFSPRITE; break;
	case OBJ_GUARDROBOT: mSpriteIdx = OBJ_TANKEP2_DEFSPRITE; break;
	case OBJ_VORTELITE: mSpriteIdx = OBJ_VORTELITE_DEFSPRITE; break;
	case OBJ_SPARK: mSpriteIdx = OBJ_SPARK_DEFSPRITE_EP2; break;

	// Mainly Episode 3
	case OBJ_FOOB: mSpriteIdx = OBJ_FOOB_DEFSPRITE; break;
	case OBJ_NINJA: mSpriteIdx = OBJ_NINJA_DEFSPRITE; break;
	case OBJ_MOTHER: mSpriteIdx = OBJ_MOTHER_DEFSPRITE; break;
	case OBJ_MEEP: mSpriteIdx = OBJ_MEEP_DEFSPRITE; break;
	case OBJ_BALL: mSpriteIdx = OBJ_BALL_DEFSPRITE; break;
	case OBJ_JACK: mSpriteIdx = OBJ_JACK_DEFSPRITE; break;
	case OBJ_MESSIE: mSpriteIdx = OBJ_NESSIE_DEFSPRITE; break;
	case OBJ_AUTORAY_V: mSpriteIdx = RAY_VERT_EP3; break;
	case OBJ_SNDWAVE: mSpriteIdx = OBJ_SNDWAVE_DEFSPRITE; break;

	case OBJ_VORT:
		{
			if(Episode == 1) mSpriteIdx = OBJ_VORT_DEFSPRITE_EP1;
			else if(Episode == 2) mSpriteIdx = OBJ_VORT_DEFSPRITE_EP2;
			else if(Episode == 3) mSpriteIdx = OBJ_VORT_DEFSPRITE_EP3;
		}break;

	case OBJ_BABY:
	{
		if(Episode == 2) mSpriteIdx = OBJ_BABY_DEFSPRITE_EP2;
		else mSpriteIdx = OBJ_BABY_DEFSPRITE_EP3;
	}break;

	case OBJ_PLATFORM:
	case OBJ_PLATVERT:
	{
		if(Episode == 2) mSpriteIdx = OBJ_PLATFORM_DEFSPRITE_EP2;
		else mSpriteIdx = OBJ_PLATFORM_DEFSPRITE_EP3;
	}break;


	case OBJ_AUTORAY: {
		if(Episode == 1) mSpriteIdx = ENEMYRAY;
		else if(Episode == 2) mSpriteIdx = ENEMYRAYEP2;
		mSpriteIdx = ENEMYRAYEP3;
	}break;

	case OBJ_DOOR: mSpriteIdx = DOOR_YELLOW_SPRITE; break;
	case OBJ_TELEPORTER: mSpriteIdx = OBJ_TELEPORTER_DEFSPRITE; break;
	case OBJ_SECTOREFFECTOR: mSpriteIdx = BLANKSPRITE; break;
	case OBJ_GOTPOINTS: mSpriteIdx = PT500_SPRITE; break;
	default: mSpriteIdx = BLANKSPRITE; break;
	}

}



/**
 * This function will check if the enemy is in the limited scenario,
 * so it will triggered. Happens normally when the Object is seen on the screen.
 */
bool CVorticonSpriteObject::checkforScenario()
{
    // The player are handled at another section
	if ( !exists || m_type==OBJ_PLAYER ) return false;

    if( m_type==OBJ_EXPLOSION || m_type==OBJ_EARTHCHUNK ||
        m_type == OBJ_BRIDGE || m_type == OBJ_NONE ||
        m_type == OBJ_TELEPORTER || m_type == OBJ_MESSIE  ) return true;

	// Check if enemy is near enough. If he isn't, don't make him perform. Exception is on the map
	if(!mpMap->m_worldmap)
	{
		if(!calcVisibility()) return false;
	}

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



// This functions checks, if the enemy is near to the player. In case, that it is
// it will return true. Other case it will return false.
// This used for objects that only can trigger, when it's really worth to do so.
bool CVorticonSpriteObject::calcVisibility()
{
	// Platform are always active
	if( m_type == OBJ_PLATFORM || m_type == OBJ_PLATVERT )
		return true;

	// If an object is in the mid-air still moves it still,
	// until it gets stuck to ceiling, wall or floor
	if( !blockedd && m_type!=OBJ_SCRUB  ) return true;

	const bool inscreen = CSpriteObject::calcVisibility();

	// Bullets should disappear when offscreen
	if(m_type == OBJ_SNDWAVE || m_type == OBJ_RAY || m_type == OBJ_FIREBALL)
	{
		if(!inscreen)
			exists=false;
	}

	return inscreen;
}
