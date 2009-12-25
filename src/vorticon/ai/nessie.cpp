#include "CObjectAI.h"

// Nessie (in ep3) (on the world map)

enum nessie_actions{
	NESSIE_SWIMNORMAL, NESSIE_SWIMUPTOMORTIMER,
	NESSIE_PAUSE_MORTIMER, NESSIE_SWIMAWAY_MORTIMER
};

#define NESSIE_PAUSE_TIME       250

#define NESSIE_ANIM_RATE        6

#define NESSIE_DOWNLEFT_FRAME   130
#define NESSIE_DOWNRIGHT_FRAME  132
#define NESSIE_UPRIGHT_FRAME    134
#define NESSIE_UPLEFT_FRAME     136

#define NESSIE_SPEED            16

#define NESSIE_MORTIMER_SWIM_AMT        ((8<<STC)/NESSIE_SPEED)
#define NESSIE_MORTIMER_SWIMAWAY_AMT    ((16<<STC)/NESSIE_SPEED)

void nessie_find_next_checkpoint(int o);

void CObjectAI::nessie_ai(CObject& object)
{
	unsigned int i;
	bool isMounted;

	if (object.needinit)
	{
		object.ai.nessie.baseframe = NESSIE_DOWNLEFT_FRAME;
		object.ai.nessie.leftrightdir = LEFT;
		object.ai.nessie.updowndir = DOWN;
		object.ai.nessie.animframe = 0;
		object.ai.nessie.animtimer = 0;
		object.ai.nessie.state = NESSIE_SWIMNORMAL;
		object.ai.nessie.pausetimer = 0;
		object.ai.nessie.mortimer_swim_amt = 0;
		object.ai.nessie.destx = 0;
		object.ai.nessie.desty = 0;
		object.inhibitfall = 1;
		object.canbezapped = 0;
		object.needinit = 0;

		for(i=0;i<=NESSIETRAILLEN;i++)
		{
			object.ai.nessie.tiletrailX[i] = 0;
			object.ai.nessie.tiletrailY[i] = 0;
		}
		// kick nessie into going counter-clockwise
		// (otherwise she'll go clockwise)
		object.ai.nessie.tiletrailX[0] = (object.x>>CSF);
		object.ai.nessie.tiletrailY[0] = (object.y>>CSF);
		object.ai.nessie.tiletrailX[1] = (object.x>>CSF)+1;
		object.ai.nessie.tiletrailY[1] = (object.y>>CSF);
		object.ai.nessie.tiletrailhead = 2;

		for(i=0;i<MAX_PLAYERS;i++)
		{
			object.ai.nessie.mounted[i] = 0;
		}

		nessie_find_next_checkpoint(object);
	}

	// find out if nessie is mounted, and for all players that are
	// mounted keep them stuck to nessie
	isMounted = 0;
	for(i=0;i<m_NumPlayers;i++)
	{
		if (object.ai.nessie.mounted[i])
		{
			m_Player[i].x = object.x;
			m_Player[i].y = object.y;
			isMounted = 1;
		}
	}

	// animation
	object.sprite = object.ai.nessie.baseframe + object.ai.nessie.animframe;
	if (isMounted) object.sprite += 8;

	if (object.ai.nessie.animtimer > NESSIE_ANIM_RATE)
	{
		object.ai.nessie.animframe ^= 1;
		object.ai.nessie.animtimer = 0;
	}
	else object.ai.nessie.animtimer++;

	//printf("nessie state: %d\n", object.ai.nessie.state);

	switch(object.ai.nessie.state)
	{
	case NESSIE_SWIMUPTOMORTIMER:
		if (!object.ai.nessie.mortimer_swim_amt)
		{
			object.ai.nessie.state = NESSIE_PAUSE_MORTIMER;
			object.ai.nessie.pausetimer = NESSIE_PAUSE_TIME;
			object.ai.nessie.baseframe = NESSIE_DOWNRIGHT_FRAME;
		}
		else
		{
			object.y -= NESSIE_SPEED;
			object.ai.nessie.mortimer_swim_amt--;
		}
		break;

	case NESSIE_PAUSE_MORTIMER:
		if (!object.ai.nessie.pausetimer)
		{
			//object.ai.nessie.state = NESSIE_SWIMAWAY_MORTIMER;
			//object.ai.nessie.mortimer_swim_amt = NESSIE_MORTIMER_SWIMAWAY_AMT;
			object.ai.nessie.state = NESSIE_SWIMNORMAL;
		}
		else object.ai.nessie.pausetimer--;
		break;

	case NESSIE_SWIMAWAY_MORTIMER:
		if (!object.ai.nessie.mortimer_swim_amt)
		{
			object.ai.nessie.state = NESSIE_SWIMNORMAL;
		}
		else
		{
			object.x += NESSIE_SPEED;
			object.y += NESSIE_SPEED;
			object.ai.nessie.mortimer_swim_amt--;
		}
		break;

	case NESSIE_SWIMNORMAL:

		// arrived at destination?
		if ( (object.x>>CSF) == (object.ai.nessie.destx>>CSF)  && \
				(object.y>>CSF) == (object.ai.nessie.desty>>CSF) )
		{
			// if we're approaching the mount point by Mortimer's castle
			// go into a special state that aligns her properly with the island
			// (i couldn't get this to work right any other way)
			if (object.ai.nessie.desty>>CSF < mp_Map->m_height>>2)
			{
				if (mp_Map->getObjectat((object.ai.nessie.destx>>CSF)+2, (((object.ai.nessie.desty>>STC)+8)>>TILE_S)-1) == NESSIE_PAUSE)
				{
					object.ai.nessie.state = NESSIE_SWIMUPTOMORTIMER;
					object.ai.nessie.mortimer_swim_amt = NESSIE_MORTIMER_SWIM_AMT;
				}
			}

			nessie_find_next_checkpoint(object);

			// set up/down and left/right direction flags for frame selection
			if (object.ai.nessie.destx > object.x)
				object.ai.nessie.leftrightdir = RIGHT;
			else if (object.ai.nessie.destx < object.x)
				object.ai.nessie.leftrightdir = LEFT;

			if (object.ai.nessie.desty < object.y)
				object.ai.nessie.updowndir = UP;
			else
				object.ai.nessie.updowndir = DOWN;
		}

		if (object.ai.nessie.pausetimer)
			object.ai.nessie.updowndir = DOWN;

		// select proper frame based on up/down and left/right direction flags
		if (object.ai.nessie.updowndir==DOWN && object.ai.nessie.leftrightdir==LEFT)
			object.ai.nessie.baseframe = NESSIE_DOWNLEFT_FRAME;
		else if (object.ai.nessie.updowndir==DOWN && object.ai.nessie.leftrightdir==RIGHT)
			object.ai.nessie.baseframe = NESSIE_DOWNRIGHT_FRAME;
		else if (object.ai.nessie.updowndir==UP && object.ai.nessie.leftrightdir==LEFT)
			object.ai.nessie.baseframe = NESSIE_UPLEFT_FRAME;
		else if (object.ai.nessie.updowndir==UP && object.ai.nessie.leftrightdir==RIGHT)
			object.ai.nessie.baseframe = NESSIE_UPRIGHT_FRAME;

		// pause if needed
		if (object.ai.nessie.pausetimer)
		{
			object.ai.nessie.pausetimer--;
			return;
		}

		// head to destination
		if (object.x < object.ai.nessie.destx)
		{
			if (object.ai.nessie.destx - object.x > NESSIE_SPEED)
			{
				object.x += NESSIE_SPEED;
			}
			else
			{
				object.x++;
			}
		}
		else if (object.x > object.ai.nessie.destx)
		{
			if (object.x - object.ai.nessie.destx > NESSIE_SPEED)
			{
				object.x -= NESSIE_SPEED;
			}
			else
			{
				object.x--;
			}
		}

		if (object.y < object.ai.nessie.desty)
		{
			if (object.ai.nessie.desty - object.y > NESSIE_SPEED)
				object.y += NESSIE_SPEED;
			else
				object.y++;
		}
		else if (object.y > object.ai.nessie.desty)
		{
			if (object.y - object.ai.nessie.desty > NESSIE_SPEED)
				object.y -= NESSIE_SPEED;
			else
				object.y--;
		}

		break;
	}
}

void CObjectAI::nessie_find_next_checkpoint(CObject& object)
{
	int x,y,i;
	int xa,ya;
	unsigned int destx, desty;

	// search in the 8 surrounding tiles and head to the first pathtile
	// we find that's not one of the last 5 we've been to
	x = (object.x>>CSF)-1;
	y = (((object.y>>STC)+8)>>TILE_S)-1;
	destx = desty = 0;

	printf("\n\nJetzt (%d|%d)\n", x+1,y+1);
	printf("\n\nTraillist\n");
	for(i=0;i<=NESSIETRAILLEN;i++)
	{
		printf("(%d|%d)\n", object.ai.nessie.tiletrailX[i], object.ai.nessie.tiletrailY[i]);
	}
	for(ya=0;ya<3;ya++)
	{
		printf("\n");
		for(xa=0;xa<3;xa++)
		{
			destx = x+xa;
			desty = y+ya;

			int obj = mp_Map->getObjectat(destx, desty);
			printf("%d (%d|%d) ", obj,destx, desty);
			if (obj==NESSIE_PATH || obj==NESSIE_PAUSE)
			{
				// find out if this is one of the last tiles we've been to
				bool oneoflasttiles = false;
				for(i=0;i<NESSIETRAILLEN;i++)
				{
					if (object.ai.nessie.tiletrailX[i]==destx &&
							object.ai.nessie.tiletrailY[i]==desty)
					{
						oneoflasttiles = true;
						break;
					}
				}

				if (!oneoflasttiles)
				{
					object.ai.nessie.tiletrailX[object.ai.nessie.tiletrailhead] = destx;
					object.ai.nessie.tiletrailY[object.ai.nessie.tiletrailhead] = desty;
					object.ai.nessie.tiletrailhead++;
					if (object.ai.nessie.tiletrailhead>=NESSIETRAILLEN)
					{
						object.ai.nessie.tiletrailhead = 0;
					}
					goto foundtile;
				} // end if (!oneoflasttiles)
			}  // end if(obj==8192 ...

		} // end for(xa...
	} // end for(ya...

	//return;

	foundtile: ;


	object.ai.nessie.destx = (destx<<CSF);
	object.ai.nessie.desty = (((desty<<STC)-8)<<TILE_S);
	printf("\n\nnext1 (%d|%d)\n", object.ai.nessie.destx>>CSF,object.ai.nessie.desty>>CSF);
	printf("\n\nnext2 (%d|%d)\n", destx,desty);

	// make nessie pause at pause points, not in the top quarter
	// of the map (i.e. not at the mortimer's castle mount point...
	// this is done separetly elsewhere with a special state)
	if (desty > mp_Map->m_height>>2)
	{
		if (mp_Map->getObjectat(destx, desty) == NESSIE_PAUSE)
		{
			// make nessie pause
			object.ai.nessie.pausetimer = NESSIE_PAUSE_TIME;
			object.ai.nessie.pausex = destx;
			object.ai.nessie.pausey = desty;
		}
	}
}

