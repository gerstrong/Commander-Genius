/*
 * CSpriteItem.cpp
 *
 *  Created on: 24.11.2010
 *      Author: gerstrong
 */

#include "CSpriteItem.h"
#include "CItemEffect.h"

namespace galaxy {


const int MAX_ANIMATION_TIMER = 20;

CSpriteItem::CSpriteItem(CMap *pmap, Uint32 x, Uint32 y,
		std::vector<CObject*>& ObjectPtrs, Uint32 sprite) :
CObject(pmap, x, y, OBJ_NONE),
m_ObjectPtrs(ObjectPtrs)
{
	m_index = sprite;
	m_basesprite = sprite;
	m_timer = (y*256+x)%MAX_ANIMATION_TIMER;
	this->sprite = (m_timer%2 == 0) ? m_basesprite : m_basesprite+1;
	calcBouncingBoxes();
	inhibitfall = true;
}

void CSpriteItem::process()
{
	// Do the animation stuff here!
	if(m_timer == 0)
	{
		this->sprite = (m_basesprite == this->sprite) ? m_basesprite + 1 : m_basesprite;
	}

	if(m_timer < MAX_ANIMATION_TIMER)
		m_timer++;
	else
		m_timer = 0;

}

void CSpriteItem::getTouchedBy(CObject &theObject)
{
	if(hitdetect(theObject))
	{
		exists = false;

		Uint32 newanimsprite = got_sprite_item_pics[0];
		/// Calculate the right animation.
		// Point items
		if( m_basesprite >= 103 && m_basesprite <= 116 )
			newanimsprite = got_sprite_item_pics[4+(m_basesprite-103)/2];

		// raygun
		if( m_basesprite >= 127 && m_basesprite <= 128 )
			newanimsprite = got_sprite_item_pics[11];

		m_ObjectPtrs.push_back(new CItemEffect(mp_Map, getXPosition(), getYPosition(), newanimsprite));
	}
}

}
