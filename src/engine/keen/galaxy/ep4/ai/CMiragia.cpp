/*
 * CMiragia.cpp
 *
 *  Created on: 31.07.2010
 *      Author: gerstrong
 */

#include "CMiragia.h"
#include "../../common/ai/CPlayerWM.h"

#include "engine/core/spritedefines.h"


namespace galaxy
{

const Uint16 ANIMATION_TIME = 32;
const Uint16 WAIT_BETWEEN_FADE_TIME = 8;

const Uint16 MIRAGIA_WIDTH = 6;
const Uint16 MIRAGIA_HEIGHT = 4;

CMiragia::CMiragia(std::shared_ptr<CMap> pMap, const Uint16 foeID, const GsVec2D<Uint32> &Location) :
CGalaxySpriteObject(pMap, foeID, Location.x, Location.y, 0),
m_fade(FADE_IN),
m_waittime(WAIT_BETWEEN_FADE_TIME)
{
    solid = false;
    mSpriteIdx = BLANKSPRITE;
    dontdraw = true;

    m_BBox.x2 = MIRAGIA_WIDTH << CSF;
    m_BBox.y2 = MIRAGIA_HEIGHT << CSF;
}

/**
 * main process cycle in which the tiles are interchanged.
 */
void CMiragia::process()
{
	// Similar to the flag we use the animation timer and time to make it happen!
	if(mpMap->getAnimtiletimer()%ANIMATION_TIME == 0)
	{
		if(m_waittime == 0)
		{
            // Now replace those tiles
            for(size_t x=0 ; x<6 ; x++)
            {
                for(size_t y=0 ; y<4 ; y++)
                {
                    const Uint16 l_x = ((x+2+m_step*6)<<CSF);
                    const Uint16 l_y = ((mpMap->m_height+y-6)<<CSF);
                    const Uint16 where_x = (getXPosition()>>CSF)+x;
                    const Uint16 where_y = (getYPosition()>>CSF)+y;
                    const Uint16 tile = mpMap->getPlaneDataAt( 1, l_x, l_y );
                    mpMap->setTile(where_x, where_y, tile, true, 1);
                    const Uint16 object = mpMap->getInfoData({l_x>>CSF, l_y>>CSF});
                    mpMap->setInfoTile( {where_x, where_y}, object );
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
