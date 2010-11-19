/*
 * CItemEffect.cpp
 *
 *  Created on: 19.11.2010
 *      Author: gerstrong
 *
 *  This will animate the picked up items. It's basically the same
 *  as CRisingPoints in the Vorticon Engine.
 *  Here we have different cases, because some animations differ
 */

#include "CItemEffect.h"

namespace galaxy {

const int itemEffectTime = 50;

CItemEffect::CItemEffect(CMap *pmap, Uint32 x, Uint32 y, Uint16 l_sprite) :
CObject(pmap, x, y, OBJ_NONE)
{
	m_timer = 0;
	sprite = l_sprite-124;
	honorPriority = false;
	solid = false;
}

void CItemEffect::process()
{
	if(m_timer >= itemEffectTime)
		exists = false;

	moveUp(1<<STC);

	m_timer++;
}

}
