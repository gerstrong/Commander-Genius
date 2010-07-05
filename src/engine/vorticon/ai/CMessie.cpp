#include "CObjectAI.h"

// Nessie (in ep3) (on the world map)
enum nessie_actions{
	NESSIE_SWIMNORMAL, NESSIE_PAUSE
};

#define NESSIE_PAUSE_TIME       250

#define NESSIE_ANIM_RATE        4

#define NESSIE_DOWNLEFT_FRAME   130
#define NESSIE_DOWNRIGHT_FRAME  132
#define NESSIE_UPRIGHT_FRAME    134
#define NESSIE_UPLEFT_FRAME     136

#define NESSIE_SPEED            32

void nessie_find_next_checkpoint(int o);

CMessie::CMessie(CMap *p_map) :
CObject(p_map)
{}

void CObjectAI::nessie_ai(CObject& object)
{
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

		for(size_t i=0;i<=NESSIETRAILLEN;i++)
		{
			object.ai.nessie.tiletrailX[i] = 0;
			object.ai.nessie.tiletrailY[i] = 0;
		}
		// kick nessie into going counter-clockwise
		// (otherwise she'll go clockwise)
		int x, y;
		x = object.getXPosition()>>CSF;
		y = object.getYPosition()>>CSF;
		object.ai.nessie.tiletrailX[0] = x;
		object.ai.nessie.tiletrailY[0] = y;
		object.ai.nessie.tiletrailX[1] = x+1;
		object.ai.nessie.tiletrailY[1] = y;
		object.ai.nessie.tiletrailhead = 2;

		for(size_t i=0;i<MAX_PLAYERS;i++)
		{
			object.ai.nessie.mounted[i] = false;
		}

		nessie_find_next_checkpoint(object);
	}

	// find out if nessie is mounted, and for all players that are
	// mounted keep them stuck to nessie
	bool isMounted = false;

	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		if (object.ai.nessie.mounted[it_player->m_index])
		{
			int x = object.getXPosition();
			int y = object.getYPosition();
			it_player->moveTo(x, y);
			isMounted = true;
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

	switch(object.ai.nessie.state)
	{
	case NESSIE_SWIMNORMAL:
		// arrived at destination?
		unsigned int x,y;
		x = object.getXPosition();
		y = object.getYPosition();
		if ( x > (object.ai.nessie.destx-NESSIE_SPEED/2)  &&
				x < (object.ai.nessie.destx+NESSIE_SPEED/2) )
		{
			if ( y > (object.ai.nessie.desty-NESSIE_SPEED/2)  &&
					y < (object.ai.nessie.desty+NESSIE_SPEED/2) )
			{
				nessie_find_next_checkpoint(object);

				// set up/down and left/right direction flags for frame selection
				x = object.getXPosition();
				y = object.getYPosition();
				if (object.ai.nessie.destx > x)
					object.ai.nessie.leftrightdir = RIGHT;
				else if (object.ai.nessie.destx < x)
					object.ai.nessie.leftrightdir = LEFT;

				if (object.ai.nessie.desty < y)
					object.ai.nessie.updowndir = UP;
				else if (object.ai.nessie.destx > y)
					object.ai.nessie.updowndir = DOWN;
			}
		}
		move_nessie(object);
		break;
	case NESSIE_PAUSE:
		if(object.ai.nessie.pausetimer)
		{
			object.ai.nessie.pausetimer--;
		}
		else
		{
			object.ai.nessie.state = NESSIE_SWIMNORMAL;
			move_nessie(object);
		}
		break;
	}
}

void CObjectAI::move_nessie(CObject& object)
{
	unsigned int x = object.getXPosition();
	unsigned int y = object.getYPosition();

	// select proper frame based on up/down and left/right direction flags
	if (object.ai.nessie.updowndir==DOWN && object.ai.nessie.leftrightdir==LEFT)
		object.ai.nessie.baseframe = NESSIE_DOWNLEFT_FRAME;
	else if (object.ai.nessie.updowndir==DOWN && object.ai.nessie.leftrightdir==RIGHT)
		object.ai.nessie.baseframe = NESSIE_DOWNRIGHT_FRAME;
	else if (object.ai.nessie.updowndir==UP && object.ai.nessie.leftrightdir==LEFT)
		object.ai.nessie.baseframe = NESSIE_UPLEFT_FRAME;
	else if (object.ai.nessie.updowndir==UP && object.ai.nessie.leftrightdir==RIGHT)
		object.ai.nessie.baseframe = NESSIE_UPRIGHT_FRAME;

	// head to destination
	if (x < object.ai.nessie.destx)
		object.moveRight(NESSIE_SPEED);
	else if (x > object.ai.nessie.destx)
		object.moveLeft(NESSIE_SPEED);

	if (y < object.ai.nessie.desty)
		object.moveDown(NESSIE_SPEED);
	else if (y > object.ai.nessie.desty)
		object.moveUp(NESSIE_SPEED);
}

void CObjectAI::nessie_find_next_checkpoint(CObject& object)
{
	int x,y,i;
	int xa,ya;
	unsigned int destx, desty;

	// search in the 8 surrounding tiles and head to the first pathtile
	// we find that's not one of the last 5 we've been to

	x = ((object.getXPosition())>>CSF)-1;
	y = ((object.getYPosition()+(8<<STC))>>CSF)-1;

	destx = desty = 0;

	for(ya=0;ya<3;ya++)
	{
		for(xa=0;xa<3;xa++)
		{
			destx = x+xa;
			desty = y+ya;

			int obj = mp_Map->getObjectat(destx, desty);
			if (obj==NESSIE_PATH || obj==NESSIE_WEED)
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

	// If Nessie went to an dead end. Go back!
	object.ai.nessie.tiletrailhead = 0;
	return;

	foundtile: ;

	object.ai.nessie.destx = (destx<<CSF);
	object.ai.nessie.desty = (desty<<CSF)-(8<<STC);

	int obj = mp_Map->getObjectat(destx, desty);
	if(obj == NESSIE_WEED || obj == NESSIE_LAND)
	{
		object.ai.nessie.state = NESSIE_PAUSE;
		object.ai.nessie.pausetimer = NESSIE_PAUSE_TIME;
	}
}

