#include <base/video/CVideoDriver.h>
#include <base/interface/misc.h>

#include "CCeilick.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include "engine/core/spritedefines.h"

#include <engine/core/CBehaviorEngine.h>
#include <graphics/GsGraphics.h>

namespace galaxy
{
  
enum CEILICKACTIONS
{
A_CEILICK_SLEEP = 0,
A_CEILICK_LICK = 1,
A_CEILICK_STUNNED = 21
};


constexpr int X_DIST_TO_LICK = 2<<CSF;
constexpr int Y_DIST_TO_LICK = 8<<CSF;


CCeilick::CCeilick(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y),
mChanceToLick(false),
mCrawlPos(0),
mLaughed(false)
{
	mActionMap[A_CEILICK_SLEEP] = (GASOFctr) &CCeilick::processSleeping;
	mActionMap[A_CEILICK_LICK] = (GASOFctr) &CCeilick::processLicking;
	mActionMap[A_CEILICK_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;
	
	setupGalaxyObjectOnMap(gBehaviorEngine.isDemo() ? 0x23FE : 0x31BE, A_CEILICK_SLEEP);
	
    xDirection = LEFT;
    solid = false;
    mDispStarsBelow = true;
}


void CCeilick::processSleeping()
{
  if(mChanceToLick)
  {
    setAction(A_CEILICK_LICK);
    playSound(GameSound::CEILICK_LICK);
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
            playSound(GameSound::CEILICK_LAUGH);
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
    /*
    if( !getProbability(30) )
        return false;	*/

    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
        mChanceToLick = false;

		if( player->getXMidPos() < getXMidPos()-X_DIST_TO_LICK )
            return false;
		if( player->getXMidPos() > getXMidPos()+X_DIST_TO_LICK )
            return false;
		if( player->getYMidPos() < getYMidPos() )
            return false;
		if( player->getYMidPos() > getYMidPos()+Y_DIST_TO_LICK )
            return false;
				
		mChanceToLick = true;
		return true;
	}

    return false;
}


void CCeilick::getTouchedBy(CSpriteObject& theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
    if( dynamic_cast<CBullet*>(&theObject) )
	{
		setAction(A_CEILICK_STUNNED);
        mCrawlPos = 0;
		mIsDead = true;
		theObject.mIsDead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
        player->kill(false);
	}
}

void CCeilick::draw()
{
    if( mSpriteIdx == BLANKSPRITE || dontdraw )
        return;

    if(mIsDead)
        drawStars();

    GsSprite &rSprite = gGraphics.getSprite(mSprVar,mSpriteIdx);

    const auto scroll = mpMap->getMainScrollCoords();

    scrx = (m_Pos.x>>STC)-scroll.x;
    scry = (m_Pos.y>>STC)-scroll.y;

    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();

    if( scrx < gameres.w && scry < gameres.h && exists )
    {
        int showX = scrx+rSprite.getXOffset();

        // Here we use the crawl effect to smoothen its motion.
        int showY = scry + mCrawlPos;

        const int w = rSprite.getWidth();
        const int h = rSprite.getHeight();

        /*
        {
            m_BBox.x1 = rSprite.m_bboxX1;
            m_BBox.x2 = rSprite.m_bboxX2;
            m_BBox.y1 = 0;
            m_BBox.y2 = (h+mCrawlPos);
        }
        */

        if(mCrawlPos < 0)
            mCrawlPos++;

        if(m_blinktime > 0)
        {
            rSprite.drawBlinkingSprite( showX, showY );
            m_blinktime--;
        }
        else
        {
            rSprite.drawSprite( showX, showY, w, h, (255-transluceny) );
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
