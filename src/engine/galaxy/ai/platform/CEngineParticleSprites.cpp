/*
 * CEngineParticleSprites.cpp
 *
 *  Created on: 10.07.2011
 *      Author: gerstrong
 */

#include "CEngineParticleSprites.h"

namespace galaxy {

const size_t PARTICLE_CHANGE_TIME = 5;

CEngineParticleSprites::CEngineParticleSprites(	CMap *pmap,
												const Uint16 foeID,
												const Uint32 x,
												const Uint32 y,
												const bool vert,
												const bool leftside ) :
CGalaxySpriteObject(pmap, foeID, x, y),
m_vert(vert),
m_timer(0)
{
	// They are never solid!!
	solid = false;

	// In case it's for a vertical platform
	if(m_vert)
	{
		if(leftside) // Left side of the particles look bit different than right side
		{
			sprite = 365;
			m_nextsprite = 366;
		}
		else
		{
			sprite = 363;
			m_nextsprite = 364;
		}
	}
	else // In case it's for a horizontal platform
	{
		if(leftside) // Left side of the particles donors draw priority so it appears behind the platform
			honorPriority = true;
		else
			honorPriority = false;
		sprite = 361;
		m_nextsprite = 362;
	}
}

void CEngineParticleSprites::swapSprites()
{
	const size_t temp_sprite = sprite;
	sprite = m_nextsprite;
	m_nextsprite = temp_sprite;
}

void CEngineParticleSprites::process()
{
	m_timer++;

	if(m_timer >= PARTICLE_CHANGE_TIME)
	{
		m_timer = 0;
		swapSprites();
	}
}

} /* namespace galaxy */
