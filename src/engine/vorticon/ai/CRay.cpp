#include "sdl/sound/CSound.h"

// raygun blast, shot by keen, and by the tank robots in ep1&2.
#include "CRay.h"
#include "engine/spritedefines.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CVideoDriver.h"

#define Sprite g_pGfxEngine->Sprite

CRay::CRay(CMap *p_map, Uint32 x, Uint32 y,
		direction_t dir, object_t byType, size_t byID,
		size_t speed) :
CVorticonSpriteObject(p_map, x, y, OBJ_RAY),
m_Direction(dir),
m_speed(speed)
{
	m_type = OBJ_RAY;
	owner.obj_type = byType;
	owner.ID = byID;

	size_t Episode = g_pBehaviorEngine->getEpisode();
	if(Episode == 1) sprite = OBJ_RAY_DEFSPRITE_EP1;
	else if(Episode == 2) sprite = OBJ_RAY_DEFSPRITE_EP2;
	else if(Episode == 3) sprite = OBJ_RAY_DEFSPRITE_EP3;

	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
	m_BBox.x1 = rSprite.m_bboxX1;		m_BBox.x2 = rSprite.m_bboxX2;
	m_BBox.y1 = rSprite.m_bboxY1;		m_BBox.y2 = rSprite.m_bboxY2;

	state = RAY_STATE_FLY;
	inhibitfall = true;

	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	blockedl = TileProperty[mp_Map->at((m_Pos.x + m_BBox.x1)>>CSF, (m_Pos.y + (m_BBox.y1+m_BBox.y2)/2)>>CSF)].bright;
	blockedr = TileProperty[mp_Map->at((m_Pos.x + m_BBox.x2)>>CSF, (m_Pos.y + (m_BBox.y1+m_BBox.y2)/2)>>CSF)].bleft;

	if( blockedd || blockedr )
		getShotByRay(owner.obj_type);
}

void CRay::setOwner(object_t type, unsigned int index)
{
	owner.obj_type = type;
	owner.ID = index;
}

void CRay::setSpeed(size_t speed)
{	m_speed = speed; }

void CRay::process()
{
	std::vector<CVorticonSpriteObject*>::iterator it_obj;
	switch(state)
	{
		case RAY_STATE_FLY:
			moveinAir();
			break;
		case RAY_STATE_SETZAPZOT:
			setZapped();
			// ... and fall through
			break;
		case RAY_STATE_ZAPZOT:
			gotZapped();
			break;
	}
}

void CRay::setZapped()
{
	state = RAY_STATE_ZAPZOT;
	zapzottimer = RAY_ZAPZOT_TIME;

	size_t Episode = g_pBehaviorEngine->getEpisode();
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
}

void CRay::gotZapped()
{
	if (zapzottimer == 0) exists=false;
	else zapzottimer--;
}

void CRay::moveinAir()
{
	//std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
	//CSprite &raysprite = g_pGfxEngine->getSprite(sprite);
	//bool hitlethal; // TODO: Why is this here?

	if (m_Direction == RIGHT)
	{
		// don't go through bonklethal tiles, even if they're not solid
		// (for the arms on mortimer's machine)
		/*if (TileProperties.at(mp_Map->at(((getXPosition()>>(CSF-4))+raysprite.getWidth())>>4, (getYPosition()>>CSF)+1)).behaviour == 1)
			hitlethal = true;
		else if (TileProperties.at(mp_Map->at(((getXPosition()>>(CSF-4))+raysprite.getWidth())>>4, ((getYPosition()>>(CSF-4))+(raysprite.getHeight()-1))>>(CSF-4))).behaviour == 1)
			hitlethal = true;
		else
			hitlethal = false;*/

		if (blockedr)
		{
			state = RAY_STATE_SETZAPZOT;
			canbezapped = false;
			if (onscreen)
				playSound(SOUND_SHOT_HIT);
		}
		moveRight(m_speed);
	}
	else if (m_Direction == LEFT)
	{
		/*if (TileProperties.at(mp_Map->at((getXPosition()-1)>>CSF, (getYPosition()+1)>>CSF)).behaviour == 1)
			hitlethal = true;
		else if (TileProperties.at(mp_Map->at((getXPosition()-1)>>CSF, ((getYPosition()>>(CSF-4))+(raysprite.getHeight()-1))>>(CSF-4))).behaviour == 1)
			hitlethal = true;
		else
			hitlethal = false;*/

		if (blockedl)
		{
			state = RAY_STATE_SETZAPZOT;
			canbezapped = false;
			if (onscreen)
				playSound(SOUND_SHOT_HIT);
		}
		moveLeft(m_speed);
	}
	else if (m_Direction == DOWN)
	{
		if (blockedd || blockedu)
		{
			state = RAY_STATE_SETZAPZOT;
			canbezapped = false;
			if (onscreen)
				playSound(SOUND_SHOT_HIT);
		}
		moveDown(m_speed);
	}
}

void CRay::getTouchedBy(CVorticonSpriteObject &theObject)
{
	if( !theObject.dead && !theObject.dying )
	{
		if(theObject.canbezapped && state ==  RAY_STATE_FLY )
		{
			// Check, if it's not form the same object
			CVorticonSpriteObject *theObjPtr = dynamic_cast<CVorticonSpriteObject*>(&theObject);
			if(theObjPtr->m_type != owner.obj_type)
			{
				state = RAY_STATE_SETZAPZOT;
				canbezapped = false;
				theObject.getShotByRay(owner.obj_type);
			}
		}
	}
}

void CRay::getShotByRay(object_t &obj_type)
{
	state = RAY_STATE_SETZAPZOT;
	canbezapped = false;
}


bool CRay::isFlying()
{ return (state==RAY_STATE_FLY); }
