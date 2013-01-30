/*
 * autoray.cpp
 *
 *  Created on: 23.08.2009
 *      Author : Caitlin Shaw
 *      Port by: Gerstrong
 */

#include "engine/spritedefines.h"
#include "sdl/sound/CSound.h"
#include "common/CBehaviorEngine.h"
#include "CRay.h"
#include "CAutoRay.h"

const int SILENT_DIST = 20<<CSF;

CAutoRay::CAutoRay(CMap *pmap, Uint32 x, Uint32 y, stRayAlignment type) :
CVorticonSpriteObject(pmap, x, y, (type==HORIZONTAL) ? OBJ_AUTORAY : OBJ_AUTORAY_V),
m_type(type),
silent(false)
{
	sprite = BLANKSPRITE;
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
	if( (mp_Map->getAnimtiletimer()%GUNFIRE_TIMER) == 0 )
	{
		unsigned int x,y;
		CRay *NewRay;
		x = getXPosition();
		y = getYPosition();
        
		if (m_type==VERTICAL)
		{
			NewRay = new CRay(mp_Map, x+(4<<STC), y+(1<<CSF), CENTER, DOWN, OBJ_AUTORAY_V, m_index, 124);
			NewRay->sprite = RAY_VERT_EP3;
			NewRay->m_VertDir = DOWN;
		}
		else
		{
			NewRay = new CRay(mp_Map, x+(1<<CSF), y+(4<<STC), CENTER, DOWN, OBJ_AUTORAY, m_index, 124);
			NewRay->sprite = ENEMYRAYEP3;
			NewRay->m_HorDir = RIGHT;
		}
		g_pBehaviorEngine->EventList().spawnObj(NewRay);
        
		if(!silent)
		{
		    playSound(SOUND_TANK_FIRE);
		    silent = true;
		}		
	}
}



