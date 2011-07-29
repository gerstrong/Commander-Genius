/*
 * CSpriteItem.cpp
 *
 *  Created on: 24.11.2010
 *      Author: gerstrong
 */

#include "CSpriteItem.h"
#include "CItemEffect.h"
#include "CPlayerBase.h"
#include "sdl/sound/CSound.h"

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
	if( CPlayerBase* pPlayer = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		stItemGalaxy &Item = pPlayer->m_Inventory.Item;

		/// Calculate the right animation.
		// Point items
		Uint32 newanimsprite = got_sprite_item_pics[0];
		if( m_basesprite >= 103 && m_basesprite <= 116 )
		{
			newanimsprite = got_sprite_item_pics[4+(m_basesprite-103)/2];
			switch(m_basesprite)
			{
			case 103: Item.m_points += 100;	break;
			case 105: Item.m_points += 200;	break;
			case 107: Item.m_points += 500;	break;
			case 109: Item.m_points += 1000;	break;
			case 111: Item.m_points += 2000;	break;
			case 113: Item.m_points += 5000;	break;
			default: break;
			}
			g_pSound->playSound( SOUND_GET_BONUS );
		}

		// raygun
		if( m_basesprite >= 127 && m_basesprite <= 128 )
		{
			Item.m_bullets += 5;
			newanimsprite = got_sprite_item_pics[11];
			g_pSound->playSound( SOUND_GET_AMMO );
		}

		m_ObjectPtrs.push_back(new CItemEffect(mp_Map, getXPosition(), getYPosition(), newanimsprite));


		// Now add the stuff to the inventory

		if( m_basesprite >= 118 && m_basesprite <= 124 )
		{
			switch(m_basesprite)
			{
			case 118: case 119:
				Item.m_gem.red++;
				break;
			case 120: case 121:
				Item.m_gem.yellow++;
				break;
			case 122: case 123:
				Item.m_gem.blue++;
				break;
			case 124: case 125:
				Item.m_gem.green++;
				break;
			default:
				break;
			}
			g_pSound->playSound( SOUND_GET_GEM );
		}

		if ( m_basesprite == 115 )
		{
			Item.m_lifes++;
			g_pSound->playSound( SOUND_EXTRA_LIFE );
		}

		exists = false;
	}
}

}
