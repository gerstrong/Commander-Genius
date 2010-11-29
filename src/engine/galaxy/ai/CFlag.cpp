/*
 * CFlag.cpp
 *
 *  Created on: 30.07.2010
 *      Author: gerstrong
 */

#include "CFlag.h"

namespace galaxy {

const Uint16 FLYING_BASEFRAME = 174;
const Uint16 WAVING_BASEFRAME = 181;

const Uint16 ANIMATION_TIME = 8;
const Uint16 SPEED = 64;

CFlag::CFlag(CMap *pmap, const VectorD2<Uint32> &Location,
			const VectorD2<Uint32> &Destination) :
CObject(pmap, Location.x, Location.y, OBJ_NONE),
m_location(Location),
m_destination(Destination),
m_baseframe(FLYING_BASEFRAME),
processState(&CFlag::processFlying)
{
	solid = false;
	honorPriority = false;


	// Here we move the coordinates in order get it positioned correctly in the pole
	m_destination.x += (6<<STC);
	CSprite &Sprite = g_pGfxEngine->getSprite(WAVING_BASEFRAME);
	sprite = WAVING_BASEFRAME;
	calcBouncingBoxes();
	m_destination.y -= Sprite.m_bboxY2;
	m_destination.y += (1<<STC);
	m_destination.y -= (6<<STC);
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
