#include <base/audio/Audio.h>
#include <base/video/CVideoDriver.h>
#include <base/interface/misc.h>
#include <base/GsEventContainer.h>

#include "CVortiMom.h"
#include "CFireBall.h"

CVortiMom::CVortiMom(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y) :
CVorticonSpriteObject(p_map, x, y, OBJ_MOTHER)
{
	mHealthPoints = MOTHER_HP;
	state = MOTHER_WALK;
	animframe = 0;
	animtimer = 0;
	canbezapped = true;

	blockedr = blockedl = 0;

    const auto diff = gBehaviorEngine.mDifficulty;

    if(diff>NORMAL)
		mHealthPoints++;
    else if(diff<NORMAL)
		mHealthPoints--;

    if(diff == EXPERT)
    {
        mSprVar = 1;
        mHealthPoints *= 2;
    }
    else if(diff == NINJA)
    {
        mSprVar = 2;
        mHealthPoints *= 3;
    }
    else if(diff == ELITE)
    {
        mSprVar = 3;
        mHealthPoints *= 4;
    }

}



bool CVortiMom::isNearby(CSpriteObject &theObject)
{
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
	if( state == MOTHER_HURT )
	{	 	    
	    if (timer > MOTHER_HURT_SHOW_TIME)
	    {
		if(mHealthPoints > 0)
		{
		    state = MOTHER_WALK;
		    if (player->getXPosition() > getXPosition())
			dir = RIGHT;
		    else
			dir = LEFT;
		}
	    }
	    
	}
    }
    
    return true;
}


void CVortiMom::getTouchedBy(CSpriteObject &theObject)
{
     if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
     {
	if(!player->pdie)
	{
		// don't push the player as he's walking through the exit door
		if (!player->level_done)
		{
			player->push(*this);
		}
	}    
     }
}


void CVortiMom::process()
{
	int prob;

	switch(state)
	{
	case MOTHER_WALK:

		prob = (gBehaviorEngine.mDifficulty>NORMAL) ? MOTHER_SPIT_PROB_HARD : MOTHER_SPIT_PROB;
		if (getProbability(prob))
		{
			if (onscreen)
			{
				if(dir == RIGHT)
					dir = LEFT;
				else
					dir = RIGHT;

				state = MOTHER_SPIT;
				timer = 0;
			}
		}

		if (dir==RIGHT)
		{
			mSpriteIdx = MOTHER_WALK_RIGHT_FRAME + animframe;

			if (blockedr)
			{
				dir = LEFT;
			}
			else
			{
				xinertia = MOTHER_WALK_SPD;
			}
		}
		else
		{
			mSpriteIdx = MOTHER_WALK_LEFT_FRAME + animframe;

			if (blockedl)
				dir = RIGHT;
			else
				xinertia = -MOTHER_WALK_SPD;
		}

		// walk animation
		if (animtimer > MOTHER_WALK_ANIM_RATE)
		{
			animframe ^= 1;
			animtimer = 0;
		}
		else animtimer++;

		break;

	case MOTHER_SPIT:
		mSpriteIdx = (dir==RIGHT) ?
				MOTHER_SPIT_RIGHT_FRAME:MOTHER_SPIT_LEFT_FRAME;


		if (timer > MOTHER_SPIT_SHOW_TIME)
		{
			CFireBall *newobject = new CFireBall(mpMap, getXMidPos()-(3<<STC),
												getYPosition()+(11<<STC), dir,
												OBJ_MOTHER, m_index);
			gEventManager.add(new EventSpawnObject(newobject));

			if (onscreen)
                playSound(GameSound::TANK_FIRE);

			state = MOTHER_WALK;
		}
		else timer++;
		break;
	case MOTHER_HURT:
	    if (timer > MOTHER_HURT_SHOW_TIME)
	    {
		if (mHealthPoints <= 0)
		{
		    mSpriteIdx = MOTHER_DEAD_FRAME;
		    mIsDead = true;
		    timer = 0;

            playSound(GameSound::VORT_DIE);
		    
		    return;
		}
	    }	    	    	    
	    mSpriteIdx = MOTHER_HURT_FRAME;
	    timer++;
	    break;
	default: break;
	}
}

void CVortiMom::getShotByRay(object_t &obj_type)
{
	if( state != MOTHER_HURT && mHealthPoints>0 )
	{
        if(mHealthPoints>1 && gBehaviorEngine.mOptions[GameOption::SPECIALFX].value)
			blink(10);
		mSpriteIdx = MOTHER_HURT_FRAME;
		mHealthPoints--;
		state = MOTHER_HURT;

	}
}


