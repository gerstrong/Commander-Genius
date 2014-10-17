/*
 * CTantalusRay.cpp
 *
 *  Created on: 21.12.2009
 *      Author: gerstrong
 */

#include "CTantalusRay.h"
#include "graphics/effects/CVibrate.h"
#include "graphics/GsGraphics.h"
#include "sdl/audio/Audio.h"
#include <base/CInput.h>
#include "../CVorticonMapLoader.h"

#include "../ai/CRay.h"
#include "../ai/CEarthExplosion.h"
#include "../ai/CEarthChunk.h"

unsigned int rnd(void);

const int TANTALUS_SPRITE = 58;
const int SHOT_SPD_X = (42*6);
const int SHOT_SPD_Y = (18*6);

const int EARTH_COORD_X = 47;
const int EARTH_COORD_Y = 21;


const int EARTHCHUNK_BIG_UP = 64;
const int EARTHCHUNK_BIG_DN = 66;
const int EARTHCHUNK_SMALL_UP = 68;
const int EARTHCHUNK_SMALL_DN = 70;

CTantalusRay::CTantalusRay(std::list< std::shared_ptr<CMessageBoxVort> > &messageBoxes,
			    const std::shared_ptr<CMap> &pMap,
			    std::vector< std::unique_ptr<CVorticonSpriteObject> > &vect_obj,
			    std::shared_ptr<CVorticonSpriteObjectAI> &objectai) :
CFinale(messageBoxes, pMap, vect_obj),
m_mustsetup(true),
m_alternate_sprite(0),
mObjectAI(objectai),
m_timer(0),
mp_Bitmap(gGraphics.getBitmapFromStr("GAMEOVER")),
mp_process(&CTantalusRay::shootray)
{
    gEffectController.setupEffect( new CVibrate(2000) );
}

void CTantalusRay::ponder()
{
    (this->*mp_process)();

    mObjectAI->process();
}

void CTantalusRay::render()
{
    // TODO: What we need here now...
}

void CTantalusRay::shootray()
{
	if(m_mustsetup)
	{
		CVorticonMapLoaderBase Maploader(mpMap);
		Maploader.load(2,81, mpMap->m_gamepath, false);
		
		m_Object.clear();

		mpMap->drawAll();

        std::unique_ptr<CVorticonSpriteObject> shootObject( new CRay(mpMap.get(), 4<<CSF, 9<<CSF, RIGHT, CENTER, 0, OBJ_NONE, 0) );
		shootObject->solid = false;
		shootObject->exists = shootObject->onscreen = true;
		m_Object.push_back( move(shootObject) );
		g_pSound->playSound(SOUND_KEEN_FIRE, PLAY_NOW);

		m_mustsetup = false;
	}
	else
	{
	    CVorticonSpriteObject &shootObject = *m_Object.back();
	    shootObject.moveRight(SHOT_SPD_X);
	    shootObject.moveDown(SHOT_SPD_Y);
	    shot_x = shootObject.getXPosition();
	    shot_y = shootObject.getYPosition();
	    int x = (shot_x>>STC)-160;
	    int y = (shot_y>>STC)-100;
		if( x>0 && y>0 )
			mpMap->gotoPos(x, y);

		shootObject.sprite = TANTALUS_SPRITE + m_alternate_sprite;
		m_alternate_sprite ^= 1;

		if( (shot_x>>CSF) >= EARTH_COORD_X)
		{
			m_Object.pop_back();
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
	    std::unique_ptr<CEarthChunk> chunk;
	    
		if (m_step<16)
		{
		    std::unique_ptr<CEarthExplosion> expl( new CEarthExplosion(mpMap.get(),shot_x+((rnd()%32)<<STC), shot_y+((rnd()%32)<<STC)-(8<<STC)) );
		    expl->solid = false;
		    m_Object.push_back( move(expl) );
		}

		switch(m_step)
		{
		case 5:
			for(int i=0;i<=9;i++)
			{
				chunk.reset( new CEarthChunk(mpMap.get(),shot_x+(14<<STC), shot_y) );
				chunk->m_Direction = EC_UPLEFTLEFT;
				chunk->sprite = (i > 4) ? EARTHCHUNK_SMALL_DN : EARTHCHUNK_SMALL_UP;
				
				m_Object.push_back( move(chunk) );
			}

			break;
		case 6:
			chunk.reset( new CEarthChunk(mpMap.get(),shot_x+(16<<STC), shot_y+(16<<STC)) );
			m_Object.push_back( move(chunk) );
			break;
		case 7:
			chunk.reset( new CEarthChunk(mpMap.get(),shot_x+(24<<STC), shot_y-(8<<STC)) );
			m_Object.push_back( move(chunk) );
			break;
		case 8:
			chunk.reset( new CEarthChunk(mpMap.get(),shot_x+(16<<STC), shot_y+(4<<STC)) );
			m_Object.push_back( move(chunk) );
			break;
		case 10:
			// spawn four big fragments of the earth to go flying off
			chunk.reset(  new CEarthChunk(mpMap.get(),shot_x+(8<<STC), shot_y) );
			chunk->m_Direction = EC_UPLEFT;
			chunk->sprite = EARTHCHUNK_BIG_UP;
			chunk->solid = false;
			m_Object.push_back( move(chunk) );

			chunk.reset( new CEarthChunk(mpMap.get(),shot_x+(8<<STC), shot_y) );
			chunk->m_Direction = EC_UPRIGHT;
			chunk->sprite = EARTHCHUNK_BIG_UP;
			chunk->solid = false;
			m_Object.push_back( move(chunk) );

			chunk.reset( new CEarthChunk(mpMap.get(),shot_x+(8<<STC), shot_y) );
			chunk->m_Direction = EC_DOWNRIGHT;
			chunk->sprite = EARTHCHUNK_BIG_DN;
			chunk->solid = false;
			m_Object.push_back( move(chunk) );

			chunk.reset( new CEarthChunk(mpMap.get(),shot_x+(8<<STC), shot_y) );
			chunk->m_Direction = EC_DOWNLEFT;
			chunk->sprite = EARTHCHUNK_BIG_DN;
			chunk->solid = false;
			m_Object.push_back( move(chunk) );

			// Hide the Earth!!! Now it's destroyed
			for(int ex = 0; ex<4 ; ex++)
			{
				for(int ey = 0; ey<4 ; ey++)
				{
					mpMap->changeTile(EARTH_COORD_X+ex, EARTH_COORD_Y+ey, 561);
				}
			}
			break;
		case 32:
		    if(!m_Object.empty())
		    {
                m_Object.clear();
		    }
		    m_mustfinishgame = true;
		    break;
		}

		m_step++;
		m_timer = 5;
	}
	else m_timer--;

}

