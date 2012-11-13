#include "../../../sdl/sound/CSound.h"
#include "CScrub.h"

// The red creatures that follow the wall (ep2)

enum scrub_actions{
	SCRUB_WALK,          // walking
	SCRUB_FALLING,       // oops, we fell off!
	SCRUB_DYING,         // getting fried!
	SCRUB_DEAD           // dead scrub! here's a dead scrub!
};

#define SCRUB_WALK_ANIM_TIME  11
#define SCRUB_WALK_SPEED      25

#define SCRUBDIE_START_INERTIA      -10
#define SCRUBDIE_INERTIA_DECREASE    2

#define SCRUBPUSHAMOUNT       28

// frames
#define SCRUB_WALK_LEFT       102
#define SCRUB_WALK_UP         104
#define SCRUB_WALK_RIGHT      106
#define SCRUB_WALK_DOWN       108
#define SCRUB_FRY_FRAME       110
#define SCRUB_DEAD_FRAME      111

CScrub::CScrub(CMap *p_map, Uint32 x, Uint32 y,
		std::vector<CPlayer>& Player) :
CVorticonSpriteObject(p_map, x, y, OBJ_SCRUB),
scrubdie_inertia_y(0),
m_Player(Player)
{
	xDirection = -1;
	state = SCRUB_FALLING;
	walkframe = 0;
	animtimer = 0;
	inhibitfall = true;
	canbezapped = true;

	performCollisions();
	dead = false;
	fallspeed = 0;

	SetAllCanSupportPlayer(1);
	for( size_t i=0 ; i<m_Player.size() ; i++ )
		kickedplayer[i] = 0;
}

void CScrub::process()
{
	// after kicking a player, wait until he falls beneath the scrub
	// before turning cansupportplayer back on...just in case we check
	// for some other things to (when he stops falling, or if he jumps)
	for(size_t p=0;p<m_Player.size();p++)
	{
		if (kickedplayer[p])
		{
			if (m_Player[p].getYPosition() > getYPosition() || !m_Player[p].pfalling || m_Player[p].pjumping)
			{
				cansupportplayer = 1;
				kickedplayer[p] = 0;
			}
		}
	}

	// Check if scrub can push one player
	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		if(touchPlayer)
		{
			if(it_player->getYDownPos() > getYMidPos())
			{
				if(it_player->getXMidPos() < getXMidPos() && yDirection >= 0 )
					it_player->push(*this);
				else if (yDirection <= 0)
					it_player->push(*this);

			}
		}
	}

	if (canbezapped)
	{
		// die if shot
		if (mHealthPoints <= 0 && state!=SCRUB_DYING )
		{
			solid=true;
			state = SCRUB_DYING;
			dietimer = 0;
			moveUp(10);
			scrubdie_inertia_y = SCRUBDIE_START_INERTIA;
			playSound(SOUND_SHOT_HIT);
		}
	}

	CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();

	switch(state)
	{
	case SCRUB_DYING:
		SetAllCanSupportPlayer(0);
		sprite = SCRUB_FRY_FRAME;
		moveYDir(scrubdie_inertia_y);
		if ( scrubdie_inertia_y < Physics.max_fallspeed )
			scrubdie_inertia_y += Physics.fallspeed_increase;

		dietimer = 0;
		if (scrubdie_inertia_y >= 0 && blockedd)
		{
			sprite = SCRUB_DEAD_FRAME;
			state = SCRUB_DEAD;
			dead = true;
			SetAllCanSupportPlayer(0);
		}
		return;
		break;
	case SCRUB_WALK:


		if(xDirection < 0)
			walkLeft( (getXLeftPos())>>CSF, (getYMidPos())>>CSF);
		else if(xDirection > 0)
			walkRight( (getXRightPos())>>CSF, (getYMidPos())>>CSF);
		else if(yDirection < 0)
			walkUp();
		else if(yDirection > 0)
			walkDown();

		// walk animation
		if (animtimer > SCRUB_WALK_ANIM_TIME)
		{
			walkframe ^= 1;
			animtimer = 0;
		} else animtimer++;
		break;

	case SCRUB_FALLING:
		fall();
		break;
	}
}

/*
 * Makes scrub walk to the left
 */
void CScrub::walkLeft(int mx, int my)
{
	sprite = SCRUB_WALK_LEFT + walkframe;

	if (blockedl)
	{
		sprite = SCRUB_WALK_UP + walkframe;
		xDirection = 0;
		yDirection = -1;
		Scrub_TurnOnCansupportWhereNotKicked();
	}
	else
	{
		moveLeft(SCRUB_WALK_SPEED);

		if(!blockedd)
		{
			// First check, if he can walk over the tile
			std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
			if(!TileProperties[mp_Map->at(mx-1, my+1)].bup &&
				!TileProperties[mp_Map->at(mx-1, my)].bleft)
			{
				// There is no gap
				processMove(0,4<<STC);
				processMove(4<<STC,0);
				performCollisions();
				if(blockedr)
				{
					xDirection = 0;
					yDirection = 1;
				}
				else
					preparetoFall();
			}
		}

		std::vector<CPlayer>::iterator it_player = m_Player.begin();
		for( ; it_player != m_Player.end() ; it_player++ )
		{
			if ( it_player->pSupportedbyobject && it_player->psupportingobject==m_index &&
					it_player->pjumping!=PJUMPUP && it_player->pjumping!=PPOGOING)
			{
				if (!it_player->blockedl)
				{
					it_player->moveLeft(SCRUB_WALK_SPEED);
				}
				else
				{
					it_player->pfalling = true;
					it_player->moveUp(1);
					it_player->moveDown(1);
				}
			}
		}
	}
}

/*
 * Makes scrub walk to the down
 */
void CScrub::walkDown()
{
	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
	sprite = SCRUB_WALK_DOWN + walkframe;

	if (blockedd)
	{
		yDirection = 0;
		xDirection = -1;
		sprite = SCRUB_WALK_LEFT + walkframe;
		Scrub_TurnOnCansupportWhereNotKicked();
	}
	else
	{
		moveDown(SCRUB_WALK_SPEED);

		if(!blockedr) // upper-right, if yes, go right! (ceiling)
		{	// Move right
			yDirection = 0;
			xDirection = 1;
			sprite = SCRUB_WALK_RIGHT + walkframe;
			processMove(2<<STC,0);
			processMove(0,-(2<<STC));
			SetAllCanSupportPlayer(0);
		}

		std::vector<CPlayer>::iterator it_player = m_Player.begin();
		for( ; it_player != m_Player.end() ; it_player++ )
		{
			if (it_player->pSupportedbyobject && it_player->psupportingobject==m_index &&
					it_player->pjumping!=PJUMPUP && it_player->pjumping!=PPOGOING)
			{
				// ensure that player is not blocked by a floor (can happen
				// in certain situations if player is hanging off the right side
				// of the scrub a bit)
				bool floor = false;
				if (!TileProperties[mp_Map->at((it_player->getXPosition()+128)>>CSF, (it_player->getYDownPos())>>CSF)].bup)
				{ // lower-left isn't solid
					if (TileProperties[mp_Map->at((it_player->getXPosition()+384)>>CSF, (it_player->getYDownPos())>>CSF)].bup)
						floor = true;
				}
				else floor = false;

				if (!floor) it_player->moveDown(SCRUB_WALK_SPEED);
			}
		}
	}
}

/*
 * Makes scrub walk to the right
 */
void CScrub::walkRight(int mx, int my)
{
	sprite = SCRUB_WALK_RIGHT + walkframe;

	if (blockedr)
	{
		xDirection = 0;
		yDirection = 1;
		sprite = SCRUB_WALK_DOWN + walkframe;
		Scrub_TurnOnCansupportWhereNotKicked();
	}
	else
	{
		moveRight(SCRUB_WALK_SPEED);
		processMove(0,-(2<<STC));

		if(!blockedu)
		{
			// First check, if he can walk over the tile
			std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
			if(!TileProperties[mp_Map->at(mx+1, my-1)].bdown &&
				!TileProperties[mp_Map->at(mx+1, my)].bright)
			{
				// There is no gap the upper-side
				processMove(4<<STC,0);
				processMove(0,-(4<<STC));
				processMove(-(6<<STC),0);
				performCollisions();

				Scrub_TurnOnCansupportWhereNotKicked();
				if(blockedl)
				{
					xDirection = 0;
					yDirection = -1;
				}
				else
					preparetoFall();
			}
		}
	}
}

/*
 * Makes scrub walk to the up
 */
void CScrub::walkUp()
{
	sprite = SCRUB_WALK_UP + walkframe;
	if (blockedu)
	{
		yDirection = 0;
		xDirection = 1;
		sprite = SCRUB_WALK_RIGHT + walkframe;
		SetAllCanSupportPlayer(0);
	}
	else
	{
		moveUp(SCRUB_WALK_SPEED);

		if(	!blockedl )
		{	// Move Left!
			yDirection = 0;
			xDirection = -1;
			sprite = SCRUB_WALK_LEFT + walkframe;
			Scrub_TurnOnCansupportWhereNotKicked();
			processMove(-(2<<STC),0);
			processMove(0,2<<STC);
		}

		std::vector<CPlayer>::iterator it_player = m_Player.begin();
		for( ; it_player != m_Player.end() ; it_player++ )
		{
			if (it_player->pSupportedbyobject && it_player->psupportingobject==m_index &&
					it_player->pjumping!=PJUMPUP && it_player->pjumping!=PPOGOING)
			{
				// kick player off if we're running him into the ceiling
				if (it_player->blockedu)
				{
					cansupportplayer = 0;
					kickedplayer[it_player->m_index] = 1;
				}
				else it_player->moveUp(SCRUB_WALK_SPEED);
			}
		}
	}
}

/*
 * Makes scrub fall
 */
void CScrub::fall()
{
	sprite = SCRUB_WALK_LEFT;
	if (blockedd)
	{
		inhibitfall = true;
		canbezapped = true;

		performCollisions();
		dead = false;
		fallspeed = 0;

		SetAllCanSupportPlayer(1);
		for( size_t i=0 ; i<m_Player.size() ; i++ )
			kickedplayer[i] = 0;

		yDirection = 0;
		xDirection = -1;
		state = SCRUB_WALK;
		walkframe = 0;
		animtimer = 0;
		Scrub_TurnOnCansupportWhereNotKicked();
	}
	else
	{
		CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();

		if (fallspeed < Physics.max_fallspeed)
			fallspeed += Physics.fallspeed_increase;

		moveDown(fallspeed);
	}
}
/**
 * Prepares the grub to fall!
 */
void CScrub::preparetoFall()
{
	fallspeed = 0;
	state = SCRUB_FALLING;
	yDirection = 0;
	xDirection = -1;
	SetAllCanSupportPlayer(0);
}


void CScrub::Scrub_TurnOnCansupportWhereNotKicked()
{
	for(size_t i=0;i<m_Player.size();i++)
	{
		if (!kickedplayer[i])
			cansupportplayer = 1;
	}
}

void CScrub::SetAllCanSupportPlayer(bool state)
{
	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		cansupportplayer = state;
		if(!state && it_player->pSupportedbyobject)
		{
			it_player->pfalling=true;
			it_player->moveDown(1);
			it_player->blockedd=false;
		}
	}
}


