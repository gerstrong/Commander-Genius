#include <base/audio/Audio.h>
#include <base/interface/misc.h>
#include "CMeep.h"
#include "CSoundWave.h"

enum meep_actions{
	MEEP_WALK, MEEP_SING,
	MEEP_DYING
};

#define MEEP_WALK_ANIM_RATE     3
#define MEEP_WALK_SPD           20

#define MEEP_SING_PROB          5
#define MEEP_SING_SHOW_TIME     10

#define MEEP_DYING_SHOW_TIME    6

#define MEEP_WALK_RIGHT_FRAME   118
#define MEEP_WALK_LEFT_FRAME    120
#define MEEP_SING_RIGHT_FRAME   122
#define MEEP_SING_LEFT_FRAME    123
#define MEEP_DYING_FRAME        124
#define MEEP_DEAD_FRAME         125

CMeep::CMeep(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y) :
CVorticonSpriteObject(p_map,x,y, OBJ_MEEP)
{
	canbezapped = true;

	state = MEEP_WALK;

	blockedr = blockedl = false;
	animframe = 0;
	animtimer = 0;

    const auto diff = gBehaviorEngine.mDifficulty;

    if(diff==HARD)
		mHealthPoints++;

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


bool CMeep::isNearby(CSpriteObject &theObject)
{
    if(CPlayer *player = dynamic_cast<CPlayer*>(&theObject))
    {
	if( state== MEEP_WALK )
	{
	    if (getProbability(MEEP_SING_PROB))
	    {
		if (onscreen)
		{
		    state = MEEP_SING;
		    timer = 0;
		}
		else
		{
		    // try to get onscreen by heading towards the player
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


void CMeep::getTouchedBy(CSpriteObject &theObject)
{
    if(CPlayer *player = dynamic_cast<CPlayer*>(&theObject))
    {
	if (!player->pdie && !player->level_done)
	{
	    // don't push the player as he's walking through the exit door
	    player->push(*this);
	}	
    }    
}


void CMeep::process()
{
	int not_about_to_fall;

	if (mHealthPoints <= 0 && state != MEEP_DYING )
	{
		timer = 0;
		state = MEEP_DYING;
		dying = true;
        gAudio.playStereofromCoord(int(GameSound::SHOT_HIT), SoundPlayMode::PLAY_NOW, scrx);
	}

	std::vector<CTileProperties> &TileProperties = gBehaviorEngine.getTileProperties();

	switch(state)
	{
	case MEEP_WALK:

		if (dir==RIGHT)
		{
			mSpriteIdx = MEEP_WALK_RIGHT_FRAME + animframe;

			not_about_to_fall = TileProperties.at(mpMap->at((getXLeftPos())>>CSF, (getYDownPos()+(1<<STC))>>CSF)).bup;

			if (blockedr || !not_about_to_fall)
				dir = LEFT;
			else
				xinertia = MEEP_WALK_SPD;
		}
		else
		{
			mSpriteIdx = MEEP_WALK_LEFT_FRAME + animframe;
			not_about_to_fall = TileProperties.at(mpMap->at((getXRightPos())>>CSF, (getYDownPos()+(1<<STC))>>CSF)).bup;

			if (blockedl || !not_about_to_fall)
				dir = RIGHT;
			else
				xinertia = -MEEP_WALK_SPD;
		}

		// walk animation
		if (animtimer > MEEP_WALK_ANIM_RATE)
		{
			animframe ^= 1;
			animtimer = 0;
		}
		else animtimer++;

		break;
	case MEEP_SING:
		if (dir==RIGHT)
		{
			mSpriteIdx = MEEP_SING_RIGHT_FRAME;
		}
		else
		{
			mSpriteIdx = MEEP_SING_LEFT_FRAME;
		}

		if (timer > MEEP_SING_SHOW_TIME)
		{
			CSoundWave *newobject;
			if (dir==RIGHT)
				newobject = new CSoundWave(mpMap, getXRightPos(), getYPosition()+(5<<STC), RIGHT);
			else
				newobject = new CSoundWave(mpMap, getXLeftPos(), getYPosition()+(5<<STC), LEFT);
			newobject->setOwner(OBJ_MEEP, m_index);
			newobject->solid = false;
			spawnObj(newobject);
            playSound(GameSound::MEEP);
			state = MEEP_WALK;
		}
		else timer++;
		break;
	case MEEP_DYING:
		mSpriteIdx = MEEP_DYING_FRAME;
		if (timer > MEEP_DYING_SHOW_TIME)
		{
			mSpriteIdx = MEEP_DEAD_FRAME;
			mIsDead = true;
			timer = 0;
		}
		else timer++;
		break;
	}
}
