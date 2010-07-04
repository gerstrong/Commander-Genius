#include "CObjectAI.h"
#include "../../../sdl/sound/CSound.h"

// raygun blast, shot by keen, and by the tank robots in ep1&2.
#include "CRay.h"
#include "../../spritedefines.h"
#include "../../../common/CBehaviorEngine.h"
#include "../../../sdl/CVideoDriver.h"

#define Sprite g_pGfxEngine->Sprite

CRay::CRay(std::vector<CPlayer> &PlayerVect, CMap *p_map,
		bool automatic_raygun, char pShotSpeed, direction_t dir) :
CObject(p_map),
m_PlayerVect(PlayerVect),
mp_map(p_map),
m_automatic_raygun(automatic_raygun),
m_pShotSpeed(pShotSpeed),
m_Direction(dir)
{
	state = RAY_STATE_FLY;
	inhibitfall = true;
	needinit = false;
	checkinitialCollisions();
}

void CRay::process()
{
	int hitlethal;
	int rayspeed;
	CSprite &raysprite = g_pGfxEngine->getSprite(object.sprite);
	
	// shots from "fully automatic" raygun go faster
	if (sprite!=OBJ_RAY_DEFSPRITE_EP1 || !automatic_raygun)
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
	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
	
	std::vector<CObject*>::iterator it_obj;
	switch(state)
	{
		case RAY_STATE_FLY:

			// test if it hit a baddie. I hate that code! TODO: think about a way to reduce this
			for( it_obj = m_Objvect.begin() ; it_obj!=m_Objvect.end() ; it_obj++)
			{
				if( (*it_obj)->exists && ((*it_obj)->m_index != object.m_index || (*it_obj)->m_type == OBJ_ROPE) )
				{
					if ( (*it_obj)->onscreen)
					{
						if((*it_obj)->canbezapped || (*it_obj)->m_type == OBJ_RAY )
						{
							if ((*it_obj)->hitdetect(object) &&
								 shotbyplayer  )
							{
								state = RAY_STATE_SETZAPZOT;
								object.canbezapped = false;
								(*it_obj)->zapped++;
								if(g_pVideoDriver->getSpecialFXConfig())
								{
									bool allow_blink = false;

									// only enemies which can support multiple hits
									// will blink.
									allow_blink = ((*it_obj)->m_type == OBJ_MOTHER) ||
												((*it_obj)->m_type == OBJ_NINJA) ||
												((*it_obj)->m_type == OBJ_VORT) ||
												((*it_obj)->m_type == OBJ_VORTELITE);
									if(allow_blink)
										(*it_obj)->blink(10);
								}
								if((*it_obj)->m_type == OBJ_RAY)
									(*it_obj)->ai.ray.state = RAY_STATE_SETZAPZOT;
								(*it_obj)->zapd = (*it_obj)->ai.ray.direction;
								if (object.sprite==ENEMYRAY || object.sprite==ENEMYRAYEP2 || object.sprite==ENEMYRAYEP3)
									(*it_obj)->zappedbyenemy = true;
								else
									(*it_obj)->zappedbyenemy = false;

							}
						}
					}
				}
			}
			// check if ray hit keen. if canpk=0, only enemy rays can hurt keen
			if (touchPlayer)
			{
				if (m_Player[object.touchedBy].pfrozentime > PFROZEN_THAW && m_Episode==1)
				{
					// shot a frozen player--melt the ice
					m_Player[object.touchedBy].pfrozentime = PFROZEN_THAW;
					state = RAY_STATE_SETZAPZOT;
					object.canbezapped = false;
				}
				else
				{
					if(shotbyplayer != true)
					{
						m_Player[object.touchedBy].kill();
						state = RAY_STATE_SETZAPZOT;
						object.canbezapped = false;
					}
					else
					{ // still could be by another player
						if(m_Player[object.touchedBy].m_index != owner)
						{
							if (dontHitEnable==0 || dontHit!=OBJ_PLAYER)
							{
								m_Player[object.touchedBy].kill();
								state = RAY_STATE_SETZAPZOT;
								object.canbezapped = false;
							}
						}
					}
				}
			}

			if (direction == RIGHT)
			{
				// don't go through bonklethal tiles, even if they're not solid
				// (for the arms on mortimer's machine)
				if (TileProperties.at(mp_Map->at(((x>>(CSF-4))+raysprite.getWidth())>>4, (y>>CSF)+1)).behaviour == 1)
					hitlethal = true;
				else if (TileProperties.at(mp_Map->at(((x>>(CSF-4))+raysprite.getWidth())>>4, ((y>>(CSF-4))+(raysprite.getHeight()-1))>>(CSF-4))).behaviour == 1)
					hitlethal = true;
				else
					hitlethal = false;
				
				if (object.blockedr)
				{
					state = RAY_STATE_SETZAPZOT;
					object.canbezapped = false;
					if (object.onscreen)
						g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
				}
				object.moveRight(rayspeed);
			}
			else if (direction == LEFT)
			{
				if (TileProperties.at(mp_Map->at((x-1)>>CSF, (y+1)>>CSF)).behaviour == 1)
					hitlethal = true;
				else if (TileProperties.at(mp_Map->at((x-1)>>CSF, ((y>>(CSF-4))+(raysprite.getHeight()-1))>>(CSF-4))).behaviour == 1)
					hitlethal = true;
				else
					hitlethal = false;
				
				if (object.blockedl)
				{
					state = RAY_STATE_SETZAPZOT;
					object.canbezapped = false;
					if (object.onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
				}
				object.moveLeft(rayspeed);
			}
			else if (direction == DOWN)
			{
				if (object.blockedd || object.blockedu)
				{
					state = RAY_STATE_SETZAPZOT;
					object.canbezapped = false;
					if (object.onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
				}
				
				object.moveDown(rayspeed);
			}
			break;
		case RAY_STATE_SETZAPZOT:
			state = RAY_STATE_ZAPZOT;
			zapzottimer = RAY_ZAPZOT_TIME;
			
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
			
			if (direction==LEFT || direction==RIGHT)
				object.moveUp(2);
			else
				object.moveLeft(4);

			// ... and fall through
		case RAY_STATE_ZAPZOT:
			if (zapzottimer == 0) deleteObj(object);
			else zapzottimer--;
			break;
	}

}
