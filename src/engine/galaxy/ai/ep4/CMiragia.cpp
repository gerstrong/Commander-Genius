/*
 * CMiragia.cpp
 *
 *  Created on: 31.07.2010
 *      Author: gerstrong
 */

#include "CMiragia.h"

namespace galaxy {

const Uint16 ANIMATION_TIME = 32;
const Uint16 WAIT_BETWEEN_FADE_TIME = 8;

CMiragia::CMiragia(CMap *pmap, const VectorD2<Uint32> &Location) :
CObject(pmap, Location.x, Location.y, OBJ_NONE),
m_step(0),
m_fade(FADE_IN),
m_waittime(WAIT_BETWEEN_FADE_TIME)
{}

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
				}
			}

			if(m_step >= 4)
			{
				m_waittime = WAIT_BETWEEN_FADE_TIME;
				m_fade = FADE_OUT;
			}
			if(m_step == 0)
			{
				m_waittime = WAIT_BETWEEN_FADE_TIME;
				m_fade = FADE_IN;
			}

			if(m_fade == FADE_IN)
				m_step++;
			else
				m_step--;
		}
		else
			m_waittime--;
	}
}

}
