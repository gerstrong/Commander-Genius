/*
 * autoray.cpp
 *
 *  Created on: 23.08.2009
 *      Author : Caitlin Shaw
 *      Port by: Gerstrong
 */

#include "../../spritedefines.h"
#include "../../../sdl/sound/CSound.h"
#include "CObjectAI.h"

void CObjectAI::autoray_ai(CObject &Object)
{
/*	if (Object.needinit)
	{
		Object.ai.se.timer = 0;
		Object.sprite = BLANKSPRITE;
		Object.inhibitfall = 1;
		Object.needinit = 0;
	}

	if (m_gunfiretimer == 0)
	{
		unsigned int x,y;
		CObject NewRay = new CObject(mp_Map);
		NewRay.ai.ray.owner = 0;
		x = Object.getXPosition();
		y = Object.getYPosition();

		if (Object.m_type==OBJ_AUTORAY_V)
		{
			NewRay.spawn(x+(4<<STC), y+(1<<CSF), OBJ_RAY, m_Episode);
			NewRay.sprite = RAY_VERT_EP3;
			NewRay.ai.ray.direction = DOWN;
		}
		else
		{
			NewRay.spawn(x+(1<<CSF), y+(4<<STC), OBJ_RAY, m_Episode);
			NewRay.sprite = ENEMYRAYEP3;
			NewRay.ai.ray.direction = RIGHT;
		}

		m_Objvect.push_back(NewRay);

		if (Object.onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, Object.scrx);
	}*/
}



