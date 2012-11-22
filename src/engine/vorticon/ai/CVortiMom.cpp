#include "../../../sdl/sound/CSound.h"
#include "../../../sdl/CVideoDriver.h"
#include "../../../misc.h"

#include "CVortiMom.h"
#include "CFireBall.h"

CVortiMom::CVortiMom(CMap *p_map, Uint32 x, Uint32 y) :
CVorticonSpriteObject(p_map, x, y, OBJ_MOTHER)
{
	mHealthPoints = MOTHER_HP;
	state = MOTHER_WALK;
	animframe = 0;
	animtimer = 0;
	canbezapped = true;

	blockedr = blockedl = 0;

	if(g_pBehaviorEngine->mDifficulty>NORMAL)
		mHealthPoints++;
	else if(g_pBehaviorEngine->mDifficulty<NORMAL)
		mHealthPoints--;
}



bool CVortiMom::isNearby(CVorticonSpriteObject &theObject)
{
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
	if( state == MOTHER_HURT )
	{	 
	    
	    if (timer > MOTHER_HURT_SHOW_TIME)
	    {
		if (mHealthPoints <= 0)
		{
		    sprite = MOTHER_DEAD_FRAME;
		    dead = true;
		    timer = 0;
		    if (onscreen)
			playSound(SOUND_VORT_DIE);
		}
		else
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


void CVortiMom::getTouchedBy(CVorticonSpriteObject &theObject)
{
     if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
     {
	if(player->pdie)
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

		prob = (g_pBehaviorEngine->mDifficulty>NORMAL) ? MOTHER_SPIT_PROB_HARD : MOTHER_SPIT_PROB;
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
			sprite = MOTHER_WALK_RIGHT_FRAME + animframe;

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
			sprite = MOTHER_WALK_LEFT_FRAME + animframe;

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
		sprite = (dir==RIGHT) ?
				MOTHER_SPIT_RIGHT_FRAME:MOTHER_SPIT_LEFT_FRAME;


		if (timer > MOTHER_SPIT_SHOW_TIME)
		{
			CFireBall *newobject = new CFireBall(mp_Map, getXMidPos()-(3<<STC),
												getYPosition()+(11<<STC), dir,
												OBJ_MOTHER, m_index);
			g_pBehaviorEngine->EventList().add(new EventSpawnObject(newobject));

			if (onscreen)
				playSound(SOUND_TANK_FIRE);

			state = MOTHER_WALK;
		}
		else timer++;
		break;
	case MOTHER_HURT:
		sprite = MOTHER_HURT_FRAME;
		timer++;
		break;
	default: break;
	}
}

void CVortiMom::getShotByRay(object_t &obj_type)
{
	if( state != MOTHER_HURT && mHealthPoints>0 )
	{
		if(mHealthPoints>1 && g_pVideoDriver->getSpecialFXConfig())
			blink(10);
		sprite = MOTHER_HURT_FRAME;
		mHealthPoints--;
		state = MOTHER_HURT;

	}
}


