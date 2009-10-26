#include "../../keen.h"
#include "../../game.h"

//#include "enemyai.h"

#include "icecannon.h"

#include "../../sdl/sound/CSound.h"

// the chunks of ice shot out by an ice cannon (ep1)
#define ICECHUNK_SPEED        		15
#define ICECHUNK_STRAIGHT_SPEED     20
#define ICECHUNK_WAIT_TIME    		150

void icechunk_ai(int o)
{
/*
	if (objects[o].needinit)
	{  // first time initialization
		int speed;

		if (objects[o].ai.icechunk.vector_x && objects[o].ai.icechunk.vector_y)
			speed = ICECHUNK_SPEED;
		else
			speed = ICECHUNK_STRAIGHT_SPEED;

		objects[o].ai.icechunk.veloc_x = speed * objects[o].ai.icechunk.vector_x;
		objects[o].ai.icechunk.veloc_y = speed * objects[o].ai.icechunk.vector_y;
		objects[o].inhibitfall = 1;
		objects[o].canbezapped = 0;
		objects[o].needinit = 0;
	}

	// freeze the player if it touches him
	if (objects[o].touchPlayer)
	{
		if (!player[objects[o].touchPlayer].pfrozentime)
		{
			// make him start sliding in the direction of the impact
			if (objects[o].ai.icechunk.vector_x > 0)
			{
				player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = RIGHT;
				player[objects[o].touchedBy].pinertia_x = PMAXSPEED;
			}
			else if (objects[o].ai.icechunk.vector_x < 0)
			{
				player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = LEFT;
				player[objects[o].touchedBy].pinertia_x = -PMAXSPEED;
			}
			else	// perfectly vertical ice cannons
			{
				#define UPDNCANNON_PUSHAMT		4
				if (player[objects[o].touchedBy].pinertia_x < UPDNCANNON_PUSHAMT)
				{
					if (rnd()&1)
						player[objects[o].touchedBy].pinertia_x = UPDNCANNON_PUSHAMT;
					else
						player[objects[o].touchedBy].pinertia_x = -UPDNCANNON_PUSHAMT;
				}
			}
		}

		freezeplayer(objects[o].touchedBy);
		smash(o);
		return;
	}

	// smash the chunk if it hits something
	if (objects[o].ai.icechunk.vector_x > 0)
	{
		if (objects[o].blockedr) { smash(o); return; }
	}
	else if (objects[o].ai.icechunk.vector_x < 0)
	{
		if (objects[o].blockedl) { smash(o); return; }
	}

	if (objects[o].ai.icechunk.vector_y > 0)
	{
		if (objects[o].blockedd) { smash(o); return; }
	}
	else if (objects[o].ai.icechunk.vector_y < 0)
	{
		if (objects[o].blockedu) { smash(o); return; }
	}

	// fly through the air
	objects[o].x += objects[o].ai.icechunk.veloc_x;
	objects[o].y += objects[o].ai.icechunk.veloc_y;*/
}


/*static void smash(int o)
{
int newobject;

	if (objects[o].onscreen)
	{
		g_pSound->playStereofromCoord(SOUND_CHUNKSMASH, PLAY_NOW, objects[o].x);

		// upleft
		newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICEBIT);
		objects[newobject].ai.icechunk.vector_x = -1;
		objects[newobject].ai.icechunk.vector_y = -1;

		// upright
		newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICEBIT);
		objects[newobject].ai.icechunk.vector_x = 1;
		objects[newobject].ai.icechunk.vector_y = -1;

		// downleft
		newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICEBIT);
		objects[newobject].ai.icechunk.vector_x = -1;
		objects[newobject].ai.icechunk.vector_y = 1;

		// downright
		newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICEBIT);
		objects[newobject].ai.icechunk.vector_x = 1;
		objects[newobject].ai.icechunk.vector_y = 1;
	}

	delete_object(o);
}*/


// the little pieces that break off of an OBJ_ICECHUNK when it hits
// a wall or a player. (Ep1)
#define ICEBIT_SPEED        20

void icebit_ai(int o)
{
	/*if (objects[o].needinit)
	{  // first time initilization
		objects[o].ai.icechunk.veloc_x = ICEBIT_SPEED * objects[o].ai.icechunk.vector_x;
		objects[o].ai.icechunk.veloc_y = ICEBIT_SPEED * objects[o].ai.icechunk.vector_y;
		objects[o].inhibitfall = 1;
		objects[o].canbezapped = 0;
		objects[o].needinit = 0;
	}

	objects[o].x += objects[o].ai.icechunk.veloc_x;
	objects[o].y += objects[o].ai.icechunk.veloc_y;

	if (!objects[o].onscreen)
	{
		delete_object(o);
	}*/
}


// the ice cannon itself
void icecannon_ai(int o)
{
/*int newobject;

	// keep spawner object invisible and properly positioned
	objects[o].sprite = BLANKSPRITE;
	objects[o].inhibitfall = 1;

	if (!gunfiretimer)
	{
		newobject = spawn_object(objects[o].x, objects[o].y, OBJ_ICECHUNK);

		objects[newobject].ai.icechunk.vector_x = objects[o].ai.icechunk.vector_x;
		objects[newobject].ai.icechunk.vector_y = objects[o].ai.icechunk.vector_y;
	}*/
}


