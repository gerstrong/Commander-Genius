#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"

// raygun blast, shot by keen, and by the tank robots in ep1&2.
#include "ray.h"
#include "../spritedefines.h"

#define Sprite g_pGfxEngine->Sprite

void CObjectAI::ray_ai( CObject &object, bool automatic_raygun, char pShotSpeed )
{
	int hitlethal;
	int rayspeed;
	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;
	std::vector<CObject>::iterator it_obj;
	if (object.needinit)
	{
		object.ai.ray.state = RAY_STATE_FLY;
		object.inhibitfall = true;
		object.needinit = false;
		
		object.blockedr = object.blockedl = false;

		int x1 = Sprite.at(object.sprite).m_bboxX1;
		int x2 = Sprite.at(object.sprite).m_bboxX2;
		int y2 = Sprite.at(object.sprite).m_bboxY2;

		// Check initial collision. This will avoid that ray go through the first blocking element
		for(int i=x1; i<x2 ; i++)
		{
			if (TileProperty[mp_Map->at((object.x+i)>>CSF,(object.y+y2)>>CSF)].bleft)
				object.blockedr |= true;
			if (TileProperty[mp_Map->at((object.x+i)>>CSF,(object.y+y2)>>CSF)].bright)
				object.blockedl |= true;
		}


		// if we shoot directly up against a wall have
		// the ZAP appear next to the wall, not in it
		if (object.ai.ray.direction==RIGHT && object.blockedr)
		{
			object.x = (object.x >> CSF) << CSF;
			if (TileProperty[mp_Map->at(object.x>>CSF,object.y>>CSF)].bleft)
				object.x--;
			
			object.ai.ray.state = RAY_STATE_SETZAPZOT;
			
			if (object.onscreen)
				g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
		}
		else if (object.ai.ray.direction==LEFT && object.blockedl)
		{
			object.x = (object.x >> CSF) << CSF;
			if (TileProperty[mp_Map->at(object.x>>CSF,object.y>>CSF)].bright) object.x--;
			//if (tiles[getmaptileat(x>>CSF,y>>CSF)].solidl) x -= (16<<CSF);
			object.ai.ray.state = RAY_STATE_SETZAPZOT;
			if (object.onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
		}
	}
	
	// shots from "fully automatic" raygun go faster
	if (object.sprite!=OBJ_RAY_DEFSPRITE_EP1 || !automatic_raygun)
	{
		if (!pShotSpeed)
			rayspeed = RAY_SPEED;
		else
			rayspeed = pShotSpeed;
	}
	else
		rayspeed = RAY_AUTO_SPEED;
	
	
	switch(object.ai.ray.state)
	{
		case RAY_STATE_FLY:
			// test if it hit a baddie
			for( it_obj = m_Objvect.begin() ; it_obj!=m_Objvect.end() ; it_obj++)
			{
				if (it_obj->m_type==OBJ_RAY) continue;
				
				if (it_obj->canbezapped && it_obj->onscreen && it_obj->m_index != object.ai.ray.owner)
				{
					if (it_obj->hitdetect(object))
					{
						object.ai.ray.state = RAY_STATE_SETZAPZOT;
						it_obj->zapped++;
						it_obj->zapx = it_obj->x;
						it_obj->zapy = it_obj->y;
						it_obj->zapd = it_obj->ai.ray.direction;
						if (object.sprite==ENEMYRAY || object.sprite==ENEMYRAYEP2 || object.sprite==ENEMYRAYEP3)
							it_obj->zappedbyenemy = true;
						else
							it_obj->zappedbyenemy = false;
						
					}
				}
			}
			// check if ray hit keen. if canpk=0, only enemy rays can hurt keen
			if (object.touchPlayer)
			{
				if (m_Player[object.touchedBy].pfrozentime > PFROZEN_THAW && m_Episode==1)
				{
					// shot a frozen player--melt the ice
					m_Player[object.touchedBy].pfrozentime = PFROZEN_THAW;
					object.ai.ray.state = RAY_STATE_SETZAPZOT;
				}
				else
				{
					if (m_Player[object.touchedBy].m_player_number != object.ai.ray.owner)
					{
						if (object.ai.ray.dontHitEnable==0 || object.ai.ray.dontHit!=OBJ_PLAYER)
						{
							m_Player[object.touchedBy].kill();
							object.ai.ray.state = RAY_STATE_SETZAPZOT;
						}
					}
				}
			}
			
			if (object.ai.ray.direction == RIGHT)
			{
				// don't go through bonklethal tiles, even if they're not solid
				// (for the arms on mortimer's machine)
				if (TileProperty[mp_Map->at(((object.x>>(CSF-4))+Sprite[object.sprite].getWidth())>>4, (object.y>>CSF)+1)].behaviour == 1)
					hitlethal = true;
				else if (TileProperty[mp_Map->at(((object.x>>(CSF-4))+Sprite[object.sprite].getWidth())>>4, ((object.y>>(CSF-4))+(Sprite[object.sprite].getHeight()-1))>>(CSF-4))].behaviour == 1)
					hitlethal = true;
				else
					hitlethal = false;
				
				if (object.blockedr)
				{
					object.ai.ray.state = RAY_STATE_SETZAPZOT;
					if (object.onscreen)
						g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
				}
				
				object.x += rayspeed;
			}
			else if (object.ai.ray.direction == LEFT)
			{
				if (TileProperty[mp_Map->at((object.x-1)>>CSF, (object.y+1)>>CSF)].behaviour == 1)
					hitlethal = true;
				else if (TileProperty[mp_Map->at((object.x-1)>>CSF, ((object.y>>(CSF-4))+(Sprite[object.sprite].getHeight()-1))>>(CSF-4))].behaviour == 1)
					hitlethal = true;
				else
					hitlethal = false;
				
				if (object.blockedl)
				{
					object.ai.ray.state = RAY_STATE_SETZAPZOT;
					if (object.onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
				}
				
				object.x -= rayspeed;
			}
			else if (object.ai.ray.direction == DOWN)
			{
				if (object.blockedd || object.blockedu)
				{
					object.ai.ray.state = RAY_STATE_SETZAPZOT;
					if (object.onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
				}
				
				object.y += rayspeed;
			}
			break;
		case RAY_STATE_SETZAPZOT:
			object.ai.ray.state = RAY_STATE_ZAPZOT;
			object.ai.ray.zapzottimer = RAY_ZAPZOT_TIME;
			
			if (m_Episode==1)
			{
				if (rnd()&1)
				{ object.sprite = RAY_FRAME_ZAP_EP1; }
				else
				{ object.sprite = RAY_FRAME_ZOT_EP1; }
			}
			else if (m_Episode==2)
			{
				if (rnd()&1)
				{ object.sprite = RAY_FRAME_ZAP_EP2; }
				else
				{ object.sprite = RAY_FRAME_ZOT_EP2; }
			}
			else
			{
				if (rnd()&1)
				{ object.sprite = RAY_FRAME_ZAP_EP3; }
				else
				{ object.sprite = RAY_FRAME_ZOT_EP3; }
			}
			
			if (object.ai.ray.direction==LEFT || object.ai.ray.direction==RIGHT)
				object.y -= 2;
			else
				object.x -= 4;

			// ... and fall through
		case RAY_STATE_ZAPZOT:
			if (object.ai.ray.zapzottimer == 0) deleteObj(object);
			else object.ai.ray.zapzottimer--;
			break;
	}
}

