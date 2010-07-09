/*
 * autoray.cpp
 *
 *  Created on: 23.08.2009
 *      Author : Caitlin Shaw
 *      Port by: Gerstrong
 */

#include "../../spritedefines.h"
#include "../../../sdl/sound/CSound.h"
#include "CRay.h"
#include "CAutoRay.h"

CAutoRay::CAutoRay(CMap *pmap, Uint32 x, Uint32 y,
		std::vector<CObject*> &Objvect, stRayAlignment type) :
CObject(pmap, x, y, (type==HORIZONTAL) ? OBJ_AUTORAY : OBJ_AUTORAY_V),
timer(0),
m_gunfiretimer(0),
m_Objvect(Objvect),
m_type(type)
{
	sprite = BLANKSPRITE;
	inhibitfall = true;
}

void CAutoRay::process()
{
	if(m_gunfiretimer<GUNFIRE_TIMER) m_gunfiretimer++;
	else
	{
		m_gunfiretimer = 0;

		unsigned int x,y;
		CRay *NewRay;
		x = getXPosition();
		y = getYPosition();

		if (m_type==VERTICAL)
		{
			NewRay = new CRay(mp_Map, x+(4<<STC), y+(1<<CSF), DOWN, OBJ_AUTORAY_V, m_index);
			NewRay->sprite = RAY_VERT_EP3;
			NewRay->m_Direction = DOWN;
		}
		else
		{
			NewRay = new CRay(mp_Map, x+(1<<CSF), y+(4<<STC), DOWN, OBJ_AUTORAY_V, m_index);
			NewRay->sprite = ENEMYRAYEP3;
			NewRay->m_Direction = RIGHT;
		}
		m_Objvect.push_back(NewRay);

		g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, scrx);
	}
}



