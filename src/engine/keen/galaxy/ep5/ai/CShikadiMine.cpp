/*
 * CShikadiMine.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */

#include <base/video/CVideoDriver.h>


#include <base/audio/music/CMusic.h>
#include "CShikadiMine.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/interface/misc.h>

#include "engine/core/spritedefines.h"


#include <graphics/GsGraphics.h>

/*
$2608W #Shikadi mine sit 0
$2626W #Shikadi mine move 1
$2644W #Shikadi mine change direction 2
$2662W #Shikadi mine detonate 3
$2680W #Shikadi mine detonate
$269EW #Shikadi mine detonate
$26BCW #Shikadi mine detonate
$26DAW #Shikadi mine detonate
$26F8W #Shikadi mine detonate
$2716W #Shikadi mine fragments
 */

namespace galaxy {

enum MINEACTIONS
{
A_MINE_SIT = 0,
A_MINE_MOVE = 1,
A_MINE_CHANGE_DIR = 2,
A_MINE_DETONATE = 3
};


//constexpr int TIME_SIT = 150;
constexpr int TIME_MOVE = 150;
//constexpr int TIME_CHANGE_DIR = 150;
constexpr int MOVE_SPEED = 10;

CShikadiMine::CShikadiMine(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y),
mTimer(0)
{
    mActionMap[A_MINE_SIT] = (GASOFctr) &CShikadiMine::processSit;
    mActionMap[A_MINE_MOVE] = (GASOFctr) &CShikadiMine::processMoving;
    mActionMap[A_MINE_CHANGE_DIR] = (GASOFctr) &CShikadiMine::processChangeDir;
    mActionMap[A_MINE_DETONATE] = (GASOFctr) &CShikadiMine::processDetonate;

    // Adapt this AI
    setupGalaxyObjectOnMap(0x2608, A_MINE_SIT);

    xDirection = CENTER;
    yDirection = CENTER;

    mEyeSprite = mSpriteIdx + 1;

    setEyeCenterOffset(mTargetEyeXOffset, mTargetEyeYOffset);

    mEyeXOffset = mTargetEyeXOffset;
    mEyeYOffset = mTargetEyeYOffset;
}


void CShikadiMine::setEyeCenterOffset(int &x, int &y)
{
    GsSprite &eyeSprite = gGraphics.getSprite(mSprVar,mEyeSprite);
    GsSprite &spriteRef = gGraphics.getSprite(mSprVar,mSpriteIdx);
    x = (spriteRef.getWidth()-eyeSprite.getWidth())/2;
    y = (spriteRef.getHeight()-eyeSprite.getHeight())/2;
}



void CShikadiMine::processSit()
{
  const int xDir = mTargetEyeXOffset-mEyeXOffset;
  const int yDir = mTargetEyeYOffset-mEyeYOffset;

  if(xDir < 0)
  {
    mEyeXOffset--;
    return;
  }
  else if(xDir>0)
  {
    mEyeXOffset++;
    return;
  }

  if(yDir < 0)
  {
    mEyeYOffset--;
    return;
  }
  else if(yDir>0)
  {
    mEyeYOffset++;
    return;
  }


  /*mTimer++;
  if( mTimer < TIME_SIT )
      return;

  mTimer = 0;*/


  /*
	delx = o->xpos - player->xpos;
	dely = o->ypos - player->ypos;

	if (delX < 0x200 && delX > -0x500 && delY < 0x300 && delY > -0x50) {
        SD_PlaySound(GameSound::MINEEXPLODE);
		o->action = ACTION_MINEEXPLODE0;
		RF_18(&o->int34);
		return;
	}
   */


  setAction(A_MINE_MOVE);
}


void CShikadiMine::processMoving()
{

  // Move normally in the direction
  moveXDir( xDirection*MOVE_SPEED );
  moveYDir( yDirection*MOVE_SPEED );

  mTimer++;
  if( mTimer < TIME_MOVE )
      return;

  mTimer = 0;

  setAction(A_MINE_CHANGE_DIR);
  setEyeCenterOffset(mTargetEyeXOffset, mTargetEyeYOffset);
}

void CShikadiMine::processChangeDir()
{
  const int xDir = mTargetEyeXOffset-mEyeXOffset;
  const int yDir = mTargetEyeYOffset-mEyeYOffset;

  if(xDir < 0)
  {
    mEyeXOffset--;
    return;
  }
  else if(xDir>0)
  {
    mEyeXOffset++;
    return;
  }

  if(yDir < 0)
  {
    mEyeYOffset--;
    return;
  }
  else if(yDir>0)
  {
    mEyeYOffset++;
    return;
  }

  bool updateDir = false;

  // Look at the Player coords and define a direction
  if(getProbability(600))
  {
    if(xDirection != mKeenAlignmentX)
    {
      xDirection = mKeenAlignmentX;
      yDirection = CENTER;
      updateDir |= true;
    }
  }
  else
  {
    if(yDirection != mKeenAlignmentY)
    {
      xDirection = CENTER;
      yDirection = mKeenAlignmentY;
      updateDir |= true;
    }

  }


    if( blockedl && xDirection == LEFT )
    {
	xDirection = CENTER;
	yDirection = mKeenAlignmentY;

	if(yDirection == UP && blockedu)
	    yDirection = DOWN;

	if(yDirection == DOWN && blockedd)
	{
	    yDirection = CENTER;
	    xDirection = RIGHT;
	}
    }
    else if( blockedr && xDirection == RIGHT )
    {
	xDirection = CENTER;
	yDirection = mKeenAlignmentY;

	if(yDirection == DOWN  && blockedd)
	    yDirection = UP;

	if(yDirection == UP && blockedu)
	{
	    yDirection = CENTER;
	    xDirection = LEFT;
	}
    }

    if( blockedu && yDirection == UP )
    {
	yDirection = CENTER;
	xDirection = mKeenAlignmentX;

	if(xDirection == LEFT && blockedl)
	    xDirection = RIGHT;

	if(xDirection == RIGHT && blockedr)
	{
	    xDirection = CENTER;
	    yDirection = DOWN;
	}
    }
    else if( blockedd && yDirection == DOWN )
    {
	yDirection = CENTER;
	xDirection = mKeenAlignmentX;

	if(xDirection == RIGHT && blockedr)
	    xDirection = LEFT;

	if(xDirection == LEFT && blockedl)
	{
	    xDirection = CENTER;
	    yDirection = UP;
	}
    }

    if(updateDir)
    {
        setEyeCenterOffset(mTargetEyeXOffset, mTargetEyeYOffset);

        GsSprite &eyeSprite = gGraphics.getSprite(mSprVar,mEyeSprite);
        GsSprite &spriteRef = gGraphics.getSprite(mSprVar,mSpriteIdx);

        if(xDirection == LEFT)
            mTargetEyeXOffset = 0;
        if(xDirection == RIGHT)
            mTargetEyeXOffset = spriteRef.getWidth()-eyeSprite.getWidth();

        if(yDirection == UP)
            mTargetEyeYOffset = 0;
        if(yDirection == DOWN)
            mTargetEyeYOffset = spriteRef.getHeight()-eyeSprite.getHeight();

        setAction(A_MINE_SIT);
    }
    else
    {
        setAction(A_MINE_MOVE);
    }
}

void CShikadiMine::processDetonate()
{
  if(!processActionRoutine())
  {
      mIsDead = true;
      exists = false;

      // Spawn little explosion shards here!
      const int newX = getXMidPos();
      const int newY = getYUpPos();


      spawnObj( new CMineShards( getMapPtr(),
                                 0, newX, newY, -90 ) );
      spawnObj( new CMineShards( getMapPtr(),
                                 0, newX, newY+(1<<CSF), -30 ) );
      spawnObj( new CMineShards( getMapPtr(),
                                 0, newX+(1<<CSF), newY, 30 ) );
      spawnObj( new CMineShards( getMapPtr(),
                                 0, newX+(1<<CSF), newY+(1<<CSF), 90 ) );

  }
}

static const int CSF_DISTANCE_TO_EXPLODE = 2<<CSF;

bool CShikadiMine::isNearby(CSpriteObject &theObject)
{
    if(getActionNumber(A_MINE_DETONATE))
    {
        return true;
    }

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
        const auto playerPos = player->getMidPos();
        const auto minePos = getMidPos();

        mKeenAlignmentX = playerPos.x < minePos.x ? LEFT : RIGHT;
        mKeenAlignmentY = playerPos.y < minePos.y ? UP : DOWN;

        // Chance to explode?
        if( playerPos.x < minePos.x - CSF_DISTANCE_TO_EXPLODE ||
            playerPos.x > minePos.x + CSF_DISTANCE_TO_EXPLODE )
            return false;

        if( playerPos.y < minePos.y - CSF_DISTANCE_TO_EXPLODE ||
            playerPos.y > minePos.y + CSF_DISTANCE_TO_EXPLODE )
            return false;

        // Player is near enough. Explode!
        playSound(GameSound::MINEEXPLODE);
        setAction(A_MINE_DETONATE);
	}

	return true;
}

void CShikadiMine::getTouchedBy(CSpriteObject &theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	if(getActionNumber(A_MINE_DETONATE))
		return;

	CStunnable::getTouchedBy(theObject);
}


void CShikadiMine::process()
{
	performCollisions();

	(this->*mp_processState)();
}



void CShikadiMine::draw()
{
	if( mSpriteIdx == BLANKSPRITE || dontdraw )
		return;

    GsSprite &Sprite = gGraphics.getSprite(mSprVar,mSpriteIdx);

    const auto scroll = mpMap->getMainScrollCoords();

    scrx = (m_Pos.x>>STC)-scroll.x;
    scry = (m_Pos.y>>STC)-scroll.y;

	SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();

    if( scrx < gameres.w && scry < gameres.h && exists )
	{
		Uint16 showX = scrx+Sprite.getXOffset();
		Uint16 showY = scry+Sprite.getYOffset();

		if(m_blinktime > 0)
		{
            Sprite.drawBlinkingSprite( showX, showY );
			m_blinktime--;
		}
		else
		{
            const int w = Sprite.getWidth();
            const int h = Sprite.getHeight();

            Sprite.drawSprite( showX, showY, w, h, (255-transluceny) );

		    if(!getActionNumber(A_MINE_DETONATE))
		    {
              GsSprite &eyeSprite = gGraphics.getSprite(mSprVar,mEyeSprite);

              const int eyeW = eyeSprite.getWidth();
              const int eyeH = eyeSprite.getHeight();

              eyeSprite.drawSprite( showX+mEyeXOffset, showY+mEyeYOffset, eyeW, eyeH );
		    }
		}
		hasbeenonscreen = true;
	}
}



///////////////////
/// Mine shards ///
///////////////////

CMineShards::CMineShards(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xSpeed) :
CStunnable(pMap, foeID, x, y),
mXSpeed(xSpeed)
{
  xDirection = (xSpeed < 0) ? LEFT : RIGHT;

  setupGalaxyObjectOnMap(0x2716, 0);

  yinertia = -100;
}


void CMineShards::getTouchedBy(CSpriteObject& theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	CStunnable::getTouchedBy(theObject);

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
        player->kill(false);
	}
}




void CMineShards::process()
{
	performCollisions();

	performGravityMid();

	// Reflections at wall. Not sure, if it makes sense, let's observe the behaviour
	if( (blockedl && mXSpeed < 0) ||
	    (blockedr && mXSpeed > 0) )
	{
	  mXSpeed = -mXSpeed;
	}

	if( blockedd )
	{
	  mIsDead = true;
	  exists = false;
	  return;
	}


	moveXDir(mXSpeed);

	processActionRoutine();
}


}
