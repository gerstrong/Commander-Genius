/*
 * CShikadiMine.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */

#include <base/video/CVideoDriver.h>


#include "sdl/audio/music/CMusic.h"
#include "CShikadiMine.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>
#include "../../common/dialog/CMessageBoxGalaxy.h"
#include "engine/core/mode/CGameMode.h"


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

// TODO: Overload the draw function, because the mine has an eye which moves around in the sprite

namespace galaxy {

enum MINEACTIONS
{
A_MINE_SIT = 0,
A_MINE_MOVE = 1,
A_MINE_CHANGE_DIR = 2,
A_MINE_DETONATE = 3
};


const int TIME_SIT = 150;
const int TIME_MOVE = 150;
const int TIME_CHANGE_DIR = 150;

const int MOVE_SPEED = 10;

CShikadiMine::CShikadiMine(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
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

    mEyeSprite = sprite + 1;

    setEyeCenterOffset(mTargetEyeXOffset, mTargetEyeYOffset);

    mEyeXOffset = mTargetEyeXOffset;
    mEyeYOffset = mTargetEyeYOffset;
}


void CShikadiMine::setEyeCenterOffset(int &x, int &y)
{
    GsSprite &eyeSprite = gGraphics.getSprite(mSprVar,mEyeSprite);
    GsSprite &spriteRef = gGraphics.getSprite(mSprVar,sprite);
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
		SD_PlaySound(SOUND_MINEEXPLODE);
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
  if(getProbability(500))
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
      GsSprite &spriteRef = gGraphics.getSprite(mSprVar,sprite);

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
      dead = true;
      exists = false;

      // Spawn little explosion shards here!
      const int newX = getXMidPos();
      const int newY = getYUpPos();

      spawnObj( new CMineShards( getMapPtr(),
							     0, newX, newY, -100 ) );
      spawnObj( new CMineShards( getMapPtr(),
							     0, newX, newY, -50 ) );
      spawnObj( new CMineShards( getMapPtr(),
							     0, newX, newY, 50 ) );
      spawnObj( new CMineShards( getMapPtr(),
							     0, newX, newY, 100 ) );

  }
}

bool CShikadiMine::isNearby(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			mKeenAlignmentX = LEFT;
		else
			mKeenAlignmentX = RIGHT;

		if( player->getYMidPos() < getYMidPos() )
			mKeenAlignmentY = UP;
		else
			mKeenAlignmentY = DOWN;
	}

	return true;
}

void CShikadiMine::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	if(getActionNumber(A_MINE_DETONATE))
		return;


	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
	    theObject.dead = true;
	    playSound(SOUND_MINEEXPLODE);
	    setAction(A_MINE_DETONATE);
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
	    playSound(SOUND_MINEEXPLODE);
	    setAction(A_MINE_DETONATE);
	    player->kill();
	}

}


void CShikadiMine::process()
{
	performCollisions();

	//if(!processActionRoutine())
	//    exists = false;

	(this->*mp_processState)();
}



void CShikadiMine::draw()
{
	if( sprite == BLANKSPRITE || dontdraw )
		return;

    GsSprite &Sprite = gGraphics.getSprite(mSprVar,sprite);

	scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
	scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;

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

CMineShards::CMineShards(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xSpeed) :
CStunnable(pmap, foeID, x, y),
mXSpeed(xSpeed)
{
  xDirection = (xSpeed < 0) ? LEFT : RIGHT;

  setupGalaxyObjectOnMap(0x2716, 0);

  yinertia = -100;
}


void CMineShards::getTouchedBy(CSpriteObject& theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
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

	// Special case when a shard touches the Omegamatic Core
	const int lx = getXMidPos();
	const int ly = getYDownPos();

	bool coreExplode = false;

	int dx=0, dy=0;

	if( mp_Map->getPlaneDataAt(2, lx, ly) == 0x29 )
	{
	  dx = lx; dy = ly;
	  coreExplode |= true;
	}
	if( mp_Map->getPlaneDataAt(2, lx+(1<<CSF), ly) == 0x29 )
	{
	  dx = lx+(1<<CSF); dy = ly;
	  coreExplode |= true;
	}
	if( mp_Map->getPlaneDataAt(2, lx, ly+(1<<CSF)) == 0x29 )
	{
	  dx = lx; dy = ly+(1<<CSF);
	  coreExplode |= true;
	}
	if( mp_Map->getPlaneDataAt(2, lx+(1<<CSF), ly+(1<<CSF)) == 0x29 )
	{
	  dx = lx+(1<<CSF); dy = ly+(1<<CSF);
	  coreExplode |= true;
	}


	if( coreExplode )
	{
	    // Quikly decorate the rotten QED!
	    dx >>= CSF; dy >>= CSF;
	    dx--;	dy--;

        for(int i=0 ; i<4 ; i++)
        {
            const int t1 = mp_Map->at(i,0);
            const int t2 = mp_Map->at(i,1);
            const int t3 = mp_Map->at(i+4,0);
            const int t4 = mp_Map->at(i+4,1);
            mp_Map->setTile(dx+i, dy,   t1, true, 1);
            mp_Map->setTile(dx+i, dy+1, t2, true, 1);
            mp_Map->setTile(dx+i, dy+2, t3, true, 1);
            mp_Map->setTile(dx+i, dy+3, t4, true, 1);
        }


		const std::string end_text("End of Episode.\n"
					       "The game will be restarted.\n"
					       "You can replay it again or\n"
					       "try another Episode for more fun!\n"
					       "The original epilog is under construction.");

        showMsg(end_text, new EventEndGamePlay());
		dead = true;
		exists = false;

		return;
	}

	if( blockedd )
	{
	  dead = true;
	  exists = false;
	  return;
	}


	moveXDir(mXSpeed);

	processActionRoutine();
}


}
