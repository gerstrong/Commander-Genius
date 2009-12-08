#include "CObjectAI.h"
#include "../spritedefines.h"
#include "../../keen.h"
#include "../../game.h"
#include "../../sdl/sound/CSound.h"

// the chunks of ice shot out by an ice cannon (ep1)
#define ICECHUNK_SPEED        		60
#define ICECHUNK_STRAIGHT_SPEED     80
#define ICECHUNK_WAIT_TIME    		38

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
		object.inhibitfall = 1;
		object.canbezapped = 0;
		object.needinit = 0;
	}

	// freeze the player if it touches him
	if (object.touchPlayer)
	{
		if (!mp_Player[object.touchPlayer].pfrozentime)
		{
			// make him start sliding in the direction of the impact
			if (object.ai.icechunk.vector_x > 0)
			{
				mp_Player[object.touchedBy].pdir = mp_Player[object.touchedBy].pshowdir = RIGHT;
				//mp_Player[object.touchedBy].pinertia_x = PMAXSPEED;
			}
			else if (object.ai.icechunk.vector_x < 0)
			{
				mp_Player[object.touchedBy].pdir = mp_Player[object.touchedBy].pshowdir = LEFT;
				//mp_Player[object.touchedBy].pinertia_x = -PMAXSPEED;
			}
			else	// perfectly vertical ice cannons
			{
#define UPDNCANNON_PUSHAMT		4
				if (mp_Player[object.touchedBy].pinertia_x < UPDNCANNON_PUSHAMT)
				{
					if (rnd()&1)
						mp_Player[object.touchedBy].pinertia_x = UPDNCANNON_PUSHAMT;
					else
						mp_Player[object.touchedBy].pinertia_x = -UPDNCANNON_PUSHAMT;
				}
			}
		}

		freezeplayer(object.touchedBy);
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
	object.x += object.ai.icechunk.veloc_x;
	object.y += object.ai.icechunk.veloc_y;
}


void CObjectAI::smash(CObject &object)
{
	CObject chunk;

	if (object.onscreen)
	{
		g_pSound->playStereofromCoord(SOUND_CHUNKSMASH, PLAY_NOW, object.x);
		chunk.spawn(object.x, object.y, OBJ_ICEBIT);

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
}


// the little pieces that break off of an OBJ_ICECHUNK when it hits
// a wall or a player. (Ep1)
#define ICEBIT_SPEED        20

void CObjectAI::icebit_ai(CObject &object)
{
	if (object.needinit)
	{  // first time initilization
		object.ai.icechunk.veloc_x = ICEBIT_SPEED * object.ai.icechunk.vector_x;
		object.ai.icechunk.veloc_y = ICEBIT_SPEED * object.ai.icechunk.vector_y;
		object.inhibitfall = 1;
		object.canbezapped = 0;
		object.needinit = 0;
	}

	object.x += object.ai.icechunk.veloc_x;
	object.y += object.ai.icechunk.veloc_y;

	if (!object.onscreen)
	{
		deleteObj(object);
	}
}


// the ice cannon itself
void CObjectAI::icecannon_ai(CObject &object)
{
	CObject chunk;
	 
	 // keep spawner object invisible and properly positioned
	 object.sprite = BLANKSPRITE;
	 object.inhibitfall = 1;
	 
	 //if (!gunfiretimer)
	 //{
		 chunk.spawn(object.x, object.y, OBJ_ICECHUNK);
	 
		 chunk.ai.icechunk.vector_x = object.ai.icechunk.vector_x;
		 chunk.ai.icechunk.vector_y = object.ai.icechunk.vector_y;
	 //}
}


