#include "CCeilick.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include <sdl/CVideoDriver.h>
#include <misc.h>

/*
 
$31BEW  #Ceilick 'asleep' 0
$31DCW  #Ceilicking 1
$31FAW  #Ceilicking 2
$3218W  #Ceilicking 3
$3236W  #Ceilicking 4
$3254W  #Ceilicking 5
$3272W  #Ceilicking 6
$3290W  #Ceilicking 7 
$32AEW  #Ceilicking 8
$32CCW  #Ceilicking 9
$32EAW  #Ceilicking 10
$3308W  #Ceilicking 11
$3326W  #Ceilicking 12 
$3344W  #Ceilicking 13 
$3362W  #Ceilicking 14
$3380W  #Ceilicking 15
$339EW  #Ceilicking 16
$33BCW  #Ceilicking 17
$33DAW  #Ceilicking 18
$33F8W  #Ceilicking 19
$3416W  #Ceilicking 20
$3434W  #Ceilick stunned 21
 
 */

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
mCrawlPos(0)
{
	mActionMap[A_CEILICK_SLEEP] = (void (CStunnable::*)()) &CCeilick::processSleeping;
	mActionMap[A_CEILICK_LICK] = (void (CStunnable::*)()) &CCeilick::processLicking;
	mActionMap[A_CEILICK_STUNNED] = &CStunnable::processGettingStunned;
	
	setupGalaxyObjectOnMap(0x31BE, A_CEILICK_SLEEP);
	
    xDirection = LEFT;
    solid = false;
}


void CCeilick::processSleeping()
{
  if(mChanceToLick)
  {
    setAction(A_CEILICK_LICK);
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

  if( getActionStatus(A_CEILICK_LICK+19) )
  {
    setAction(A_CEILICK_SLEEP);
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

    CSprite &Sprite = g_pGfxEngine->getSprite(sprite);

    scrx = (m_Pos.x>>STC)-mp_Map->m_scrollx;
    scry = (m_Pos.y>>STC)-mp_Map->m_scrolly;

    SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();

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
            Sprite.drawSprite( showX, showY, (255-transluceny) );
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
