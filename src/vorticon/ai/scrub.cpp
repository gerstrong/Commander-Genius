#include "../../sdl/sound/CSound.h"
#include "../../keen.h"
#include "../../game.h"

//#include "enemyai.h"

// The red creatures that follow the wall (ep2)

#define SCRUB_WALK     0     // walking
#define SCRUB_FALLING  1     // oops, we fell off!
#define SCRUB_DYING    2     // getting fried!
#define SCRUB_DEAD     3     // dead scrub! here's a dead scrub!

#define SCRUB_WALK_ANIM_TIME  50
#define SCRUB_WALK_SPEED      4

#define SCRUB_FALLSPDINCRATE   2
#define SCRUB_MIN_FALL_SPEED  10
#define SCRUB_MAX_FALL_SPEED  25

#define SCRUBDIE_START_INERTIA      -10
#define SCRUBDIE_INERTIA_DECREASE    2

#define SCRUBPUSHAMOUNT       7

// frames
#define SCRUB_WALK_LEFT       102
#define SCRUB_WALK_UP         104
#define SCRUB_WALK_RIGHT      106
#define SCRUB_WALK_DOWN       108
#define SCRUB_FRY_FRAME       110
#define SCRUB_DEAD_FRAME      111

#define Sprite g_pGfxEngine->Sprite

void Scrub_TurnOnCansupportWhereNotKicked(int o);

// Reference to ../game.cpp
void bumpplayer(int p, int pushamt, bool solid);
/*

void CObjectAI::scrub_ai(CObject *p_object)
{
	unsigned int p;
	unsigned int i=0;
	int nopush;
	int floor;
	bool walkovertile=false;

	if (p_object->needinit)
	{  // first time initialization
		p_object->ai.scrub.walkdir = LEFT;
		p_object->ai.scrub.state = SCRUB_WALK;
		p_object->ai.scrub.walkframe = 0;
		p_object->ai.scrub.animtimer = 0;
		p_object->inhibitfall = 1;
		p_object->needinit = 0;
		p_object->canbezapped = 1;
		p_object->y = (p_object->y>>CSF>>4)<<4<<CSF;
		p_object->blockedd = 1;
		p_object->blockedl = 0;
		p_object->blockedr = 0;
		p_object->blockedu = 0;
		p_object->dead = 0;

		SetAllCanSupportPlayer(p_object, 1);
		for(i=0;i<numplayers;i++)
		{
			p_object->ai.scrub.kickedplayer[i] = 0;
		}
	}
	if (p_object->ai.scrub.state==SCRUB_DEAD)
	{
		p_object->hasbeenonscreen = 0;
		return;
	}

	// after kicking a player, wait until he falls beneath the scrub
	// before turning cansupportplayer back on...just in case we check
	// for some other things to (when he stops falling, or if he jumps)
	for(p=0;p<numplayers;p++)
	{
		if (p_object->ai.scrub.kickedplayer[p])
		{
			if (player[p].y > p_object->y || !player[p].pfalling || player[p].pjumping)
			{
				p_object->cansupportplayer[p] = 1;
				p_object->ai.scrub.kickedplayer[p] = 0;
			}
		}
	}

	// push player horizontally
	if (p_object->touchPlayer && !player[p_object->touchedBy].pdie &&\
			player[i].psupportingobject!=o)
	{
		nopush = 0;
		// don't push the player if he's standing on top of the scrub
		if (player[p_object->touchedBy].pfalling)
		{
			if (objects[0].scry+Sprite[0]->getHeight() >
			p_object->scry+(Sprite[p_object->sprite]->getHeight()>>1))
			{
				nopush = 1;
			}
		}
		// don't push the player as he falls through us during a kick-off
		else if (p_object->ai.scrub.kickedplayer[p_object->touchedBy])
		{
			nopush = 1;
		}
		// don't push the player as he's walking through the exit door
		else if (levelcontrol.level_done && levelcontrol.level_finished_by == p_object->touchedBy)
		{
			nopush = 1;
		}

		if (!nopush)
		{
			if (player[p_object->touchedBy].x < p_object->x)
				bumpplayer(p_object->touchedBy, -SCRUBPUSHAMOUNT, 1);
			else
				bumpplayer(p_object->touchedBy, SCRUBPUSHAMOUNT, 1);
		}
	}

	if (p_object->canbezapped)
	{
		// if we touch a glowcell, we die!
		if (getmaptileat((p_object->x>>CSF)+8, (p_object->y>>CSF)+8)==TILE_GLOWCELL)
		{
			p_object->ai.scrub.state = SCRUB_DYING;
			p_object->ai.scrub.dietimer = 0;
			p_object->zapped = 0;
			p_object->canbezapped = 0;
		}
		// die if shot
		if (p_object->zapped)
		{
			p_object->ai.scrub.state = SCRUB_DYING;
			p_object->ai.scrub.dietimer = 0;
			p_object->zapped = 0;
			p_object->canbezapped = 0;
			p_object->y -= 10;
			p_object->ai.scrub.scrubdie_inertia_y = SCRUBDIE_START_INERTIA;
			g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, p_object->scrx);
		}
	}

	switch(p_object->ai.scrub.state)
	{
	case SCRUB_DYING:
		SetAllCanSupportPlayer(o, 0);
		p_object->sprite = SCRUB_FRY_FRAME;
		p_object->y += p_object->ai.scrub.scrubdie_inertia_y;
		if (p_object->ai.scrub.dietimer>SCRUBDIE_INERTIA_DECREASE)
		{
			if (p_object->ai.scrub.scrubdie_inertia_y < SCRUB_MAX_FALL_SPEED)
			{
				p_object->ai.scrub.scrubdie_inertia_y++;
			}
			p_object->ai.scrub.dietimer = 0;
		}
		else p_object->ai.scrub.dietimer++;
		if (p_object->ai.scrub.scrubdie_inertia_y >= 0 && p_object->blockedd)
		{
			p_object->sprite = SCRUB_DEAD_FRAME;
			p_object->ai.scrub.state = SCRUB_DEAD;
			p_object->y = (p_object->y>>CSF>>4)<<4<<CSF;
			p_object->dead = 1;
			SetAllCanSupportPlayer(o, 0);
		}
		return;
		break;
	case SCRUB_WALK:

		switch(p_object->ai.scrub.walkdir)
		{
		case LEFT:
			p_object->sprite = SCRUB_WALK_LEFT + p_object->ai.scrub.walkframe;

			walkovertile = (TileProperty[getmaptileat((p_object->x>>CSF)-8, (p_object->y>>CSF)+16)][BUP] != 0);
			if (!p_object->blockedd && !walkovertile)
			{ // walked off the edge
				p_object->sprite = SCRUB_WALK_DOWN + p_object->ai.scrub.walkframe;
				p_object->ai.scrub.walkdir = DOWN;
				p_object->y += (2<<CSF);
				for(i=0;i<numplayers;i++)
				{
					if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
					{
						player[i].x -= (1<<CSF);
						player[i].y += (2<<CSF);
					}
				}
				common_enemy_ai(o);                // recalculate blockedx's
				Scrub_TurnOnCansupportWhereNotKicked(o);
			}
			else if (p_object->blockedl)
			{
				p_object->sprite = SCRUB_WALK_UP + p_object->ai.scrub.walkframe;
				p_object->ai.scrub.walkdir = UP;
				Scrub_TurnOnCansupportWhereNotKicked(o);
			}
			else
			{
				p_object->x -= SCRUB_WALK_SPEED;
				for(i=0;i<numplayers;i++)
				{
					if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
					{
						if (!player[i].blockedl)
						{
							player[i].x -= SCRUB_WALK_SPEED;
						}
					}
				}
			}
			break;
		case RIGHT:
			p_object->sprite = SCRUB_WALK_RIGHT + p_object->ai.scrub.walkframe;

			if (!p_object->blockedu)
			{
				p_object->ai.scrub.walkdir = UP;
				p_object->sprite = SCRUB_WALK_UP + p_object->ai.scrub.walkframe;
				Scrub_TurnOnCansupportWhereNotKicked(o);
				p_object->y -= (2<<CSF);
				common_enemy_ai(o);                // recalculate blockedx's
			}
			else if (p_object->blockedr)
			{
				p_object->ai.scrub.walkdir = DOWN;
				p_object->sprite = SCRUB_WALK_DOWN + p_object->ai.scrub.walkframe;
				Scrub_TurnOnCansupportWhereNotKicked(o);
			}
			else
			{
				p_object->x += SCRUB_WALK_SPEED;
			}
			break;
		case DOWN:
			p_object->sprite = SCRUB_WALK_DOWN + p_object->ai.scrub.walkframe;
			if (!p_object->blockedr)
			{
				p_object->ai.scrub.walkdir = RIGHT;
				p_object->sprite = SCRUB_WALK_RIGHT + p_object->ai.scrub.walkframe;
				SetAllCanSupportPlayer(o, 0);
				p_object->y += (1<<CSF);
				p_object->x += (2<<CSF);
				common_enemy_ai(o);
			}
			else if (p_object->blockedd)
			{
				p_object->ai.scrub.walkdir = LEFT;
				p_object->sprite = SCRUB_WALK_LEFT + p_object->ai.scrub.walkframe;
				Scrub_TurnOnCansupportWhereNotKicked(o);
			}
			else
			{
				p_object->y += SCRUB_WALK_SPEED;
				for(i=0;i<numplayers;i++)
				{
					if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
					{
						// ensure that player is not blocked by a floor (can happen
						// in certain situations if player is hanging off the right side
						// of the scrub a bit)
						floor = 0;
						if (!TileProperty[getmaptileat((player[i].x>>CSF)+4, (player[i].y>>CSF)+Sprite[0]->getHeight())][BUP])
						{ // lower-left isn't solid
							if (TileProperty[getmaptileat((player[i].x>>CSF)+12, (player[i].y>>CSF)+Sprite[0]->getHeight())][BUP])
							{
								floor = 1;
							}
						}
						else floor = 1;

						if (!floor)
						{
							player[i].y += SCRUB_WALK_SPEED;
						}
					}
				}
			}
			break;
		case UP:
			p_object->sprite = SCRUB_WALK_UP + p_object->ai.scrub.walkframe;
			if (!p_object->blockedl)
			{
				p_object->ai.scrub.walkdir = LEFT;
				p_object->sprite = SCRUB_WALK_LEFT + p_object->ai.scrub.walkframe;
				Scrub_TurnOnCansupportWhereNotKicked(o);
				p_object->x -= (2<<CSF);
				p_object->y = (((p_object->y>>CSF>>4)<<4)+1)<<CSF;
				common_enemy_ai(o);                // recalculate blockedx's
				for(i=0;i<numplayers;i++)
				{
					if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
					{
						player[i].x -= (2<<CSF);
						player[i].y = (p_object->y - (Sprite[0]->getHeight()<<CSF));
					}
				}
			}
			else if (p_object->blockedu)
			{
				p_object->ai.scrub.walkdir = RIGHT;
				p_object->sprite = SCRUB_WALK_RIGHT + p_object->ai.scrub.walkframe;
				SetAllCanSupportPlayer(o, 0);
			}
			{
				p_object->y -= SCRUB_WALK_SPEED;
				for(i=0;i<numplayers;i++)
				{
					if (player[i].psupportingobject==o && player[i].pjumping!=PJUMPUP && player[i].pjumping!=PPOGOING)
					{
						// kick player off if we're running him into the ceiling
						if (player[i].blockedu)
						{
							p_object->cansupportplayer[i] = 0;
							p_object->ai.scrub.kickedplayer[i] = 1;
						}
						else player[i].y -= SCRUB_WALK_SPEED;
					}
				}
			}
			break;
		}

		// walk animation
		if (p_object->ai.scrub.animtimer > SCRUB_WALK_ANIM_TIME)
		{
			p_object->ai.scrub.walkframe ^= 1;
			p_object->ai.scrub.animtimer = 0;
		} else p_object->ai.scrub.animtimer++;
		break;
		case SCRUB_FALLING:
			p_object->sprite = SCRUB_WALK_DOWN;
			if (p_object->blockedd)
			{
				p_object->ai.scrub.walkdir = LEFT;
				p_object->ai.scrub.state = SCRUB_WALK;
				p_object->ai.scrub.walkframe = 0;
				p_object->ai.scrub.animtimer = 0;
				Scrub_TurnOnCansupportWhereNotKicked(o);
			}
			else
			{
				if (p_object->ai.scrub.fallinctimer > SCRUB_FALLSPDINCRATE)
				{
					if (p_object->ai.scrub.fallspeed < SCRUB_MAX_FALL_SPEED)
					{
						p_object->ai.scrub.fallspeed++;
					}
					p_object->ai.scrub.fallinctimer = 0;
				} else p_object->ai.scrub.fallinctimer++;

				p_object->y += p_object->ai.scrub.fallspeed;
			}
			break;
	}

	// sometimes it's possible for a scrub to fall off, for example
	// if it's walking left on a platform that has solidfall but not solidlr,
	// then turns to walk down. in this case we need to go into a falling
	// state and fall until we reach solid ground again. this keeps it from
	// freaking out and going flying across the screen.
	if (!p_object->blockedl && !p_object->blockedr &&
			!p_object->blockedu && !p_object->blockedd && !walkovertile)
	{
		if (p_object->ai.scrub.state!=SCRUB_FALLING)
		{
			p_object->ai.scrub.fallinctimer = 0;
			p_object->ai.scrub.fallspeed = SCRUB_MIN_FALL_SPEED;
			p_object->ai.scrub.state = SCRUB_FALLING;
			SetAllCanSupportPlayer(o, 0);
		}
	}
}

void CObjectAI::Scrub_TurnOnCansupportWhereNotKicked(CObject &object)
{
	unsigned int i;
	for(i=0;i<numplayers;i++)
	{
		if (!object.ai.scrub.kickedplayer[i])
		{
			object.cansupportplayer[i] = 1;
		}
	}
}
*/
