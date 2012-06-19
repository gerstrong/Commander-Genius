/*
 * autoray.cpp
 *
 *  Created on: 23.08.2009
 *      Author : Caitlin Shaw
 *      Port by: Gerstrong
 */

#include "engine/spritedefines.h"
#include "sdl/sound/CSound.h"
#include "CRay.h"
#include "CAutoRay.h"

CAutoRay::CAutoRay(CMap *pmap, Uint32 x, Uint32 y,
		std::vector<CSpriteObject*> &Objvect, stRayAlignment type) :
CSpriteObject(pmap, x, y, (type==HORIZONTAL) ? OBJ_AUTORAY : OBJ_AUTORAY_V),
m_Objvect(Objvect),
m_type(type)
{
	sprite = BLANKSPRITE;
	inhibitfall = true;
	hasbeenonscreen = true;
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
			NewRay = new CRay(mp_Map, x+(4<<STC), y+(1<<CSF), DOWN, OBJ_AUTORAY_V, m_index, 124);
			NewRay->sprite = RAY_VERT_EP3;
			NewRay->m_Direction = DOWN;
		}
		else
		{
			NewRay = new CRay(mp_Map, x+(1<<CSF), y+(4<<STC), DOWN, OBJ_AUTORAY, m_index, 124);
			NewRay->sprite = ENEMYRAYEP3;
			NewRay->m_Direction = RIGHT;
		}
		m_Objvect.push_back(NewRay);

		playSound(SOUND_TANK_FIRE);
	}
}



