#include "../../sdl/sound/CSound.h"
#include "../../keen.h"
#include "../../game.h"
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

#define SCRUB_FALLSPDINCRATE   8
#define SCRUB_MIN_FALL_SPEED  40
#define SCRUB_MAX_FALL_SPEED  100

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

#define Sprite g_pGfxEngine->Sprite

void CObjectAI::scrub_ai(CObject &object)
{
	unsigned int p;
	int i=0;
	int nopush;
	int floor;
	bool walkovertile=false;

	if (object.needinit)
	{  // first time initialization
		object.ai.scrub.walkdir = LEFT;
		object.ai.scrub.state = SCRUB_WALK;
		object.ai.scrub.walkframe = 0;
		object.ai.scrub.animtimer = 0;
		object.inhibitfall = 1;
		object.needinit = 0;
		object.canbezapped = 1;
		object.y = (object.y>>STC)<<STC;
		object.blockedd = 1;
		object.blockedl = 0;
		object.blockedr = 0;
		object.blockedu = 0;
		object.dead = 0;

		SetAllCanSupportPlayer(object, 1);
		for(i=0;i<m_NumPlayers;i++)
			object.ai.scrub.kickedplayer[i] = 0;

	}
	if (object.ai.scrub.state==SCRUB_DEAD)
	{
		object.hasbeenonscreen = 0;
		return;
	}

	// after kicking a player, wait until he falls beneath the scrub
	// before turning cansupportplayer back on...just in case we check
	// for some other things to (when he stops falling, or if he jumps)
	for(p=0;p<m_NumPlayers;p++)
	{
		if (object.ai.scrub.kickedplayer[p])
		{
			if (m_Player[p].y > object.y || !m_Player[p].pfalling || m_Player[p].pjumping)
			{
				object.cansupportplayer[p] = 1;
				object.ai.scrub.kickedplayer[p] = 0;
			}
		}
	}

	// push player horizontally
	if (object.touchPlayer && !m_Player[object.touchedBy].pdie &&\
			m_Player[i].psupportingobject!=object.m_index)
	{
		nopush = 0;
		// don't push the player if he's standing on top of the scrub
		if (m_Player[object.touchedBy].pfalling)
		{
			if (m_Objvect[0].scry+Sprite[0]->getHeight() >
			object.scry+(Sprite[object.sprite]->getHeight()>>1))
			{
				nopush = 1;
			}
		}
		// don't push the player as he falls through us during a kick-off
		else if (object.ai.scrub.kickedplayer[object.touchedBy])
		{
			nopush = 1;
		}
		// don't push the player as he's walking through the exit door
		/*else if (levelcontrol.level_done && levelcontrol.level_finished_by == object.touchedBy)
		{
			nopush = 1;
		}
		 */
		if (!nopush)
			m_Player[object.touchedBy].bump( (m_Player[object.touchedBy].x < object.x) ? -SCRUBPUSHAMOUNT : SCRUBPUSHAMOUNT, 1);
	}

	if (object.canbezapped)
	{
		// if we touch a glowcell, we die!
		if (mp_Map->at((object.x+256)>>CSF, (object.y+256)>>CSF)==TILE_GLOWCELL)
		{
			object.ai.scrub.state = SCRUB_DYING;
			object.ai.scrub.dietimer = 0;
			object.zapped = 0;
			object.canbezapped = 0;
		}
		// die if shot
		if (object.zapped)
		{
			object.ai.scrub.state = SCRUB_DYING;
			object.ai.scrub.dietimer = 0;
			object.zapped = 0;
			object.canbezapped = 0;
			object.y -= 10;
			object.ai.scrub.scrubdie_inertia_y = SCRUBDIE_START_INERTIA;
			g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
		}
	}

	stTile *TileProperty = g_pGfxEngine->Tilemap->mp_tiles;

	switch(object.ai.scrub.state)
	{
	case SCRUB_DYING:
		SetAllCanSupportPlayer(object, 0);
		object.sprite = SCRUB_FRY_FRAME;
		object.y += object.ai.scrub.scrubdie_inertia_y;
		if (object.ai.scrub.dietimer>SCRUBDIE_INERTIA_DECREASE)
		{
			if (object.ai.scrub.scrubdie_inertia_y < SCRUB_MAX_FALL_SPEED)
			{
				object.ai.scrub.scrubdie_inertia_y++;
			}
			object.ai.scrub.dietimer = 0;
		}
		else object.ai.scrub.dietimer++;
		if (object.ai.scrub.scrubdie_inertia_y >= 0 && object.blockedd)
		{
			object.sprite = SCRUB_DEAD_FRAME;
			object.ai.scrub.state = SCRUB_DEAD;
			object.y = (object.y>>STC)<<STC;
			object.dead = 1;
			SetAllCanSupportPlayer(object, 0);
		}
		return;
		break;
	case SCRUB_WALK:

		switch(object.ai.scrub.walkdir)
		{
		case LEFT:
			object.sprite = SCRUB_WALK_LEFT + object.ai.scrub.walkframe;

			walkovertile = TileProperty[mp_Map->at((object.x-256)>>CSF, (object.y+16)>>CSF)].bup;
			if (!object.blockedd && !walkovertile)
			{ // walked off the edge
				object.sprite = SCRUB_WALK_DOWN + object.ai.scrub.walkframe;
				object.ai.scrub.walkdir = DOWN;
				object.y += (2<<STC);
				for(i=0;i<m_NumPlayers;i++)
				{
					if (m_Player[i].psupportingobject==object.m_index && m_Player[i].pjumping!=PJUMPUP && m_Player[i].pjumping!=PPOGOING)
					{
						m_Player[i].x -= (1<<STC);
						m_Player[i].y += (2<<STC);
					}
				}
				object.performCollision(mp_Map);
				//common_enemy_ai(o);                // recalculate blockedx's
				Scrub_TurnOnCansupportWhereNotKicked(object);
			}
			else if (object.blockedl)
			{
				object.sprite = SCRUB_WALK_UP + object.ai.scrub.walkframe;
				object.ai.scrub.walkdir = UP;
				Scrub_TurnOnCansupportWhereNotKicked(object);
			}
			else
			{
				object.x -= SCRUB_WALK_SPEED;
				for(i=0;i<m_NumPlayers;i++)
				{
					if (m_Player[i].psupportingobject==object.m_index && m_Player[i].pjumping!=PJUMPUP && m_Player[i].pjumping!=PPOGOING)
					{
						if (!m_Player[i].blockedl)
						{
							m_Player[i].x -= SCRUB_WALK_SPEED;
						}
					}
				}
			}
			break;
		case RIGHT:
			object.sprite = SCRUB_WALK_RIGHT + object.ai.scrub.walkframe;

			if (!object.blockedu)
			{
				object.ai.scrub.walkdir = UP;
				object.sprite = SCRUB_WALK_UP + object.ai.scrub.walkframe;
				Scrub_TurnOnCansupportWhereNotKicked(object);
				object.y -= (2<<STC);
				object.performCollision(mp_Map);
				//common_enemy_ai(o);                // recalculate blockedx's
			}
			else if (object.blockedr)
			{
				object.ai.scrub.walkdir = DOWN;
				object.sprite = SCRUB_WALK_DOWN + object.ai.scrub.walkframe;
				Scrub_TurnOnCansupportWhereNotKicked(object);
			}
			else
			{
				object.x += SCRUB_WALK_SPEED;
			}
			break;
		case DOWN:
			object.sprite = SCRUB_WALK_DOWN + object.ai.scrub.walkframe;
			if (!object.blockedr)
			{
				object.ai.scrub.walkdir = RIGHT;
				object.sprite = SCRUB_WALK_RIGHT + object.ai.scrub.walkframe;
				SetAllCanSupportPlayer(object, 0);
				object.y += (1<<STC);
				object.x += (2<<STC);
				object.performCollision(mp_Map);
				//common_enemy_ai(o);
			}
			else if (object.blockedd)
			{
				object.ai.scrub.walkdir = LEFT;
				object.sprite = SCRUB_WALK_LEFT + object.ai.scrub.walkframe;
				Scrub_TurnOnCansupportWhereNotKicked(object);
			}
			else
			{
				object.y += SCRUB_WALK_SPEED;
				for(i=0;i<m_NumPlayers;i++)
				{
					if (m_Player[i].psupportingobject==object.m_index && m_Player[i].pjumping!=PJUMPUP && m_Player[i].pjumping!=PPOGOING)
					{
						// ensure that player is not blocked by a floor (can happen
						// in certain situations if player is hanging off the right side
						// of the scrub a bit)
						floor = 0;
						if (!TileProperty[mp_Map->at((m_Player[i].x+128)>>CSF, (m_Player[i].y+m_Player[i].h)>>CSF)].bup)
						{ // lower-left isn't solid
							if (TileProperty[mp_Map->at((m_Player[i].x+384)>>CSF, (m_Player[i].y+m_Player[i].h)>>CSF)].bup)
								floor = 1;
						}
						else floor = 1;

						if (!floor)
						{
							m_Player[i].y += SCRUB_WALK_SPEED;
						}
					}
				}
			}
			break;
		case UP:
			object.sprite = SCRUB_WALK_UP + object.ai.scrub.walkframe;
			if (!object.blockedl)
			{
				object.ai.scrub.walkdir = LEFT;
				object.sprite = SCRUB_WALK_LEFT + object.ai.scrub.walkframe;
				Scrub_TurnOnCansupportWhereNotKicked(object);
				object.x -= (2<<STC);
				object.y = (((object.y>>CSF)<<TILE_S)+1)<<STC;
				object.performCollision(mp_Map);
				//common_enemy_ai(o);                // recalculate blockedx's
				for(i=0;i<m_NumPlayers;i++)
				{
					if (m_Player[i].psupportingobject==object.m_index && m_Player[i].pjumping!=PJUMPUP && m_Player[i].pjumping!=PPOGOING)
					{
						m_Player[i].x -= (2<<STC);
						m_Player[i].y = (object.y - (Sprite[0]->getHeight()<<STC));
					}
				}
			}
			else if (object.blockedu)
			{
				object.ai.scrub.walkdir = RIGHT;
				object.sprite = SCRUB_WALK_RIGHT + object.ai.scrub.walkframe;
				SetAllCanSupportPlayer(object, 0);
			}
			{
				object.y -= SCRUB_WALK_SPEED;
				for(i=0;i<m_NumPlayers;i++)
				{
					if (m_Player[i].psupportingobject==object.m_index && m_Player[i].pjumping!=PJUMPUP && m_Player[i].pjumping!=PPOGOING)
					{
						// kick player off if we're running him into the ceiling
						if (m_Player[i].blockedu)
						{
							object.cansupportplayer[i] = 0;
							object.ai.scrub.kickedplayer[i] = 1;
						}
						else m_Player[i].y -= SCRUB_WALK_SPEED;
					}
				}
			}
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

				object.y += object.ai.scrub.fallspeed;
			}
			break;
	}

	// sometimes it's possible for a scrub to fall off, for example
	// if it's walking left on a platform that has solidfall but not solidlr,
	// then turns to walk down. in this case we need to go into a falling
	// state and fall until we reach solid ground again. this keeps it from
	// freaking out and going flying across the screen.
	if (!object.blockedl && !object.blockedr &&
			!object.blockedu && !object.blockedd && !walkovertile)
	{
		if (object.ai.scrub.state!=SCRUB_FALLING)
		{
			object.ai.scrub.fallinctimer = 0;
			object.ai.scrub.fallspeed = SCRUB_MIN_FALL_SPEED;
			object.ai.scrub.state = SCRUB_FALLING;
			SetAllCanSupportPlayer(object, 0);
		}
	}
}

void CObjectAI::Scrub_TurnOnCansupportWhereNotKicked(CObject &object)
{
	unsigned int i;
	for(i=0;i<m_NumPlayers;i++)
	{
		if (!object.ai.scrub.kickedplayer[i])
			object.cansupportplayer[i] = 1;
	}
}

