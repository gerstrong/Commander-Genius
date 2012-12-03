/*
 * CFlag.cpp
 *
 *  Created on: 30.07.2010
 *      Author: gerstrong
 */

#include "CFlag.h"
#include "sdl/sound/CSound.h"
#include "common/CBehaviorEngine.h"


namespace galaxy {

const Uint16 FLYING_BASEFRAME_EP4 = 174;
const Uint16 FLYING_BASEFRAME_EP5 = 173;

const Uint16 ANIMATION_TIME = 8;
const Uint16 SPEED = 64;

CFlag::CFlag(CMap *pmap, const VectorD2<Uint32> &Location,
			const VectorD2<Uint32> &Destination) :
CGalaxySpriteObject(pmap, FOE_ID, Location.x, Location.y),
m_location(Location),
m_destination(Destination),
m_baseframe(FLYING_BASEFRAME_EP4),
processState(&CFlag::processFlying)
{
	solid = false;
	honorPriority = false;
	sprite = WAVING_BASEFRAME;
	
	alignToTile();
	
	if(g_pBehaviorEngine->getEpisode() == 5)
	{
	    // In Episode 5 the sign is not thrown! It just appears in the holder
	    //m_location = m_destination;
	    moveTo(m_destination);
	    processState = &CFlag::processRotation;
	    m_baseframe = FLYING_BASEFRAME_EP5;
	    sprite = m_baseframe;
	}
	
}

/**
 * Main process cycle
 */
void CFlag::process()
{
	(this->*processState)();
}

/**
 * Called when Flag is flying to the pole
 */
void CFlag::processFlying()
{
	if(mp_Map->getAnimtiletimer()%ANIMATION_TIME == 0)
	{
		if(sprite-m_baseframe >= 4)
			sprite = m_baseframe;
		else
			sprite++;
	}

	if(m_destination != m_location)
	{
		VectorD2<int> dir = m_destination - m_location;
		float length = dir.GetLength();
		VectorD2<float> base_dir( dir.x/length, dir.y/length );

		if( fabs(length) < SPEED )
		{
			moveTo(m_destination);
		}
		else
			moveDir(base_dir*SPEED);

		m_location.x = getXPosition();
		m_location.y = getYPosition();
	}
	else
	{
		processState = &CFlag::processWaving;
		m_baseframe = WAVING_BASEFRAME;
		g_pSound->playSound( SOUND_FLAG_LAND );
	}
}

/*
 * Called when flag is in the pole. Keen 4 or 6
 */
void CFlag::processWaving()
{
	if(mp_Map->getAnimtiletimer()%ANIMATION_TIME == 0)
	{
		if(sprite-m_baseframe >= 3)
			sprite = m_baseframe;
		else
			sprite++;
	}
}

/*
 * Called when sign is in the holder. Keen 5 normally
 */
void CFlag::processRotation()
{ 
	if(mp_Map->getAnimtiletimer()%ANIMATION_TIME == 0)
	{
		if(sprite-m_baseframe >= 3)
			sprite = m_baseframe;
		else
			sprite++;
	}    
}

}
