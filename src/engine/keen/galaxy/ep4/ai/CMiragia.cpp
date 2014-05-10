/*
 * CMiragia.cpp
 *
 *  Created on: 31.07.2010
 *      Author: gerstrong
 */

#include "CMiragia.h"
#include "../../common/ai/CPlayerWM.h"

namespace galaxy
{

const Uint16 ANIMATION_TIME = 32;
const Uint16 WAIT_BETWEEN_FADE_TIME = 8;

CMiragia::CMiragia(CMap *pmap, const Uint16 foeID, const Vector2D<Uint32> &Location) :
CGalaxySpriteObject(pmap, foeID, Location.x, Location.y, 0),
m_step(0),
m_fade(FADE_IN),
m_waittime(WAIT_BETWEEN_FADE_TIME),
m_empty(true)
{
    solid = false;
    sprite = BLANKSPRITE;
    dontdraw = true;
}

/**
 * main process cycle in which the tiles are interchanged.
 */
void CMiragia::process()
{
	// Similar to the flag we use the animation timer and time to make it happen!
	if(mp_Map->getAnimtiletimer()%ANIMATION_TIME == 0)
	{
		if(m_waittime == 0)
		{
			// Now replace those tiles
			for(size_t x=0 ; x<6 ; x++)
			{
				for(size_t y=0 ; y<4 ; y++)
				{
					const Uint16 l_x = ((x+2+m_step*6)<<CSF);
					const Uint16 l_y = ((mp_Map->m_height+y-6)<<CSF);
					const Uint16 where_x = (getXPosition()>>CSF)+x;
					const Uint16 where_y = (getYPosition()>>CSF)+y;
					const Uint16 tile = mp_Map->getPlaneDataAt( 1, l_x, l_y );
					mp_Map->setTile(where_x, where_y, tile, true, 1);
					const Uint16 object = mp_Map->getPlaneDataAt( 2, l_x, l_y );
					mp_Map->setTile(where_x, where_y, object, false, 2);
					m_empty = false;
				}
			}

			if(m_step >= 4)
			{
				m_waittime = WAIT_BETWEEN_FADE_TIME;
				m_fade = FADE_OUT;
			}
			else if(m_step == 0)
			{
				m_waittime = WAIT_BETWEEN_FADE_TIME;
				m_fade = FADE_IN;
				m_empty = true;
			}

			if(m_fade == FADE_IN)
				m_step++;
			else
				m_step--;
		}
		else
		{
			m_waittime--;
		}
	}
}




void CMiragia::getTouchedBy(CSpriteObject &theObject)
{
	// Do that only if nothing of the tiles is seens (Miragia is invisible)
	if(m_empty)
	{
		// Is is touched by the player on the map?
		if( dynamic_cast<CPlayerWM*>(&theObject) )
		{
			// This will reset the Miragia Status time so it does not fade in
			m_waittime = WAIT_BETWEEN_FADE_TIME;
		}
	}
}

}
