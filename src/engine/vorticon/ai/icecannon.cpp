#include "CObjectAI.h"
#include "../../spritedefines.h"
#include "../../../keen.h"
#include "../../../sdl/sound/CSound.h"

// the chunks of ice shot out by an ice cannon (ep1)
const int ICECHUNK_SPEED = 60;
const int ICECHUNK_STRAIGHT_SPEED = 80;
const int ICECHUNK_WAIT_TIME = 19;

unsigned int rnd(void);

void CObjectAI::icechunk_ai(CObject &object)
{
	if (object.needinit)
	{  // first time initialization
		int speed;

		if (object.ai.icechunk.vector_x && object.ai.icechunk.vector_y)
			speed = ICECHUNK_SPEED;
		else
			speed = ICECHUNK_STRAIGHT_SPEED;

		object.ai.icechunk.veloc_x = speed * object.ai.icechunk.vector_x;
		object.ai.icechunk.veloc_y = speed * object.ai.icechunk.vector_y;
		object.inhibitfall = true;
		object.canbezapped = 0;
		object.needinit = 0;
	}

	// freeze the player if it touches him
	if (object.touchPlayer)
	{
		CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();
		// make him start sliding in the direction of the impact
		if (object.ai.icechunk.vector_x > 0)
		{
			m_Player[object.touchedBy].pdir = m_Player[object.touchedBy].pshowdir = RIGHT;
			m_Player[object.touchedBy].xinertia = Physics.player.max_x_speed;
			m_Player[object.touchedBy].bump(Physics.player.max_x_speed/2, true);
		}
		else if (object.ai.icechunk.vector_x < 0)
		{
			m_Player[object.touchedBy].pdir = m_Player[object.touchedBy].pshowdir = LEFT;
			m_Player[object.touchedBy].xinertia = -Physics.player.max_x_speed;
			m_Player[object.touchedBy].bump(-Physics.player.max_x_speed/2, true);
		}
		else	// perfectly vertical ice cannons
		{
			const int UPDNCANNON_PUSHAMT = 16;
			if (m_Player[object.touchedBy].xinertia < UPDNCANNON_PUSHAMT)
			{
				if (rnd()&1)
					m_Player[object.touchedBy].xinertia = UPDNCANNON_PUSHAMT;
				else
					m_Player[object.touchedBy].xinertia = -UPDNCANNON_PUSHAMT;
			}
		}

		m_Player[object.touchedBy].freeze();
		smash(object);
		return;
	}

	// smash the chunk if it hits something
	if (object.ai.icechunk.vector_x > 0)
	{
		if (object.blockedr) { smash(object); return; }
	}
	else if (object.ai.icechunk.vector_x < 0)
	{
		if (object.blockedl) { smash(object); return; }
	}

	if (object.ai.icechunk.vector_y > 0)
	{
		if (object.blockedd) { smash(object); return; }
	}
	else if (object.ai.icechunk.vector_y < 0)
	{
		if (object.blockedu) { smash(object); return; }
	}

	// fly through the air
	object.moveXDir(object.ai.icechunk.veloc_x);
	object.moveYDir(object.ai.icechunk.veloc_y);
}


void CObjectAI::smash(CObject &object)
{
	CObject chunk(mp_Map);

	if (object.onscreen)
	{
		g_pSound->playStereofromCoord(SOUND_CHUNKSMASH, PLAY_NOW, object.getXPosition());
		chunk.spawn(object.getXPosition(), object.getYPosition(), OBJ_ICEBIT, m_Episode);
		chunk.solid = false;

		// upleft
		chunk.ai.icechunk.vector_x = -1;
		chunk.ai.icechunk.vector_y = -1;
		m_Objvect.push_back(chunk);

		// upright
		chunk.ai.icechunk.vector_x = 1;
		chunk.ai.icechunk.vector_y = -1;
		m_Objvect.push_back(chunk);

		// downleft
		chunk.ai.icechunk.vector_x = -1;
		chunk.ai.icechunk.vector_y = 1;
		m_Objvect.push_back(chunk);

		// downright
		chunk.ai.icechunk.vector_x = 1;
		chunk.ai.icechunk.vector_y = 1;
		m_Objvect.push_back(chunk);
	}
	deleteObj(object);
}


// the little pieces that break off of an OBJ_ICECHUNK when it hits
// a wall or a player. (Ep1)
#define ICEBIT_SPEED        80

void CObjectAI::icebit_ai(CObject &object)
{
	if (object.needinit)
	{  // first time initialization
		object.ai.icechunk.veloc_x = ICEBIT_SPEED * object.ai.icechunk.vector_x;
		object.ai.icechunk.veloc_y = ICEBIT_SPEED * object.ai.icechunk.vector_y;
		object.ai.icechunk.timer = 40;
		object.inhibitfall = true;
		object.canbezapped = false;
		object.needinit = false;
	}

	object.ai.icechunk.timer--;

	object.moveXDir(object.ai.icechunk.veloc_x);
	object.moveYDir(object.ai.icechunk.veloc_y);

	if (!object.onscreen or !m_gunfiretimer or object.ai.icechunk.timer <= 0)
	{
		deleteObj(object);
	}
}


// the ice cannon itself
void CObjectAI::icecannon_ai(CObject &object)
{
	 // keep spawner object invisible and properly positioned
	 object.sprite = BLANKSPRITE;
	 object.inhibitfall = 1;
	 
	 if (m_gunfiretimer == 0)
	 {
		 CObject chunk(mp_Map);
		 int newpos_x = object.getXPosition()+(object.ai.icechunk.vector_x)*512;
		 int newpos_y = object.getYPosition()+(object.ai.icechunk.vector_y)*512;
		 chunk.spawn( newpos_x, newpos_y, OBJ_ICECHUNK, m_Episode);
		 chunk.ai.icechunk.vector_x = object.ai.icechunk.vector_x;
		 chunk.ai.icechunk.vector_y = object.ai.icechunk.vector_y;
		 m_Objvect.push_back(chunk);
	 }
}


