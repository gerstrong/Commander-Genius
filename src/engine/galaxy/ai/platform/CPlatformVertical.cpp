/*
 * CPlatformVertical.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformVertical.h"
#include "common/CBehaviorEngine.h"

// Vertical platform speed
const int MOVE_VERT_SPEED = 20;

namespace galaxy
{

CPlatformVertical::CPlatformVertical(CMap *pmap, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pmap, x, y),
CPlatform(pmap, x, y)
{
	m_hDir = NONE;
	m_vDir = DOWN;
	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBoundingBoxes();

	// Setup boost effects
	mp_BoostEngObjLeft = new CEngineParticleSprites(mp_Map, x+(1<<STC), y+(8<<STC), true, true);
	mp_BoostEngObjRight = new CEngineParticleSprites(mp_Map, x+m_BBox.x2+(4<<STC), y+(8<<STC), true, false);

	g_pBehaviorEngine->m_EventList.add( new EventSpawnObject( mp_BoostEngObjLeft ) );
	g_pBehaviorEngine->m_EventList.add( new EventSpawnObject( mp_BoostEngObjRight ) );
}

void CPlatformVertical::process()
{
	const Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());

	// If there is a blocker, change the direction
	if( object == 31 )
		m_vDir = (m_vDir == UP) ? DOWN : UP;

	if(m_vDir == UP && blockedu)
		m_vDir = DOWN;
	else if(m_vDir == DOWN && blockedd)
		m_vDir = UP;

	if(m_vDir == UP)
	{
		mp_BoostEngObjLeft->moveUp(MOVE_VERT_SPEED);
		mp_BoostEngObjRight->moveUp(MOVE_VERT_SPEED);
		movePlatUp(MOVE_VERT_SPEED);
	}
	else
	{
		mp_BoostEngObjLeft->moveDown(MOVE_VERT_SPEED);
		mp_BoostEngObjRight->moveDown(MOVE_VERT_SPEED);
		movePlatDown(MOVE_VERT_SPEED);
	}

	CPlatform::process();
}



}
