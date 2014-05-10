/*
 * CSpriteItem.cpp
 *
 *  Created on: 24.11.2010
 *      Author: gerstrong
 */

#include "CSpriteItem.h"
#include "CItemEffect.h"
#include "CPlayerBase.h"
#include "sdl/audio/Audio.h"

namespace galaxy {


const int MAX_ANIMATION_TIMER = 20;

CSpriteItem::CSpriteItem(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, Uint32 sprite, const int sprVar, const bool gravity) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
mGravity(gravity)
{
	m_basesprite = sprite;
	m_timer = (y*256+x)%MAX_ANIMATION_TIMER;
	this->sprite = (m_timer%2 == 0) ? m_basesprite : m_basesprite+1;
	calcBoundingBoxes();
	inhibitfall = true;
	yinertia = -70;
    pmap->setSpriteOrigin(foeID, Vector2D<int>(x, y));
}

void CSpriteItem::process()
{
    if(mGravity)
    {
        performCollisions();
        performGravityHigh();
    }
    
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

void CSpriteItem::getTouchedBy(CSpriteObject &theObject)
{
	if( CPlayerBase* pPlayer = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		const int ep = g_pBehaviorEngine->getEpisode();
		stItemGalaxy &Item = pPlayer->m_Inventory.Item;
		
		int epOff = 0;
		if(ep == 5)
		{
		  epOff = -5;
		}
		else if(ep == 6)
		{
		  epOff = -1;
		}
		  
		int relBase = m_basesprite+epOff;		

		/// Calculate the right animation.
		// Point items
		Uint32 newanimsprite = got_sprite_item_pics[ep-4][0];
		if( relBase >= 103 && relBase <= 113 )
		{
			newanimsprite = got_sprite_item_pics[ep-4][4+(relBase-103)/2];
			switch(relBase)
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
		
		// Check for the extra life
		if ( relBase == 115 )
		{
			newanimsprite = got_sprite_item_pics[ep-4][10];
			Item.m_lifes++;
			g_pSound->playSound( SOUND_EXTRA_LIFE );
		}		
		
		// keycard (Keen 5 only)
		if( ep == 5 && m_basesprite >= 105 && m_basesprite <= 106 )
		{
			Item.m_keycards++;
			newanimsprite = 231;
			g_pSound->playSound( SOUND_GET_CARD );
		}
				

		// Now add the stuff to the inventory
		if(ep == 5)
		{
		  epOff = -4;
		}
		else
		{
		  epOff = 0;
		}
		  
		relBase = m_basesprite+epOff;
		
		// raygun
		if( relBase >= 127 && relBase <= 128 )
		{
			Item.m_bullets += 5;
			newanimsprite = got_sprite_item_pics[ep-4][11];
			g_pSound->playSound( SOUND_GET_AMMO );
		}
		

		if( relBase >= 118 && relBase <= 125 )
		{
			switch(relBase)
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
		
        spawnObj(new CItemEffect(mp_Map, 0, getXPosition(), getYPosition(), newanimsprite, FADEOUT));

		exists = false;
	}
}

}
