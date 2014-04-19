#include <base/video/CVideoDriver.h>
#include <base/utils/misc.h>

#include "CCeilick.h"
#include "../../common/ai/CPlayerLevel.h"

namespace galaxy
{
  
enum CEILICKACTIONS
{
A_CEILICK_SLEEP = 0,
A_CEILICK_LICK = 1,
A_CEILICK_STUNNED = 21
};


const int X_DIST_TO_LICK = 3<<CSF;
const int Y_DIST_TO_LICK = 8<<CSF;


CCeilick::CCeilick(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0),
mChanceToLick(false),
mCrawlPos(0),
mLaughed(false)
{
	mActionMap[A_CEILICK_SLEEP] = (GASOFctr) &CCeilick::processSleeping;
	mActionMap[A_CEILICK_LICK] = (GASOFctr) &CCeilick::processLicking;
	mActionMap[A_CEILICK_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;
	
	setupGalaxyObjectOnMap(0x31BE, A_CEILICK_SLEEP);
	
    xDirection = LEFT;
    solid = false;
}


void CCeilick::processSleeping()
{
  if(mChanceToLick)
  {
    setAction(A_CEILICK_LICK);
    playSound(SOUND_CEILICK_LICK);
    mChanceToLick = false;
    return;
  }
}


void CCeilick::processLicking()
{
  if( getActionStatus(A_CEILICK_LICK+10) )
  {
      // This will show the crawl effect!
      mCrawlPos = -16;
  }

  if( getActionStatus(A_CEILICK_LICK+15) )
  {
      // This will make him laugh!
      if(!mLaughed)
      {
	playSound(SOUND_CEILICK_LAUGH);
	mLaughed = true;
      }
  }  
  

  if( getActionStatus(A_CEILICK_LICK+19) )
  {
    setAction(A_CEILICK_SLEEP);
    mLaughed = false;
  }

}


bool CCeilick::isNearby(CSpriteObject& theObject)
{
	if( !getProbability(30) )
		return false;		

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos()-X_DIST_TO_LICK )
			return true;
		if( player->getXMidPos() > getXMidPos()+X_DIST_TO_LICK )
			return true;
		if( player->getYMidPos() < getYMidPos() )
			return true;
		if( player->getYMidPos() > getYMidPos()+Y_DIST_TO_LICK )
			return true;
				
		mChanceToLick = true;
		return true;
	}
	
	mChanceToLick = false;
	return true;
}


void CCeilick::getTouchedBy(CSpriteObject& theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		setAction(A_CEILICK_STUNNED);
        mCrawlPos = 0;
		dead = true;
		theObject.dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}

void CCeilick::draw()
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

        // Here we use the crawl effect to smoothen the movement of the ceilick.
        Uint16 showY = scry - 16 + mCrawlPos;

        if(mCrawlPos < 0)
            mCrawlPos++;

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
        }
        hasbeenonscreen = true;
    }
}



void CCeilick::process()
{
    performCollisions();

    if(!processActionRoutine())
    {
        setActionForce(A_CEILICK_SLEEP);
    }
	
	(this->*mp_processState)();
}


};
