#include "CObjectAI.h"
#include "../../../sdl/sound/CSound.h"

// raygun blast, shot by keen, and by the tank robots in ep1&2.
#include "CRay.h"
#include "../../spritedefines.h"
#include "../../../common/CBehaviorEngine.h"
#include "../../../sdl/CVideoDriver.h"

#define Sprite g_pGfxEngine->Sprite

CRay::CRay(CMap *p_map, Uint32 x, Uint32 y,
		direction_t dir, object_t byType, size_t byID) :
CObject(p_map, x, y, OBJ_RAY),
m_Direction(dir)
{
	m_type = OBJ_RAY;
	owner.obj_type = byType;
	owner.ID = byID;

	size_t Episode = g_pBehaviorEngine->getEpisode();
	if(Episode == 1) sprite = OBJ_RAY_DEFSPRITE_EP1;
	else if(Episode == 2) sprite = OBJ_RAY_DEFSPRITE_EP2;
	else if(Episode == 3) sprite = OBJ_RAY_DEFSPRITE_EP3;

	this->x=x;
	this->y=y;

	state = RAY_STATE_FLY;
	inhibitfall = true;
	checkinitialCollisions();
}

void CRay::setOwner(object_t type, unsigned int index)
{
	owner.obj_type = type;
	owner.ID = index;
}

void CRay::process()
{
	int hitlethal;
	int rayspeed;
	CSprite &raysprite = g_pGfxEngine->getSprite(sprite);

	rayspeed = RAY_SPEED;
	
	int x = getXPosition();
	int y = getYPosition();
	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
	
	size_t Episode = g_pBehaviorEngine->getEpisode();
	std::vector<CObject*>::iterator it_obj;
	switch(state)
	{
		case RAY_STATE_FLY:

			if (m_Direction == RIGHT)
			{
				// don't go through bonklethal tiles, even if they're not solid
				// (for the arms on mortimer's machine)
				if (TileProperties.at(mp_Map->at(((x>>(CSF-4))+raysprite.getWidth())>>4, (y>>CSF)+1)).behaviour == 1)
					hitlethal = true;
				else if (TileProperties.at(mp_Map->at(((x>>(CSF-4))+raysprite.getWidth())>>4, ((y>>(CSF-4))+(raysprite.getHeight()-1))>>(CSF-4))).behaviour == 1)
					hitlethal = true;
				else
					hitlethal = false;
				
				if (blockedr)
				{
					state = RAY_STATE_SETZAPZOT;
					canbezapped = false;
					if (onscreen)
						g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
				}
				moveRight(rayspeed);
			}
			else if (m_Direction == LEFT)
			{
				if (TileProperties.at(mp_Map->at((x-1)>>CSF, (y+1)>>CSF)).behaviour == 1)
					hitlethal = true;
				else if (TileProperties.at(mp_Map->at((x-1)>>CSF, ((y>>(CSF-4))+(raysprite.getHeight()-1))>>(CSF-4))).behaviour == 1)
					hitlethal = true;
				else
					hitlethal = false;
				
				if (blockedl)
				{
					state = RAY_STATE_SETZAPZOT;
					canbezapped = false;
					if (onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
				}
				moveLeft(rayspeed);
			}
			else if (m_Direction == DOWN)
			{
				if (blockedd || blockedu)
				{
					state = RAY_STATE_SETZAPZOT;
					canbezapped = false;
					if (onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
				}
				moveDown(rayspeed);
			}
			break;
		case RAY_STATE_SETZAPZOT:
			state = RAY_STATE_ZAPZOT;
			zapzottimer = RAY_ZAPZOT_TIME;
			
			if (Episode==1)
			{
				if (rnd()&1)
				{ sprite = RAY_FRAME_ZAP_EP1; }
				else
				{ sprite = RAY_FRAME_ZOT_EP1; }
			}
			else if (Episode==2)
			{
				if (rnd()&1)
				{ sprite = RAY_FRAME_ZAP_EP2; }
				else
				{ sprite = RAY_FRAME_ZOT_EP2; }
			}
			else
			{
				if (rnd()&1)
				{ sprite = RAY_FRAME_ZAP_EP3; }
				else
				{ sprite = RAY_FRAME_ZOT_EP3; }
			}
			
			if (m_Direction==LEFT || m_Direction==RIGHT)
				moveUp(2);
			else
				moveLeft(4);

			// ... and fall through
		case RAY_STATE_ZAPZOT:
			if (zapzottimer == 0) exists=false;
			else zapzottimer--;
			break;
	}

}

void CRay::getTouchedBy(CObject &theObject)
{
	if(hitdetect(theObject))
	{
		if(theObject.canbezapped && state ==  RAY_STATE_FLY )
		{
			// Check, if it's not form the same object
			if(theObject.m_type != owner.obj_type)
			{
				state = RAY_STATE_SETZAPZOT;
				theObject.getShotByRay();
			}
		}
	}
}

bool CRay::isFlying()
{ return (state==RAY_STATE_FLY); }
