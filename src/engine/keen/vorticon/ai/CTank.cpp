#include <base/GsEventContainer.h>
#include "engine/core/spritedefines.h"
#include <base/audio/Audio.h>
#include "CTank.h"
#include "CRay.h"

#include "fileio/KeenFiles.h"

CTank::CTank(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y, object_t objtype) :
CVorticonSpriteObject(p_map, x, y, objtype, 0)
{
	m_type = OBJ_TANK;
	hardmode = (gBehaviorEngine.mDifficulty>=NORMAL);
	state = TANK_WALK;
	movedir = RIGHT;
	animtimer = 0;
	frame = 0;
	timer = 0;
	ponsameleveltime = 0;
	alreadyfiredcauseonsamelevel = 0;
	dist_to_travel = TANK_MAXTRAVELDIST;
	canbezapped = true;  // will stop bullets but are not harmed
	mInvincible = true;
	
	// Read this from the Exe-File. Patchwork Mod 1 uses different one!
	shotHeight = 0; 
    gs_byte *ptr = gKeenFiles.exeFile.getRawData();
	ptr += 0x4900;
	memcpy(&shotHeight, ptr, 1 );	
	shotHeight <<= STC;
}


bool CTank::isNearby(CSpriteObject &theObject)
{
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
	if( state == TANK_LOOK )
	{	    
	    // when time is up go back to moving
	    if (timer > TANK_LOOK_TOTALTIME)
	    {
		// decide what direction to go
		
		if(player->getXMidPos() < getXMidPos())
		{
		    movedir = LEFT;
		    mSpriteIdx = TANK_WALK_LEFT_FRAME;
		}
		else if(player->getXMidPos() > getXMidPos())
		{
		    movedir = RIGHT;
		    mSpriteIdx = TANK_WALK_RIGHT_FRAME;
		}
		state = TANK_WALK;
		animtimer = 0;
		timer = 0;
	    } 
	}
    }
    
    return true;
}

void CTank::process()
{
	switch(state)
	{
	case TANK_WALK: // Walk in a direction
	{
		// is keen on same level?
		if (movedir==LEFT)
		{  // move left
			xDirection = LEFT;
			mSpriteIdx = TANK_WALK_LEFT_FRAME + frame;
			xinertia = -TANK_WALK_SPEED;
			if( blockedl )
			{
				movedir = RIGHT;
				frame = 0;
				timer = 0;
				animtimer = 0;
				state = TANK_TURN;
			}

			dist_to_travel--;
		}
		else
		{  // move right
			xDirection = RIGHT;
			mSpriteIdx = TANK_WALK_RIGHT_FRAME + frame;
			xinertia = TANK_WALK_SPEED;
			if ( blockedr )
			{
				movedir = LEFT;
				frame = 0;
				timer = 0;
				animtimer = 0;
				state = TANK_TURN;
			}

			dist_to_travel--;
		}

		// walk animation
		if (animtimer > TANK_WALK_ANIM_TIME)
		{
			if (frame>=3) frame=0;
			else frame++;
			animtimer = 0;
		} else animtimer++;

		if(dist_to_travel==0)
		{
			frame = 0;
			timer = 0;
			animtimer = 0;
			state = TANK_WAIT;
		}
	default: break;
	}
	break;

	case TANK_WAIT:
		if ( (timer > TANK_PREPAREFIRE_TIME) ||
				(timer > TANK_PREPAREFIRE_TIME_FAST && hardmode) )
		{
			timer = 0;
			state = TANK_FIRE;
		}
		else
			timer++;

		break;

	case TANK_TURN:
		// If it gets stuck somewhere turn around
		mSpriteIdx = TANK_LOOK_FRAME + frame;
		// animation
		if (animtimer > TANK_LOOK_ANIM_TIME)
		{
			frame ^= 1;
			animtimer = 0;
		} else animtimer++;

		// when time is up go back to moving
		if (timer > TANK_LOOK_TOTALTIME)
		{
			// decide what direction to go
			state = TANK_WALK;
			animtimer = 0;
			timer = 0;
		} else timer++;
		break;
	case TANK_FIRE:
	{
		int height_top = shotHeight;
		if(height_top!=0)
		{
			CRay *newobject;
			if (onscreen)
                playSound(GameSound::TANK_FIRE);
			if (movedir==RIGHT)
                newobject = new CRay(mpMap, getXMidPos(), getYUpPos()+height_top, RIGHT, CENTER, getSpriteVariantIdx());
			else
                newobject = new CRay(mpMap, getXMidPos()-(1<<CSF), getYUpPos()+height_top, LEFT, CENTER, getSpriteVariantIdx());
			newobject->setOwner(OBJ_TANK, m_index);
			newobject->setSpeed(108);
			newobject->mSpriteIdx = ENEMYRAY;
			newobject->canbezapped = true;
			gEventManager.add(new EventSpawnObject(newobject));
		}

		state = TANK_WAIT_LOOK;
		frame = 0;
		timer = 0;
		animtimer = 0;
		dist_to_travel = TANK_MINTRAVELDIST + (rnd()%10)*(TANK_MAXTRAVELDIST-TANK_MINTRAVELDIST)/10;
	}
	break;

	case TANK_WAIT_LOOK:
		// Happens after Robot has fired
		if ( timer > TANK_WAITAFTER_FIRE )
		{
			timer = 0;
			state = TANK_LOOK;
		}
		else
			timer++;

		break;

	case TANK_LOOK:
		mSpriteIdx = TANK_LOOK_FRAME + frame;
		// animation
		if (animtimer > TANK_LOOK_ANIM_TIME)
		{
			frame ^= 1;
			animtimer = 0;
		} else animtimer++;

		timer++;
		break;
	}
}

void CTank::getTouchedBy(CSpriteObject &theObject)
{   // push keen
	if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
	{
	    player->bump( movedir );
	}
}

int CTank::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CSpriteObject::checkSolidD(x1, x2, y2, push_mode);
}

