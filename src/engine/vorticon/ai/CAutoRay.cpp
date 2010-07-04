/*
 * autoray.cpp
 *
 *  Created on: 23.08.2009
 *      Author : Caitlin Shaw
 *      Port by: Gerstrong
 */

#include "../../spritedefines.h"
#include "../../../sdl/sound/CSound.h"
#include "CAutoRay.h"

CAutoRay::CAutoRay(CMap *p_map, std::vector<CObject*> &Objvect, char episode, stRayAlignment type) :
CObject(p_map),
timer(0),
m_gunfiretimer(0),
m_Objvect(Objvect),
m_type(HORIZONTAL),
m_Episode(episode)
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
		CObject *NewRay = new CObject(mp_Map);
		NewRay->ai.ray.owner = 0;
		x = getXPosition();
		y = getYPosition();

		if (m_type==VERTICAL)
		{
			NewRay->spawn(x+(4<<STC), y+(1<<CSF), OBJ_RAY, m_Episode);
			NewRay->sprite = RAY_VERT_EP3;
			NewRay->ai.ray.direction = DOWN;
		}
		else
		{
			NewRay->spawn(x+(1<<CSF), y+(4<<STC), OBJ_RAY, m_Episode);
			NewRay->sprite = ENEMYRAYEP3;
			NewRay->ai.ray.direction = RIGHT;
		}
		m_Objvect.push_back(NewRay);

		g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, scrx);
	}
}



