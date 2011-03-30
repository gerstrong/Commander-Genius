/*
 * CTantalusRay.cpp
 *
 *  Created on: 21.12.2009
 *      Author: gerstrong
 */

#include "CTantalusRay.h"
#include "graphics/effects/CVibrate.h"
#include "graphics/CGfxEngine.h"
#include "sdl/sound/CSound.h"
#include "sdl/CInput.h"
#include "common/CMapLoader.h"

#include "engine/vorticon/ai/CRay.h"
#include "engine/vorticon/ai/CEarthExplosion.h"
#include "engine/vorticon/ai/CEarthChunk.h"

unsigned int rnd(void);

const int TANTALUS_SPRITE = 58;
const int SHOT_SPD_X = (42*6);
const int SHOT_SPD_Y = (18*6);

const int EARTHCHUNK_BIG_UP = 64;
const int EARTHCHUNK_BIG_DN = 66;
const int EARTHCHUNK_SMALL_UP = 68;
const int EARTHCHUNK_SMALL_DN = 70;

CTantalusRay::CTantalusRay(CMap &Map, std::vector<CObject*> &vect_obj, CObjectAI &objectai) :
CFinale(Map),
m_mustsetup(true),
m_alternate_sprite(0),
mp_MessageBox(new CMessageBoxVort("Uh-Oh")),
m_vect_obj(vect_obj),
m_objectai(objectai),
m_timer(0),
mp_Bitmap(g_pGfxEngine->getBitmap("GAMEOVER")),
mp_process(&CTantalusRay::shootray)
{
	g_pGfxEngine->pushEffectPtr( new CVibrate(2000) );
}

void CTantalusRay::process()
{
	if(mp_MessageBox)
	{
		mp_MessageBox->process();

		if(mp_MessageBox->isFinished())
		{
			delete mp_MessageBox;
			mp_MessageBox = NULL;
		}
	}
	else
	{
		(this->*mp_process)();
	}
}

void CTantalusRay::shootray()
{
	if(m_mustsetup)
	{
		CMapLoader Maploader(&m_Map);
		Maploader.load(2,81, m_Map.m_gamepath, false);

		while(!m_vect_obj.empty())
		{
			delete m_vect_obj.back();
			m_vect_obj.pop_back();
		}

		m_Map.drawAll();

		CObject* ShootObject = new CRay(&m_Map, 4<<CSF, 4<<CSF, RIGHT, OBJ_NONE, 0);
		ShootObject->solid = false;
		ShootObject->exists = ShootObject->onscreen = true;
		m_vect_obj.push_back(ShootObject);
		mp_ShootObject = m_vect_obj.back();
		g_pSound->playSound(SOUND_KEEN_FIRE, PLAY_NOW);

		m_mustsetup = false;
	}
	else
	{
		mp_ShootObject->moveRight(SHOT_SPD_X);
		mp_ShootObject->moveDown(SHOT_SPD_Y);
		shot_x = mp_ShootObject->getXPosition();
		shot_y = mp_ShootObject->getYPosition();
		int x = (shot_x>>STC)-160;
		int y = (shot_y>>STC)-100;
		if( x>0 && y>0 )
			m_Map.gotoPos( x, y);

		mp_ShootObject->sprite = TANTALUS_SPRITE + m_alternate_sprite;
		m_alternate_sprite ^= 1;

		if( (shot_x>>CSF) >= 47)
		{
			m_vect_obj.pop_back();
			m_mustsetup = true;
			m_step = 0;
			mp_process = &CTantalusRay::explodeEarth;
		}
	}
}

void CTantalusRay::explodeEarth()
{
	if (!m_timer)
	{
		if (m_step<16)
		{
			CEarthExplosion *EarthExplosion;
			EarthExplosion = new CEarthExplosion(&m_Map,shot_x+((rnd()%32)<<STC), shot_y+((rnd()%32)<<STC)-(8<<STC));
			EarthExplosion->solid = false;
			m_vect_obj.push_back(EarthExplosion);
		}

		CEarthChunk *chunk;

		switch(m_step)
		{
		case 5:
			for(int i=0;i<=9;i++)
			{
				chunk = new CEarthChunk(&m_Map,shot_x+(14<<STC), shot_y);
				chunk->m_Direction = EC_UPLEFTLEFT;

				if (i > 4)
					chunk->sprite = (i > 4) ? EARTHCHUNK_SMALL_DN : EARTHCHUNK_SMALL_UP;
				m_vect_obj.push_back(chunk);
			}

			break;
		case 6:
			chunk = new CEarthChunk(&m_Map,shot_x+(16<<STC), shot_y+(16<<STC));
			m_vect_obj.push_back(chunk);
			break;
		case 7:
			chunk = new CEarthChunk(&m_Map,shot_x+(24<<STC), shot_y-(8<<STC));
			m_vect_obj.push_back(chunk);
		case 8:
			chunk = new CEarthChunk(&m_Map,shot_x+(16<<STC), shot_y+(4<<STC));
			m_vect_obj.push_back(chunk);
		case 10:
			// spawn four big fragments of the earth to go flying off
			chunk = new CEarthChunk(&m_Map,shot_x+(8<<STC), shot_y);
			chunk->m_Direction = EC_UPLEFT;
			chunk->sprite = EARTHCHUNK_BIG_UP;
			chunk->solid = false;
			m_vect_obj.push_back(chunk);

			chunk = new CEarthChunk(&m_Map,shot_x+(8<<STC), shot_y);
			chunk->m_Direction = EC_UPRIGHT;
			chunk->sprite = EARTHCHUNK_BIG_UP;
			chunk->solid = false;
			m_vect_obj.push_back(chunk);

			chunk = new CEarthChunk(&m_Map,shot_x+(8<<STC), shot_y);
			chunk->m_Direction = EC_DOWNRIGHT;
			chunk->sprite = EARTHCHUNK_BIG_DN;
			chunk->solid = false;
			m_vect_obj.push_back(chunk);

			chunk = new CEarthChunk(&m_Map,shot_x+(8<<STC), shot_y);
			chunk->m_Direction = EC_DOWNLEFT;
			chunk->sprite = EARTHCHUNK_BIG_DN;
			chunk->solid = false;
			m_vect_obj.push_back(chunk);

			// Hide the Earth!!!
			for(int ex = 0; ex<3 ; ex++)
				for(int ey = 0; ey<3 ; ey++)
					m_Map.changeTile((shot_x>>CSF)+ex, (shot_y>>CSF)+ey, 561);
			break;
		case 32:
			while(!m_vect_obj.empty())
			{
				delete m_vect_obj.back();
				m_vect_obj.pop_back();
			}
			m_mustfinishgame = true;
			break;
		}

		m_step++;
		m_timer = 5;
	}
	else m_timer--;

	m_objectai.process();
}

