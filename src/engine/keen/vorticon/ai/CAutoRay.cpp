/*
 * autoray.cpp
 *
 *  Created on: 23.08.2009
 *      Author : Caitlin Shaw
 *      Port by: Gerstrong
 */

#include "engine/core/spritedefines.h"
#include "sdl/audio/Audio.h"
#include "engine/core/CBehaviorEngine.h"
#include "CRay.h"
#include "CAutoRay.h"

const int SILENT_DIST = 20<<CSF;

CAutoRay::CAutoRay(CMap *pmap, Uint32 x, Uint32 y, stRayAlignment type) :
CVorticonSpriteObject(pmap, x, y, (type==HORIZONTAL) ? OBJ_AUTORAY : OBJ_AUTORAY_V),
m_type(type),
silent(false),
mGunTimer(0)
{
	mSpriteIdx = BLANKSPRITE;
	inhibitfall = true;
	hasbeenonscreen = true;

}

bool CAutoRay::isNearby(CVorticonSpriteObject &theObject)
{       
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
        int distx = player->getXPosition() - getXPosition();
        if(distx<0)
            distx = -distx;

        int disty = player->getYPosition() - getYPosition();
        if(disty<0)
            disty = -disty;

        if( disty < SILENT_DIST && distx < SILENT_DIST )
        {
            silent = false;
        }
    }

    return true;
}

void CAutoRay::process()
{
    if( (mGunTimer%GUNFIRE_TIMER) == 0 )
	{
		unsigned int x,y;
		CRay *NewRay;
		x = getXPosition();
		y = getYPosition();

		if (m_type==VERTICAL)
		{
            NewRay = new CRay(mpMap, x+(4<<STC), y+(1<<CSF), CENTER, DOWN, getSpriteVariantIdx(), OBJ_AUTORAY_V, m_index, 124);
			NewRay->mSpriteIdx = RAY_VERT_EP3;
			NewRay->m_VertDir = DOWN;
		}
		else
		{
            NewRay = new CRay(mpMap, x+(1<<CSF), y+(4<<STC), CENTER, DOWN, getSpriteVariantIdx(), OBJ_AUTORAY, m_index, 124);
			NewRay->mSpriteIdx = ENEMYRAYEP3;
			NewRay->m_HorDir = RIGHT;
		}
		spawnObj(NewRay);

		if(!silent)
		{
		    playSound(SOUND_TANK_FIRE);
		    silent = true;
		}		
	}

    mGunTimer++;
}



