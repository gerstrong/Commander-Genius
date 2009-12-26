#include "../../sdl/sound/CSound.h"
#include "../../graphics/CGfxEngine.h"
#include "CObjectAI.h"

// AI for the foobs (yellow "scaredy cat" creatures) (ep3)
enum FOOB_ACTIONS{
	FOOB_WALK,
	FOOB_SPOOK,
	FOOB_FLEE,
	FOOB_EXPLODE,
	FOOB_DEAD
};

#define FOOB_WALK_SPEED      32
#define FOOB_WALK_ANIM_RATE  4

#define FOOB_FLEE_SPEED      77
#define FOOB_FLEE_ANIM_RATE  4

#define FOOB_SPOOK_SHOW_TIME    12

#define FOOB_HARDMODE_BLOCK_TIME	35

#define FOOB_EXPLODE_ANIM_RATE  8

#define FOOB_SPOOK_TIME         10
#define FOOB_RELAX_TIME         50

#define FOOB_WALK_LEFT_FRAME    93
#define FOOB_WALK_RIGHT_FRAME   95
#define FOOB_SPOOK_FRAME        97
#define FOOB_EXPLODE_FRAME      97
#define FOOB_DEAD_FRAME         101

void CObjectAI::foob_ai(CObject &object, bool hardmode)
{
	int onsamelevel;
	if (object.needinit)
	{
		object.ai.foob.state = FOOB_WALK;
		object.ai.foob.dir = RIGHT;
		object.ai.foob.animframe = 0;
		object.ai.foob.animtimer = 0;
		object.ai.foob.OnSameLevelTime = 0;
		object.blockedr = 0;
		object.canbezapped = 1;
		object.dead = 0;
		object.needinit = 0;
	}
	if (object.ai.foob.state==FOOB_DEAD) return;
	if (!object.hasbeenonscreen) return;


	if (object.zapped || object.touchPlayer)
	{
		if (object.ai.foob.state != FOOB_EXPLODE)
		{
			object.ai.foob.animframe = 0;
			object.ai.foob.animtimer = 0;
			object.ai.foob.state = FOOB_EXPLODE;
			object.canbezapped = 0;
			if (object.onscreen) g_pSound->playStereofromCoord(SOUND_YORP_DIE, PLAY_NOW, object.scrx);
			if (hardmode && object.touchPlayer)
			{
				killplayer(object.touchedBy);
			}
		}
	}

	// find out if a player is on the same level as the foob cat
	Uint16 player_height = g_pGfxEngine->Sprite[0]->getHeight()<<STC;
	Uint16 foob_height = object.bboxY2;
	onsamelevel = 0;
	for(size_t i=0;i<m_NumPlayers;i++)
	{
		if ( (m_Player[i].y >= object.y-(24<<STC)) &&
			(m_Player[i].y+player_height <= object.y+foob_height+(24<<STC)) )
		{
			onsamelevel = 1;
			object.ai.foob.SpookedByWho = i;
			break;
		}
	}

	switch(object.ai.foob.state)
	{
	case FOOB_WALK:
		// if player is on the same level for FOOB_SPOOK_TIME, run away
		if (onsamelevel)
		{
			if (object.ai.foob.OnSameLevelTime > FOOB_SPOOK_TIME)
			{
				object.ai.foob.state = FOOB_SPOOK;
				object.ai.foob.spooktimer = 0;
				if (object.onscreen) g_pSound->playStereofromCoord(SOUND_YORP_DIE, PLAY_NOW, object.scrx);
			}
			else object.ai.foob.OnSameLevelTime++;
		}
		else object.ai.foob.OnSameLevelTime = 0;

		if (object.ai.foob.dir == RIGHT)
		{  // walking right
			object.sprite = FOOB_WALK_RIGHT_FRAME + object.ai.foob.animframe;
			if (object.blockedr)
			{
				object.ai.foob.dir = LEFT;
			}
			else
			{
				object.x += FOOB_WALK_SPEED;
			}
		}
		else
		{  // walking left
			object.sprite = FOOB_WALK_LEFT_FRAME + object.ai.foob.animframe;
			if (object.blockedl)
			{
				object.ai.foob.dir = RIGHT;
			}
			else
			{
				object.x -= FOOB_WALK_SPEED;
			}
		}

		// walk animation
		if (object.ai.foob.animtimer > FOOB_WALK_ANIM_RATE)
		{
			object.ai.foob.animframe ^= 1;
			object.ai.foob.animtimer = 0;
		}
		else object.ai.foob.animtimer++;
		break;

	case FOOB_SPOOK:
		object.sprite = FOOB_SPOOK_FRAME;

		if (object.ai.foob.spooktimer > FOOB_SPOOK_SHOW_TIME)
		{
			object.ai.foob.state = FOOB_FLEE;
			object.ai.foob.OffOfSameLevelTime = 0;
			// run away from the offending player
			if (m_Player[object.ai.foob.SpookedByWho].x < object.x)
			{
				object.ai.foob.dir = RIGHT;
			}
			else
			{
				object.ai.foob.dir = LEFT;
			}
			// in hard mode run TOWARDS the player (he's deadly in hard mode)
			if (hardmode) object.ai.foob.dir ^= 1;

		}
		else object.ai.foob.spooktimer++;
		break;

	case FOOB_FLEE:
		// if player is off of the same level for FOOB_RELAX_TIME,
		// we can stop fleeing
		if (!onsamelevel)
		{
			if (object.ai.foob.OffOfSameLevelTime > FOOB_RELAX_TIME)
			{
				relax: ;
				object.ai.foob.state = FOOB_WALK;
				object.ai.foob.OnSameLevelTime = 0;
				break;
			}
			else object.ai.foob.OffOfSameLevelTime++;
		}
		else object.ai.foob.OffOfSameLevelTime = 0;

		if (object.ai.foob.dir == RIGHT)
		{  // walking right
			object.sprite = FOOB_WALK_RIGHT_FRAME + object.ai.foob.animframe;
			if (!object.blockedr)
			{
				object.x += FOOB_FLEE_SPEED;
				object.ai.foob.blockedtime = 0;
			}
			else if (hardmode)
			{
				if (++object.ai.foob.blockedtime >= FOOB_HARDMODE_BLOCK_TIME)
				{
					object.ai.foob.blockedtime = 0;
					goto relax;
				}
			}
		}
		else
		{  // walking left
			object.sprite = FOOB_WALK_LEFT_FRAME + object.ai.foob.animframe;
			if (!object.blockedl)
			{
				object.x -= FOOB_FLEE_SPEED;
				object.ai.foob.blockedtime = 0;
			}
			else if (hardmode)
			{
				if (++object.ai.foob.blockedtime >= FOOB_HARDMODE_BLOCK_TIME)
				{
					object.ai.foob.blockedtime = 0;
					goto relax;
				}
			}
		}

		// walk animation
		if (object.ai.foob.animtimer > FOOB_FLEE_ANIM_RATE)
		{
			object.ai.foob.animframe ^= 1;
			object.ai.foob.animtimer = 0;
		}
		else object.ai.foob.animtimer++;
		break;

	case FOOB_EXPLODE:
		// ahhhhh; I'm sorry.....you poor little thing......
		object.sprite = FOOB_EXPLODE_FRAME + object.ai.foob.animframe;
		if (object.sprite==FOOB_DEAD_FRAME)
		{
			object.ai.foob.state = FOOB_DEAD;
			object.dead = 1;
		}
		else
		{
			if (object.ai.foob.animtimer > FOOB_EXPLODE_ANIM_RATE)
			{
				object.ai.foob.animframe++;
				object.ai.foob.animtimer = 0;
			}
			else object.ai.foob.animtimer++;
		}
		break;
	}
}

