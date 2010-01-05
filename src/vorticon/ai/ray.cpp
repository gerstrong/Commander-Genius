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
	CSprite &raysprite = g_pGfxEngine->getSprite(object.sprite);
	std::vector<CObject>::iterator it_obj;
	if (object.needinit)
	{
		object.ai.ray.state = RAY_STATE_FLY;
		object.inhibitfall = true;
		object.needinit = false;
		
		object.blockedr = object.blockedl = false;

		int y2 = raysprite.m_bboxY2;
		int x = object.getXPosition();
		int y = object.getYPosition();

		// Check initial collision. This will avoid that ray go through the first blocking element
		for(size_t i=raysprite.m_bboxX1; i<=raysprite.m_bboxX2+1 ; i++)
		{
			if (TileProperty[mp_Map->at((x+i)>>CSF,(y+y2)>>CSF)].bleft)
			{
				object.blockedr |= true;
				break;
			}
			if (TileProperty[mp_Map->at((x+i)>>CSF,(y+y2)>>CSF)].bright)
			{
				object.blockedl |= true;
				break;
			}
		}

		// if we shoot directly up against a wall have
		// the ZAP appear next to the wall, not in it
		if (object.blockedr || object.blockedl)
		{
			object.ai.ray.state = RAY_STATE_SETZAPZOT;
			g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
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
	
	int x = object.getXPosition();
	int y = object.getYPosition();
	
	switch(object.ai.ray.state)
	{
		case RAY_STATE_FLY:
			// test if it hit a baddie
			for( it_obj = m_Objvect.begin() ; it_obj!=m_Objvect.end() ; it_obj++)
			{
				if (it_obj->m_type==OBJ_RAY) continue;
				
				if (it_obj->canbezapped && it_obj->onscreen)
				{
					if (it_obj->hitdetect(object))
					{
						object.ai.ray.state = RAY_STATE_SETZAPZOT;
						it_obj->zapped++;
						it_obj->zapx = it_obj->getXPosition();
						it_obj->zapy = it_obj->getYPosition();
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
					if(object.ai.ray.shotbyplayer != true)
					{
						m_Player[object.touchedBy].kill();
						object.ai.ray.state = RAY_STATE_SETZAPZOT;
					}
					else
					{ // still could be by another player
						if(m_Player[object.touchedBy].m_player_number != object.ai.ray.owner)
						{
							if (object.ai.ray.dontHitEnable==0 || object.ai.ray.dontHit!=OBJ_PLAYER)
							{
								m_Player[object.touchedBy].kill();
								object.ai.ray.state = RAY_STATE_SETZAPZOT;
							}
						}
					}
				}
			}
			
			if (object.ai.ray.direction == RIGHT)
			{
				// don't go through bonklethal tiles, even if they're not solid
				// (for the arms on mortimer's machine)
				if (TileProperty[mp_Map->at(((x>>(CSF-4))+raysprite.getWidth())>>4, (y>>CSF)+1)].behaviour == 1)
					hitlethal = true;
				else if (TileProperty[mp_Map->at(((x>>(CSF-4))+raysprite.getWidth())>>4, ((y>>(CSF-4))+(raysprite.getHeight()-1))>>(CSF-4))].behaviour == 1)
					hitlethal = true;
				else
					hitlethal = false;
				
				if (object.blockedr)
				{
					object.ai.ray.state = RAY_STATE_SETZAPZOT;
					if (object.onscreen)
						g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
				}
				object.moveRight(rayspeed);
			}
			else if (object.ai.ray.direction == LEFT)
			{
				if (TileProperty[mp_Map->at((x-1)>>CSF, (y+1)>>CSF)].behaviour == 1)
					hitlethal = true;
				else if (TileProperty[mp_Map->at((x-1)>>CSF, ((y>>(CSF-4))+(raysprite.getHeight()-1))>>(CSF-4))].behaviour == 1)
					hitlethal = true;
				else
					hitlethal = false;
				
				if (object.blockedl)
				{
					object.ai.ray.state = RAY_STATE_SETZAPZOT;
					if (object.onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
				}
				object.moveLeft(rayspeed);
			}
			else if (object.ai.ray.direction == DOWN)
			{
				if (object.blockedd || object.blockedu)
				{
					object.ai.ray.state = RAY_STATE_SETZAPZOT;
					if (object.onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
				}
				
				object.moveDown(rayspeed);
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
				object.moveUp(2);
			else
				object.moveLeft(4);

			// ... and fall through
		case RAY_STATE_ZAPZOT:
			if (object.ai.ray.zapzottimer == 0) deleteObj(object);
			else object.ai.ray.zapzottimer--;
			break;
	}
}

