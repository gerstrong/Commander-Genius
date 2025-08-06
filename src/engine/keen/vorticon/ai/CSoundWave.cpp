#include "CSoundWave.h"

#include <graphics/GsGraphics.h>

// Sound Wave projectile, emitted by Meeps (ep3)

#define SNDWAVE_SPEED         62
#define SNDWAVE_SPEED_FAST    80

#define SNDWAVE_ANIM_RATE     20

#define SNDWAVE_LEFT_FRAME    128
#define SNDWAVE_RIGHT_FRAME   126

#define SNDWAVE_OFFSCREEN_KILL_TIME     100

CSoundWave::CSoundWave(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y,
		direction_t dir, object_t byType, size_t byID) :
CRay(p_map, x, y, dir, CENTER, 0,byType, byID),
animframe(0),
animtimer(0),
offscreentime(0)

{
	m_type = OBJ_SNDWAVE;
	setupObjectType(gBehaviorEngine.getEpisode());
    GsSprite &rSprite = gGraphics.getSprite(mSprVar,mSpriteIdx);
	m_BBox.x1 = rSprite.m_bboxX1;		m_BBox.x2 = rSprite.m_bboxX2;
	m_BBox.y1 = rSprite.m_bboxY1;		m_BBox.y2 = rSprite.m_bboxY2;

	performCollisions();
}

void CSoundWave::process()
{
	// destroy the sound wave if it's been offscreen for a good amount
	// of time. this is to prevent a massive buildup of soundwaves
	// slowly traveling through walls all the way across the level
	// (which can crash the game due to running out of object slots).
	if (!onscreen)
	{
		if (offscreentime > SNDWAVE_OFFSCREEN_KILL_TIME)
		{
			exists = false;
			return;
		}
		else offscreentime++;
	}
	else offscreentime = 0;

	// fly through the air
	int x = getXPosition()>>CSF;
	if (m_HorDir == RIGHT)
	{
		mSpriteIdx = SNDWAVE_RIGHT_FRAME + animframe;

		if (x > static_cast<int>(mpMap->m_width) )
			exists=false;
		else
		{
			if(gBehaviorEngine.mDifficulty>=NORMAL)
				moveRight(SNDWAVE_SPEED_FAST);
			else
				moveRight(SNDWAVE_SPEED);
		}
	}
	else
	{
		mSpriteIdx = SNDWAVE_LEFT_FRAME + animframe;

		if (x < 2)
			exists = false;
		else
		{
			if (gBehaviorEngine.mDifficulty>=NORMAL)
				moveLeft(SNDWAVE_SPEED_FAST);
			else
				moveLeft(SNDWAVE_SPEED);
		}
	}

	// animation
	if (animtimer > SNDWAVE_ANIM_RATE)
	{
		animframe ^= 1;
		animtimer = 0;
	}
	else animtimer++;
}

