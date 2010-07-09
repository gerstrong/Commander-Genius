#include "CMessie.h"

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

CMessie::CMessie(CMap *p_map, Uint32 x, Uint32 y,
		std::vector<CPlayer>& Player) :
CObject(p_map, x, y, OBJ_MESSIE),
m_Player(Player)
{
	onscreen = true;
	solid = false;

	baseframe = NESSIE_DOWNLEFT_FRAME;
	leftrightdir = LEFT;
	updowndir = DOWN;
	animframe = 0;
	animtimer = 0;
	state = NESSIE_SWIMNORMAL;
	pausetimer = 0;
	mortimer_swim_amt = 0;
	destx = 0;
	desty = 0;
	inhibitfall = 1;
	canbezapped = 0;
	needinit = 0;

	for(size_t i=0;i<=NESSIETRAILLEN;i++)
	{
		tiletrailX[i] = 0;
		tiletrailY[i] = 0;
	}
	// kick nessie into going counter-clockwise
	// (otherwise she'll go clockwise)
	int mx, my;
	mx = getXPosition()>>CSF;
	my = getYPosition()>>CSF;
	tiletrailX[0] = mx;
	tiletrailY[0] = my;
	tiletrailX[1] = mx+1;
	tiletrailY[1] = my;
	tiletrailhead = 2;

	for(size_t i=0;i<MAX_PLAYERS;i++)
	{
		mounted[i] = false;
	}

	nessie_find_next_checkpoint();
}

void CMessie::process()
{
	// find out if nessie is mounted, and for all players that are
	// mounted keep them stuck to nessie
	bool isMounted = false;

	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		if (mounted[it_player->m_index])
		{
			int x = getXPosition();
			int y = getYPosition();
			it_player->moveTo(x, y);
			isMounted = true;
		}
	}

	// animation
	sprite = baseframe + animframe;
	if (isMounted) sprite += 8;

	if (animtimer > NESSIE_ANIM_RATE)
	{
		animframe ^= 1;
		animtimer = 0;
	}
	else animtimer++;

	switch(state)
	{
	case NESSIE_SWIMNORMAL:
		// arrived at destination?
		unsigned int x,y;
		x = getXPosition();
		y = getYPosition();
		if ( x > (destx-NESSIE_SPEED/2)  &&
				x < (destx+NESSIE_SPEED/2) )
		{
			if ( y > (desty-NESSIE_SPEED/2)  &&
					y < (desty+NESSIE_SPEED/2) )
			{
				nessie_find_next_checkpoint();

				// set up/down and left/right direction flags for frame selection
				x = getXPosition();
				y = getYPosition();
				if (destx > x)
					leftrightdir = RIGHT;
				else if (destx < x)
					leftrightdir = LEFT;

				if (desty < y)
					updowndir = UP;
				else if (destx > y)
					updowndir = DOWN;
			}
		}
		move_nessie();
		break;
	case NESSIE_PAUSE:
		if(pausetimer)
		{
			pausetimer--;
		}
		else
		{
			state = NESSIE_SWIMNORMAL;
			move_nessie();
		}
		break;
	}
}

void CMessie::move_nessie()
{
	unsigned int x = getXPosition();
	unsigned int y = getYPosition();

	// select proper frame based on up/down and left/right direction flags
	if (updowndir==DOWN && leftrightdir==LEFT)
		baseframe = NESSIE_DOWNLEFT_FRAME;
	else if (updowndir==DOWN && leftrightdir==RIGHT)
		baseframe = NESSIE_DOWNRIGHT_FRAME;
	else if (updowndir==UP && leftrightdir==LEFT)
		baseframe = NESSIE_UPLEFT_FRAME;
	else if (updowndir==UP && leftrightdir==RIGHT)
		baseframe = NESSIE_UPRIGHT_FRAME;

	// head to destination
	if (x < destx)
		moveRight(NESSIE_SPEED);
	else if (x > destx)
		moveLeft(NESSIE_SPEED);

	if (y < desty)
		moveDown(NESSIE_SPEED);
	else if (y > desty)
		moveUp(NESSIE_SPEED);
}

void CMessie::nessie_find_next_checkpoint()
{
	int x,y,i;
	int xa,ya;
	unsigned int destx, desty;

	// search in the 8 surrounding tiles and head to the first pathtile
	// we find that's not one of the last 5 we've been to

	x = ((getXPosition())>>CSF)-1;
	y = ((getYPosition()+(8<<STC))>>CSF)-1;

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
					if (tiletrailX[i]==destx &&
							tiletrailY[i]==desty)
					{
						oneoflasttiles = true;
						break;
					}
				}

				if (!oneoflasttiles)
				{
					tiletrailX[tiletrailhead] = destx;
					tiletrailY[tiletrailhead] = desty;
					tiletrailhead++;
					if (tiletrailhead>=NESSIETRAILLEN)
					{
						tiletrailhead = 0;
					}
					goto foundtile;
				} // end if (!oneoflasttiles)
			}  // end if(obj==8192 ...

		} // end for(xa...
	} // end for(ya...

	// If Nessie went to an dead end. Go back!
	tiletrailhead = 0;
	return;

	foundtile: ;

	destx = (destx<<CSF);
	desty = (desty<<CSF)-(8<<STC);

	int obj = mp_Map->getObjectat(destx, desty);
	if(obj == NESSIE_WEED || obj == NESSIE_LAND)
	{
		state = NESSIE_PAUSE;
		pausetimer = NESSIE_PAUSE_TIME;
	}
}
