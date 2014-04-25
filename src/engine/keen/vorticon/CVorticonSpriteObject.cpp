/*
 * CVorticonSpriteObject.cpp
 *
 *  Created on: 21.06.2012
 *      Author: gerstrong
 */

#include "CVorticonSpriteObject.h"
#include "engine/core/spritedefines.h"


CVorticonSpriteObject::CVorticonSpriteObject(CMap *pmap,
                                             Uint32 x, Uint32 y,
                                             object_t type, const int sprVar) :
CSpriteObject(pmap, x, y, sprVar),
m_index(m_number_of_objects),
m_type(type),
touchPlayer(0),
touchedBy(0)
{
	if(m_type != OBJ_NONE )
	{
		setupObjectType(g_pBehaviorEngine->getEpisode());
		performCollisions();
	}

}

void CVorticonSpriteObject::setupObjectType(const int Episode)
{
	switch(m_type)
	{
	// Mainly Episode 1
	case OBJ_GARG: sprite = OBJ_GARG_DEFSPRITE; break;
	case OBJ_BUTLER: sprite = OBJ_BUTLER_DEFSPRITE; break;
	case OBJ_TANK: sprite = OBJ_TANK_DEFSPRITE; break;
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
	default: sprite = BLANKSPRITE; break;
	}

}



/**
 * This function will check if the enemy is in the limited scenario,
 * so it will triggered. Happens normally when the Object is seen on the screen.
 */
bool CVorticonSpriteObject::checkforScenario()
{
	if ( !exists || m_type==OBJ_PLAYER ) return false;

	if( m_type==OBJ_EXPLOSION || m_type==OBJ_EARTHCHUNK
            || m_type == OBJ_BRIDGE || m_type == OBJ_NONE
            || m_type == OBJ_TELEPORTER  ) return true;

	// Check if enemy is near enough. If he isn't, don't make him perform. Exception is on the map
	if(!mp_Map->m_worldmap)
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
