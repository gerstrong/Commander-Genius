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

void CObjectAI::icechunk_ai(CObject *p_object)
{
	if (p_object->needinit)
	{  // first time initialization
		int speed;

		if (p_object->ai.icechunk.vector_x && p_object->ai.icechunk.vector_y)
			speed = ICECHUNK_SPEED;
		else
			speed = ICECHUNK_STRAIGHT_SPEED;

		p_object->ai.icechunk.veloc_x = speed * p_object->ai.icechunk.vector_x;
		p_object->ai.icechunk.veloc_y = speed * p_object->ai.icechunk.vector_y;
		p_object->inhibitfall = 1;
		p_object->canbezapped = 0;
		p_object->needinit = 0;
	}

	// freeze the player if it touches him
	if (p_object->touchPlayer)
	{
		if (!mp_Player[p_object->touchPlayer].pfrozentime)
		{
			// make him start sliding in the direction of the impact
			if (p_object->ai.icechunk.vector_x > 0)
			{
				mp_Player[p_object->touchedBy].pdir = mp_Player[p_object->touchedBy].pshowdir = RIGHT;
				//mp_Player[p_object->touchedBy].pinertia_x = PMAXSPEED;
			}
			else if (p_object->ai.icechunk.vector_x < 0)
			{
				mp_Player[p_object->touchedBy].pdir = mp_Player[p_object->touchedBy].pshowdir = LEFT;
				//mp_Player[p_object->touchedBy].pinertia_x = -PMAXSPEED;
			}
			else	// perfectly vertical ice cannons
			{
#define UPDNCANNON_PUSHAMT		4
				if (mp_Player[p_object->touchedBy].pinertia_x < UPDNCANNON_PUSHAMT)
				{
					if (rnd()&1)
						mp_Player[p_object->touchedBy].pinertia_x = UPDNCANNON_PUSHAMT;
					else
						mp_Player[p_object->touchedBy].pinertia_x = -UPDNCANNON_PUSHAMT;
				}
			}
		}

		freezeplayer(p_object->touchedBy);
		smash(p_object);
		return;
	}

	// smash the chunk if it hits something
	if (p_object->ai.icechunk.vector_x > 0)
	{
		if (p_object->blockedr) { smash(p_object); return; }
	}
	else if (p_object->ai.icechunk.vector_x < 0)
	{
		if (p_object->blockedl) { smash(p_object); return; }
	}

	if (p_object->ai.icechunk.vector_y > 0)
	{
		if (p_object->blockedd) { smash(p_object); return; }
	}
	else if (p_object->ai.icechunk.vector_y < 0)
	{
		if (p_object->blockedu) { smash(p_object); return; }
	}

	// fly through the air
	p_object->x += p_object->ai.icechunk.veloc_x;
	p_object->y += p_object->ai.icechunk.veloc_y;
}


void CObjectAI::smash(CObject *p_object)
{
	CObject chunk;

	if (p_object->onscreen)
	{
		g_pSound->playStereofromCoord(SOUND_CHUNKSMASH, PLAY_NOW, p_object->x);
		chunk.spawn(p_object->x, p_object->y, OBJ_ICEBIT);

		// upleft
		chunk.ai.icechunk.vector_x = -1;
		chunk.ai.icechunk.vector_y = -1;
		mp_Objvect->push_back(chunk);

		// upright
		chunk.ai.icechunk.vector_x = 1;
		chunk.ai.icechunk.vector_y = -1;
		mp_Objvect->push_back(chunk);

		// downleft
		chunk.ai.icechunk.vector_x = -1;
		chunk.ai.icechunk.vector_y = 1;
		mp_Objvect->push_back(chunk);

		// downright
		chunk.ai.icechunk.vector_x = 1;
		chunk.ai.icechunk.vector_y = 1;
		mp_Objvect->push_back(chunk);
	}
}


// the little pieces that break off of an OBJ_ICECHUNK when it hits
// a wall or a player. (Ep1)
#define ICEBIT_SPEED        20

void CObjectAI::icebit_ai(CObject *p_object)
{
	if (p_object->needinit)
	{  // first time initilization
		p_object->ai.icechunk.veloc_x = ICEBIT_SPEED * p_object->ai.icechunk.vector_x;
		p_object->ai.icechunk.veloc_y = ICEBIT_SPEED * p_object->ai.icechunk.vector_y;
		p_object->inhibitfall = 1;
		p_object->canbezapped = 0;
		p_object->needinit = 0;
	}

	p_object->x += p_object->ai.icechunk.veloc_x;
	p_object->y += p_object->ai.icechunk.veloc_y;

	if (!p_object->onscreen)
	{
		deleteObj(p_object);
	}
}


// the ice cannon itself
void CObjectAI::icecannon_ai(CObject *p_object)
{
	CObject chunk;
	 
	 // keep spawner object invisible and properly positioned
	 p_object->sprite = BLANKSPRITE;
	 p_object->inhibitfall = 1;
	 
	 //if (!gunfiretimer)
	 //{

		 chunk.spawn(p_object->x, p_object->y, OBJ_ICECHUNK);
	 
		 chunk.ai.icechunk.vector_x = p_object->ai.icechunk.vector_x;
		 chunk.ai.icechunk.vector_y = p_object->ai.icechunk.vector_y;
	 //}
}


