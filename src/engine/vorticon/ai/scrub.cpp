#include "../../../sdl/sound/CSound.h"
#include "CObjectAI.h"

// The red creatures that follow the wall (ep2)

enum scrub_actions{
	SCRUB_WALK,          // walking
	SCRUB_FALLING,       // oops, we fell off!
	SCRUB_DYING,         // getting fried!
	SCRUB_DEAD           // dead scrub! here's a dead scrub!
};

#define SCRUB_WALK_ANIM_TIME  11
#define SCRUB_WALK_SPEED      16

#define SCRUB_FALLSPDINCRATE   2
#define SCRUB_MIN_FALL_SPEED  40
#define SCRUB_MAX_FALL_SPEED  150

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

void CObjectAI::scrub_ai(CObject &object)
{
	if (object.needinit)
	{  // first time initialization
		object.ai.scrub.walkdir = LEFT;
		object.ai.scrub.state = SCRUB_FALLING;
		object.ai.scrub.walkframe = 0;
		object.ai.scrub.animtimer = 0;
		object.inhibitfall = true;
		object.needinit = false;
		object.canbezapped = 1;

		object.checkinitialCollisions();
		object.dead = 0;

		SetAllCanSupportPlayer(object, 1);
		for( size_t i=0 ; i<m_Player.size() ; i++ )
			object.ai.scrub.kickedplayer[i] = 0;

		return;
	}
	if (object.ai.scrub.state==SCRUB_DEAD)
	{
		object.hasbeenonscreen = 0;
		return;
	}

	// after kicking a player, wait until he falls beneath the scrub
	// before turning cansupportplayer back on...just in case we check
	// for some other things to (when he stops falling, or if he jumps)
	for(size_t p=0;p<m_Player.size();p++)
	{
		if (object.ai.scrub.kickedplayer[p])
		{
			if (m_Player[p].getYPosition() > object.getYPosition() || !m_Player[p].pfalling || m_Player[p].pjumping)
			{
				object.cansupportplayer = 1;
				object.ai.scrub.kickedplayer[p] = 0;
			}
		}
	}

	// Check if scrub can push one player
	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		if(object.touchPlayer)
		{
			if(it_player->getYDownPos() > object.getYMidPos())
			{
				if(it_player->getXMidPos() < object.getXMidPos() && object.ai.scrub.walkdir != UP)
					it_player->bump(-SCRUB_WALK_SPEED,true);
				else if (object.ai.scrub.walkdir != DOWN)
					it_player->bump(SCRUB_WALK_SPEED,true);

			}
		}
	}

	if (object.canbezapped)
	{
		// if we touch a glowcell, we die!
		int x = object.getXPosition();
		int y = object.getYPosition();
		if (mp_Map->at((x+256)>>CSF, (y+256)>>CSF)==TILE_GLOWCELL)
		{
			object.solid=true;
			object.ai.scrub.state = SCRUB_DYING;
			object.ai.scrub.dietimer = 0;
			object.zapped = 0;
			object.canbezapped = 0;
		}
		// die if shot
		if (object.zapped)
		{
			object.solid=true;
			object.ai.scrub.state = SCRUB_DYING;
			object.ai.scrub.dietimer = 0;
			object.zapped = 0;
			object.canbezapped = 0;
			object.moveUp(10);
			object.ai.scrub.scrubdie_inertia_y = SCRUBDIE_START_INERTIA;
			g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
		}
	}

	switch(object.ai.scrub.state)
	{
	case SCRUB_DYING:
		SetAllCanSupportPlayer(object, 0);
		object.sprite = SCRUB_FRY_FRAME;
		object.moveYDir(object.ai.scrub.scrubdie_inertia_y);
		if (object.ai.scrub.scrubdie_inertia_y < SCRUB_MAX_FALL_SPEED)
			object.ai.scrub.scrubdie_inertia_y+=2;

		object.ai.scrub.dietimer = 0;
		if (object.ai.scrub.scrubdie_inertia_y >= 0 && object.blockedd)
		{
			object.sprite = SCRUB_DEAD_FRAME;
			object.ai.scrub.state = SCRUB_DEAD;
			object.dead = true;
			SetAllCanSupportPlayer(object, 0);
		}
		return;
		break;
	case SCRUB_WALK:

		switch(object.ai.scrub.walkdir)
		{
		case LEFT:
			walkLeft(object, (object.getXMidPos())>>CSF, (object.getYMidPos())>>CSF);
			break;
		case RIGHT:
			walkRight(object, (object.getXMidPos())>>CSF, (object.getYMidPos())>>CSF);
			break;
		case DOWN:
			walkDown(object);
			break;
		case UP:
			walkUp(object);
			break;
		}

		// walk animation
		if (object.ai.scrub.animtimer > SCRUB_WALK_ANIM_TIME)
		{
			object.ai.scrub.walkframe ^= 1;
			object.ai.scrub.animtimer = 0;
		} else object.ai.scrub.animtimer++;
		break;

	case SCRUB_FALLING:
		fall(object);
		break;
	}

	//setupDirection(object);
}

/*
 * Makes scrub walk to the left
 */
void CObjectAI::walkLeft(CObject &object, int mx, int my)
{
	object.sprite = SCRUB_WALK_LEFT + object.ai.scrub.walkframe;

	if (object.blockedl)
	{
		object.sprite = SCRUB_WALK_UP + object.ai.scrub.walkframe;
		object.ai.scrub.walkdir = UP;
		Scrub_TurnOnCansupportWhereNotKicked(object);
	}
	else
	{
		object.moveLeft(SCRUB_WALK_SPEED);

		if(!object.blockedd)
		{
			// First check, if he can walk over the tile
			std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
			if(!TileProperties[mp_Map->at(mx-1, my+1)].bup)
			{
				// There is no gap
				object.moveDown(2<<STC);
				object.moveRight(2<<STC);
				if(object.blockedr)
					object.ai.scrub.walkdir = DOWN;
				else
					preparetoFall(object);
			}
		}

		std::vector<CPlayer>::iterator it_player = m_Player.begin();
		for( ; it_player != m_Player.end() ; it_player++ )
		{
			if ( it_player->supportedbyobject && it_player->psupportingobject==object.m_index &&
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
void CObjectAI::walkDown(CObject &object)
{
	std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
	object.sprite = SCRUB_WALK_DOWN + object.ai.scrub.walkframe;

	if (object.blockedd)
	{
		object.ai.scrub.walkdir = LEFT;
		object.sprite = SCRUB_WALK_LEFT + object.ai.scrub.walkframe;
		Scrub_TurnOnCansupportWhereNotKicked(object);
	}
	else
	{
		object.moveDown(SCRUB_WALK_SPEED);

		if(!object.blockedr) // upper-right, if yes, go right! (ceiling)
		{	// Move right
			object.ai.scrub.walkdir = RIGHT;
			object.sprite = SCRUB_WALK_RIGHT + object.ai.scrub.walkframe;
			object.moveRight(2<<STC);
			object.moveUp(2<<STC);
			SetAllCanSupportPlayer(object, 0);
		}

		std::vector<CPlayer>::iterator it_player = m_Player.begin();
		for( ; it_player != m_Player.end() ; it_player++ )
		{
			if (it_player->supportedbyobject && it_player->psupportingobject==object.m_index &&
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
void CObjectAI::walkRight(CObject &object, int mx, int my)
{
	object.sprite = SCRUB_WALK_RIGHT + object.ai.scrub.walkframe;

	if (object.blockedr)
	{
		object.ai.scrub.walkdir = DOWN;
		object.sprite = SCRUB_WALK_DOWN + object.ai.scrub.walkframe;
		Scrub_TurnOnCansupportWhereNotKicked(object);
	}
	else
	{
		object.moveRight(SCRUB_WALK_SPEED);

		if(!object.blockedu)
		{
			// First check, if he can walk over the tile
			std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
			if(!TileProperties[mp_Map->at(mx+1, my-1)].bdown)
			{
				// There is no gap the upper-side
				object.moveRight(1<<STC);
				object.moveUp(2<<STC);
				object.moveLeft(2<<STC);
				Scrub_TurnOnCansupportWhereNotKicked(object);
				if(object.blockedl)
					object.ai.scrub.walkdir = UP;
				else
					preparetoFall(object);
			}
		}
	}
}

/*
 * Makes scrub walk to the up
 */
void CObjectAI::walkUp(CObject &object)
{
	object.sprite = SCRUB_WALK_UP + object.ai.scrub.walkframe;
	if (object.blockedu)
	{
		object.ai.scrub.walkdir = RIGHT;
		object.sprite = SCRUB_WALK_RIGHT + object.ai.scrub.walkframe;
		SetAllCanSupportPlayer(object, 0);
	}
	else
	{
		object.moveUp(SCRUB_WALK_SPEED);

		if(	!object.blockedl )
		{	// Move Left!
			object.ai.scrub.walkdir = LEFT;
			object.sprite = SCRUB_WALK_LEFT + object.ai.scrub.walkframe;
			Scrub_TurnOnCansupportWhereNotKicked(object);
			object.moveLeft(2<<STC);
			object.moveDown(2<<STC);
		}

		std::vector<CPlayer>::iterator it_player = m_Player.begin();
		for( ; it_player != m_Player.end() ; it_player++ )
		{
			if (it_player->supportedbyobject && it_player->psupportingobject==object.m_index &&
					it_player->pjumping!=PJUMPUP && it_player->pjumping!=PPOGOING)
			{
				// kick player off if we're running him into the ceiling
				if (it_player->blockedu)
				{
					object.cansupportplayer = 0;
					object.ai.scrub.kickedplayer[it_player->m_index] = 1;
				}
				else it_player->moveUp(SCRUB_WALK_SPEED);
			}
		}
	}
}

/*
 * Makes scrub fall
 */
void CObjectAI::fall(CObject &object)
{
	object.sprite = SCRUB_WALK_DOWN;
	if (object.blockedd)
	{
		object.ai.scrub.walkdir = LEFT;
		object.ai.scrub.state = SCRUB_WALK;
		object.ai.scrub.walkframe = 0;
		object.ai.scrub.animtimer = 0;
		Scrub_TurnOnCansupportWhereNotKicked(object);
	}
	else
	{
		if (object.ai.scrub.fallinctimer > SCRUB_FALLSPDINCRATE)
		{
			if (object.ai.scrub.fallspeed < SCRUB_MAX_FALL_SPEED)
			{
				object.ai.scrub.fallspeed++;
			}
			object.ai.scrub.fallinctimer = 0;
		} else object.ai.scrub.fallinctimer++;

		object.moveDown(object.ai.scrub.fallspeed);
	}
}
/**
 * Prepares the grub to fall!
 */
void CObjectAI::preparetoFall(CObject &object)
{
	object.ai.scrub.fallinctimer = 0;
	object.ai.scrub.fallspeed = SCRUB_MIN_FALL_SPEED;
	object.ai.scrub.state = SCRUB_FALLING;
	object.ai.scrub.walkdir = LEFT;
	SetAllCanSupportPlayer(object, 0);
}


void CObjectAI::Scrub_TurnOnCansupportWhereNotKicked(CObject &object)
{
	for(size_t i=0;i<m_Player.size();i++)
	{
		if (!object.ai.scrub.kickedplayer[i])
			object.cansupportplayer = 1;
	}
}

