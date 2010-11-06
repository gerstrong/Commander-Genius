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
m_direction(LEFTDOWN),
m_Player(Player)
{
	onscreen = true;
	solid = false;

	baseframe = NESSIE_DOWNLEFT_FRAME;

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
}

void CMessie::process()
{
	// find out if nessie is mounted, and for all players that are
	// mounted keep them stuck to nessie
	bool isMounted = false;

	if(destx == 0 && desty == 0)
		nessie_find_next_checkpoint();

	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		if (mounted[it_player->m_index])
		{
			it_player->moveTo(m_Pos);
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
		if ( getXPosition() > (destx-NESSIE_SPEED/2)  &&
				getXPosition() < (destx+NESSIE_SPEED/2) )
		{
			if ( getYPosition() > (desty-NESSIE_SPEED/2)  &&
					getYPosition() < (desty+NESSIE_SPEED/2) )
			{
				nessie_find_next_checkpoint();

				// set up/down and left/right direction flags for frame selection
				bool goleft = (destx < getXPosition());
				bool godown = (desty > getYPosition());

				if(goleft && !godown)
					m_direction = LEFTUP;
				else if(goleft && godown)
					m_direction = LEFTDOWN;
				else if(!goleft && !godown)
					m_direction = RIGHTUP;
				else if(!goleft && godown)
					m_direction = RIGHTDOWN;
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
	// select proper frame based on up/down and left/right direction flags
	if (m_direction == LEFTDOWN)
		baseframe = NESSIE_DOWNLEFT_FRAME;
	else if (m_direction == RIGHTDOWN)
		baseframe = NESSIE_DOWNRIGHT_FRAME;
	else if (m_direction == LEFTUP)
		baseframe = NESSIE_UPLEFT_FRAME;
	else if (m_direction == RIGHTUP)
		baseframe = NESSIE_UPRIGHT_FRAME;

	// head to destination
	if (getXPosition() < destx)
		moveRight(NESSIE_SPEED);
	else if (getXPosition() > destx)
		moveLeft(NESSIE_SPEED);

	if (getYPosition() < desty)
		moveDown(NESSIE_SPEED);
	else if (getYPosition() > desty)
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

	this->destx = (destx<<CSF);
	this->desty = (desty<<CSF)-(8<<STC);

	int obj = mp_Map->getObjectat(destx, desty);
	if(obj == NESSIE_WEED || obj == NESSIE_LAND)
	{
		state = NESSIE_PAUSE;
		pausetimer = NESSIE_PAUSE_TIME;
	}
}
