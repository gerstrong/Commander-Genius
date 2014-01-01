#include "CMessie.h"
#include "sdl/input/CInput.h"

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

CMessie::CMessie(CMap *p_map, Uint32 x, Uint32 y) :
CVorticonSpriteObject(p_map, x, y, OBJ_MESSIE),
mounted(nullptr)
{
	xDirection = LEFT, yDirection = DOWN;
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
}


void CMessie::getTouchedBy(CVorticonSpriteObject &theObject)
{
    // This is not valid. Only if nessie is mounted or unmounted
    if(CPlayer *Player = dynamic_cast<CPlayer*>(&theObject))
    {
	if (mounted)
	    Player->moveTo(m_Pos);
    }
}

bool CMessie::tryMounting(CPlayer &player)
{
    const int dist = 2<<CSF;
	const int nessie_x = getXPosition();
	const int nessie_y = getYPosition();
	
	// Look if Messie is nearby
	const int x = player.getXPosition();
	const int y = player.getYPosition();
	
    if( x >= nessie_x-dist+m_BBox.x1 and x <= nessie_x+dist+m_BBox.x2 )
    {
        if( y >= nessie_y-dist+m_BBox.y1 and y <= nessie_y+dist+m_BBox.y2 )
        {
            player.solid = false;
            player.beingteleported = true;
            mounted = &player;
            return true;
        }
    }
	return false;
}

bool CMessie::tryToUnmount()
{
    // Unmount part!
    // Check if a NESSIE_LAND_OBJ is nearby the player. Only then he can unmount
    const int x = mounted->getXPosition()>>CSF;
    const int y = mounted->getYPosition()>>CSF;
    for(int dy=-2 ; dy <= 2 ; dy++)
    {
        for(int dx=-2 ; dx <= 2 ; dx++)
        {
            // If NESSIE_LAND_OBJ was found, than put the player there!
            const int infotile = mp_Map->getObjectat(x+dx, y+dy);

            if(infotile == NESSIE_LAND)
            {
                // Look for the Nessie object
                std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
                CTileProperties &Tile = TileProperty[mp_Map->at(x+dx, y+dy)];
                if( !Tile.bdown and !Tile.bup and
                        !Tile.bleft and !Tile.bright )
                {
                    // unmount Messie!
                    mounted->solid = !mounted->godmode;
                    mounted->beingteleported = false;

                    mounted->moveToForce((x+dx)<<CSF, (y+dy)<<CSF);

                    mounted = nullptr;
                    g_pInput->flushAll();
                    return true;
                }
            }
        }
    }
    return false;
}

void CMessie::process()
{    
    auto &evList = g_pBehaviorEngine->EventList();
    
    if( CPlayer::Mount *ev = evList.occurredEvent<CPlayer::Mount>() )
    {
        // Let's see if he can do that...
        if(tryMounting( const_cast<CPlayer&>(ev->player) ))
        {
            evList.pop_Event();
            // TODO: Create an Event here that hides all the Players
        }
        else
        {
            evList.pop_Event();
        }
    }
    
    // Search for the next where Messie has to swim
    if(destx == 0 && desty == 0)
    {
	nessie_find_next_checkpoint();
    }
    
    // animation
	sprite = baseframe + animframe;
	
	// Did player try to mount Messie?
	if (mounted)
	{
	    // Move mounted object with Messie. It still should be hidden at this point.
	    mounted->moveTo(m_Pos);
	    sprite += 8;
	    
	    // If first Player pushes the first button, unmount!
        if(g_pInput->getPressedCommand(IC_JUMP))
	    {
            tryToUnmount();
            g_pInput->flushAll();
	    }	    
	}
	
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
			    xDirection = LEFT, yDirection = UP;
			else if(goleft && godown)
			    xDirection = LEFT, yDirection = DOWN;
			else if(!goleft && !godown)
			    xDirection = RIGHT, yDirection = UP;
			else if(!goleft && godown)
			    xDirection = RIGHT, yDirection = DOWN;
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
	if (xDirection == LEFT && yDirection == DOWN)
		baseframe = NESSIE_DOWNLEFT_FRAME;
	else if (xDirection == RIGHT && yDirection == DOWN)
		baseframe = NESSIE_DOWNRIGHT_FRAME;
	else if (xDirection == LEFT && yDirection == UP)
		baseframe = NESSIE_UPLEFT_FRAME;
	else if (xDirection == RIGHT && yDirection == UP)
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
	int x,y;
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
				for(size_t i=0;i<NESSIETRAILLEN;i++)
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
