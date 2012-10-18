/*
 * CFlag.cpp
 *
 *  Created on: 30.07.2010
 *      Author: gerstrong
 */

#include "CFlag.h"
#include "sdl/sound/CSound.h"


namespace galaxy {

const Uint16 FLYING_BASEFRAME = 174;

const Uint16 ANIMATION_TIME = 8;
const Uint16 SPEED = 64;

CFlag::CFlag(CMap *pmap, const VectorD2<Uint32> &Location,
			const VectorD2<Uint32> &Destination) :
CGalaxySpriteObject(pmap, FOE_ID, Location.x, Location.y),
m_location(Location),
m_destination(Destination),
m_baseframe(FLYING_BASEFRAME),
processState(&CFlag::processFlying)
{
	solid = false;
	honorPriority = false;
	sprite = WAVING_BASEFRAME;
	
	setActionSprite();	
	
	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);

	performCollisions();

	int moveup = (1<<CSF)-1;
	moveup -= ((rSprite.getHeight()+1)<<STC);
	m_Pos.y += moveup;
	processMove(0, 1);	
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
 * Called when flag is in the pole
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

}
