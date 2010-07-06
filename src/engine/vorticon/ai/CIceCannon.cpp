#include "CIceCannon.h"
#include "../../spritedefines.h"
#include "../../../keen.h"
#include "../../../sdl/sound/CSound.h"

// the chunks of ice shot out by an ice cannon (ep1)
const int ICECHUNK_SPEED = 60;
const int ICECHUNK_STRAIGHT_SPEED = 80;
const int ICECHUNK_WAIT_TIME = 19;

unsigned int rnd(void);

CIceCannon::CIceCannon(CMap *p_map, Uint32 x, Uint32 y) :
CObject(p_map,x,y)
{
//	int speed;
//
//	if (vector_x && vector_y)
//		speed = ICECHUNK_SPEED;
//	else
//		speed = ICECHUNK_STRAIGHT_SPEED;
//
//	veloc_x = speed * vector_x;
//	veloc_y = speed * vector_y;
//	inhibitfall = true;
//	canbezapped = 0;
//	needinit = 0;
}

void CIceCannon::process()
{

//	// freeze the player if it touches him
//	if (touchPlayer)
//	{
//		CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();
//		// make him start sliding in the direction of the impact
//		if (vector_x > 0)
//		{
//			m_Player[touchedBy].pdir = m_Player[touchedBy].pshowdir = RIGHT;
//			m_Player[touchedBy].xinertia = Physics.player.max_x_speed;
//			m_Player[touchedBy].bump(Physics.player.max_x_speed/2, true);
//		}
//		else if (vector_x < 0)
//		{
//			m_Player[touchedBy].pdir = m_Player[touchedBy].pshowdir = LEFT;
//			m_Player[touchedBy].xinertia = -Physics.player.max_x_speed;
//			m_Player[touchedBy].bump(-Physics.player.max_x_speed/2, true);
//		}
//		else	// perfectly vertical ice cannons
//		{
//			const int UPDNCANNON_PUSHAMT = 16;
//			if (m_Player[touchedBy].xinertia < UPDNCANNON_PUSHAMT)
//			{
//				if (rnd()&1)
//					m_Player[touchedBy].xinertia = UPDNCANNON_PUSHAMT;
//				else
//					m_Player[touchedBy].xinertia = -UPDNCANNON_PUSHAMT;
//			}
//		}
//
//		m_Player[touchedBy].freeze();
//		smash(object);
//		return;
//	}
//
//	// smash the chunk if it hits something
//	if (vector_x > 0)
//	{
//		if (blockedr) { smash(object); return; }
//	}
//	else if (vector_x < 0)
//	{
//		if (blockedl) { smash(object); return; }
//	}
//
//	if (vector_y > 0)
//	{
//		if (blockedd) { smash(object); return; }
//	}
//	else if (vector_y < 0)
//	{
//		if (blockedu) { smash(object); return; }
//	}
//
//	// fly through the air
//	moveXDir(veloc_x);
//	moveYDir(veloc_y);
}


//void CIceCannon::smash(CObject &object)
//{
//	CObject *chunk = new CObject(mp_Map);
//
//	if (onscreen)
//	{
//		g_pSound->playStereofromCoord(SOUND_CHUNKSMASH, PLAY_NOW, getXPosition());
//		chunk->spawn(getXPosition(), getYPosition(), OBJ_ICEBIT, m_Episode);
//		chunk->solid = false;
//
//		// upleft
//		chunk->ai.icechunk.vector_x = -1;
//		chunk->ai.icechunk.vector_y = -1;
//		m_Objvect.push_back(chunk);
//
//		// upright
//		chunk->ai.icechunk.vector_x = 1;
//		chunk->ai.icechunk.vector_y = -1;
//		m_Objvect.push_back(chunk);
//
//		// downleft
//		chunk->ai.icechunk.vector_x = -1;
//		chunk->ai.icechunk.vector_y = 1;
//		m_Objvect.push_back(chunk);
//
//		// downright
//		chunk->ai.icechunk.vector_x = 1;
//		chunk->ai.icechunk.vector_y = 1;
//		m_Objvect.push_back(chunk);
//	}
//	deleteObj(object);
//}
//
//
//// the little pieces that break off of an OBJ_ICECHUNK when it hits
//// a wall or a player. (Ep1)
//#define ICEBIT_SPEED        80
//
//void CIceCannon::icebit_ai(CObject &object)
//{
//	if (needinit)
//	{  // first time initialization
//		veloc_x = ICEBIT_SPEED * vector_x;
//		veloc_y = ICEBIT_SPEED * vector_y;
//		timer = 40;
//		inhibitfall = true;
//		canbezapped = false;
//		needinit = false;
//	}
//
//	timer--;
//
//	moveXDir(veloc_x);
//	moveYDir(veloc_y);
//
//	if (!onscreen or !m_gunfiretimer or timer <= 0)
//	{
//		deleteObj(object);
//	}
//}
//
//
//// the ice cannon itself
//void CIceCannon::icecannon_ai(CObject &object)
//{
//	 // keep spawner object invisible and properly positioned
//	 sprite = BLANKSPRITE;
//	 inhibitfall = 1;
//
//	 if (m_gunfiretimer == 0)
//	 {
//		 CObject *chunk = new CObject(mp_Map);
//		 int newpos_x = getXPosition()+(vector_x)*512;
//		 int newpos_y = getYPosition()+(vector_y)*512;
//		 chunk->spawn( newpos_x, newpos_y, OBJ_ICECHUNK, m_Episode);
//		 chunk->ai.icechunk.vector_x = vector_x;
//		 chunk->ai.icechunk.vector_y = vector_y;
//		 m_Objvect.push_back(chunk);
//	 }
//}
//

