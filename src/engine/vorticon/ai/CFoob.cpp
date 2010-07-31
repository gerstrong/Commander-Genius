#include "CFoob.h"
#include "../../../sdl/sound/CSound.h"
#include "../../../graphics/CGfxEngine.h"

CFoob::CFoob(CMap *p_map, Uint32 x, Uint32 y, std::vector<CPlayer>& Player):
CObject(p_map,x,y, OBJ_FOOB),
m_Player(Player)
{
	state = FOOB_WALK;
	dir = RIGHT;
	animframe = 0;
	animtimer = 0;
	OnSameLevelTime = 0;
	blockedr = 0;
	canbezapped = 1;
	dead = 0;
}

void CFoob::process()
{
	bool onsamelevel;

	if (state==FOOB_DEAD) return;

	if (!hasbeenonscreen) return;

	if ( (HealthPoints <=0 && state != FOOB_EXPLODE) || touchPlayer)
	{
		if (state != FOOB_EXPLODE)
		{
			animframe = 0;
			animtimer = 0;
			state = FOOB_EXPLODE;
			g_pSound->playStereofromCoord(SOUND_YORP_DIE, PLAY_NOW, scrx);
		}
	}

	// find out if a player is on the same level as the foob cat
	onsamelevel = false;

	std::vector<CPlayer>::iterator it_player = m_Player.begin();
	for( ; it_player != m_Player.end() ; it_player++ )
	{
		if ( (it_player->getYUpPos() >= getYUpPos()-(2<<CSF)) &&
			(it_player->getYDownPos() <= getYDownPos()+(1<<CSF)) )
		{
			onsamelevel = true;
			SpookedByWho = it_player->m_index;
			break;
		}
	}

	switch(state)
	{
	case FOOB_WALK:
		// if player is on the same level for FOOB_SPOOK_TIME, run away
		if (onsamelevel)
		{
			if (OnSameLevelTime > FOOB_SPOOK_TIME)
			{
				state = FOOB_SPOOK;
				spooktimer = 0;
				if (onscreen) g_pSound->playStereofromCoord(SOUND_YORP_DIE, PLAY_NOW, scrx);
			}
			else OnSameLevelTime++;
		}
		else OnSameLevelTime = 0;

		if (dir == RIGHT)
		{  // walking right
			sprite = FOOB_WALK_RIGHT_FRAME + animframe;
			if (blockedr)
			{
				dir = LEFT;
			}
			else
			{
				moveRight(FOOB_WALK_SPEED);
			}
		}
		else
		{  // walking left
			sprite = FOOB_WALK_LEFT_FRAME + animframe;
			if (blockedl)
			{
				dir = RIGHT;
			}
			else
			{
				moveLeft(FOOB_WALK_SPEED);
			}
		}

		// walk animation
		if (animtimer > FOOB_WALK_ANIM_RATE)
		{
			animframe ^= 1;
			animtimer = 0;
		}
		else animtimer++;
		break;

	case FOOB_SPOOK:
		sprite = FOOB_SPOOK_FRAME;

		if (spooktimer > FOOB_SPOOK_SHOW_TIME)
		{
			state = FOOB_FLEE;
			OffOfSameLevelTime = 0;
			// run away from the offending player
			if (m_Player[SpookedByWho].getXPosition() < getXPosition())
				dir = RIGHT;
			else
				dir = LEFT;
			// in hard mode run TOWARDS the player (he's deadly in hard mode)
			if (mp_Map->m_Difficulty>1)
				dir = LEFT ? RIGHT : LEFT;

		}
		else spooktimer++;
		break;

	case FOOB_FLEE:
		// if player is off of the same level for FOOB_RELAX_TIME,
		// we can stop fleeing
		if (!onsamelevel)
		{
			if (OffOfSameLevelTime > FOOB_RELAX_TIME)
			{
				relax: ;
				state = FOOB_WALK;
				OnSameLevelTime = 0;
				break;
			}
			else OffOfSameLevelTime++;
		}
		else OffOfSameLevelTime = 0;

		if (dir == RIGHT)
		{  // walking right
			sprite = FOOB_WALK_RIGHT_FRAME + animframe;
			if (!blockedr)
			{
				moveRight(FOOB_FLEE_SPEED);
				blockedtime = 0;
			}
			else if ((mp_Map->m_Difficulty>1))
			{
				if (++blockedtime >= FOOB_HARDMODE_BLOCK_TIME)
				{
					blockedtime = 0;
					goto relax;
				}
			}
		}
		else
		{  // walking left
			sprite = FOOB_WALK_LEFT_FRAME + animframe;
			if (!blockedl)
			{
				moveLeft(FOOB_FLEE_SPEED);
				blockedtime = 0;
			}
			else if ((mp_Map->m_Difficulty>1))
			{
				if (++blockedtime >= FOOB_HARDMODE_BLOCK_TIME)
				{
					blockedtime = 0;
					goto relax;
				}
			}
		}

		// walk animation
		if (animtimer > FOOB_FLEE_ANIM_RATE)
		{
			animframe ^= 1;
			animtimer = 0;
		}
		else animtimer++;
		break;

	case FOOB_EXPLODE:
		// ahhhhh; I'm sorry.....you poor little thing......
		sprite = FOOB_EXPLODE_FRAME + animframe;
		if (sprite==FOOB_DEAD_FRAME)
		{
			state = FOOB_DEAD;
			dead = 1;
		}
		else
		{
			if (animtimer > FOOB_EXPLODE_ANIM_RATE)
			{
				animframe++;
				animtimer = 0;
			}
			else animtimer++;
		}
		break;
	}
}

void CFoob::getTouchedBy(CObject &theObject)
{
	if(hitdetect(theObject))
	{
		if(theObject.m_type == OBJ_PLAYER)
		{
			CPlayer &Player = dynamic_cast<CPlayer&>(theObject);
			if(mp_Map->m_Difficulty>1)
				Player.kill();
		}
	}
}
