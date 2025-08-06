#include "CFoob.h"
#include <base/audio/Audio.h>
#include "graphics/GsGraphics.h"

CFoob::CFoob(std::shared_ptr<CMap> p_map, const int x, const int y):
CVorticonSpriteObject(p_map,x,y, OBJ_FOOB),
onsamelevel(false)
{
	state = FOOB_WALK;
	dir = RIGHT;
	animframe = 0;
	animtimer = 0;
	OnSameLevelTime = 0;
	blockedr = 0;
	canbezapped = 1;
	mIsDead = 0;

	if(gBehaviorEngine.mDifficulty==HARD)
		mHealthPoints++;
}

void CFoob::process()
{
	if (state==FOOB_DEAD) return;

	if (!hasbeenonscreen) return;

	if ( (mHealthPoints <=0 && state != FOOB_EXPLODE) || touchPlayer)
	{
		if (state != FOOB_EXPLODE)
		{
			animframe = 0;
			animtimer = 0;
			state = FOOB_EXPLODE;
            playSound(GameSound::YORP_DIE);
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
				if (onscreen)
                    playSound(GameSound::YORP_DIE);
			}
			else OnSameLevelTime++;
		}
		else OnSameLevelTime = 0;

		if (dir == RIGHT)
		{  // walking right
			mSpriteIdx = FOOB_WALK_RIGHT_FRAME + animframe;
			if (blockedr)
			{
				dir = LEFT;
			}
			else
			{
				xinertia = FOOB_WALK_SPEED;
			}
		}
		else
		{  // walking left
			mSpriteIdx = FOOB_WALK_LEFT_FRAME + animframe;
			if (blockedl)
			{
				dir = RIGHT;
			}
			else
			{
				xinertia = -FOOB_WALK_SPEED;
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
			mSpriteIdx = FOOB_WALK_RIGHT_FRAME + animframe;
			if (!blockedr)
			{
				xinertia = FOOB_FLEE_SPEED;
				blockedtime = 0;
			}
			else if(gBehaviorEngine.mDifficulty==HARD)
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
			mSpriteIdx = FOOB_WALK_LEFT_FRAME + animframe;
			if (!blockedl)
			{
				xinertia = -FOOB_FLEE_SPEED;
				blockedtime = 0;
			}
			else if(gBehaviorEngine.mDifficulty==HARD)
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
		mSpriteIdx = FOOB_EXPLODE_FRAME + animframe;
		if (mSpriteIdx==FOOB_DEAD_FRAME)
		{
			state = FOOB_DEAD;
			mIsDead = 1;
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
	case FOOB_DEAD: break;
	}
}

bool CFoob::isNearby(CSpriteObject &theObject)
{
    if(CPlayer *player = dynamic_cast<CPlayer*>(&theObject))
    {
        if ( (player->getYDownPos() >= getYUpPos()-(2<<CSF)) &&
             (player->getYDownPos() <= getYDownPos()+(1<<CSF)) )
        {
            onsamelevel = true;
            SpookedByWho = player->m_index;
        }

        if(state == FOOB_SPOOK)
        {
            mSpriteIdx = FOOB_SPOOK_FRAME;

            if (spooktimer > FOOB_SPOOK_SHOW_TIME)
            {
                state = FOOB_FLEE;
                OffOfSameLevelTime = 0;
                // run away from the offending player

                if (player->getXPosition() < getXPosition())
                    dir = RIGHT;
                else
                    dir = LEFT;

                // in hard mode run TOWARDS the player (he's deadly in hard mode)
                if (gBehaviorEngine.mDifficulty == HARD)
                {
                    dir = (dir==LEFT) ? RIGHT : LEFT;
                }

            }
            else spooktimer++;
        }
    }
    
    return true;
}

void CFoob::getTouchedBy(CSpriteObject &theObject)
{
	if(CPlayer *player = dynamic_cast<CPlayer*>(&theObject))
	{
		if(gBehaviorEngine.mDifficulty==HARD)
			player->kill();
	}
}
