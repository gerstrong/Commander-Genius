#include "Autogun.h"
#include "CPlayerBase.h"

namespace galaxy
{  

// TODO: Autoguns should only shoot when they are visible.
// In that matter they get different patterns that might help getting through the game

AutoShot::AutoShot(CMap *pmap, const Uint32 foeID, const Uint32 x, const Uint32 y,
         direction_t horDir, direction_t vertDir, int basesprite, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
mTimer(0)
{
    // Coding for autogun. It covers Keen 4 Darts in Pyramids and the auto shooting guns in Keen 5

    const int ep = g_pBehaviorEngine->getEpisode();

    xDirection = horDir;
    yDirection = vertDir;

    mBaseSprite = basesprite;

    if(ep == 4)
    {
        if(xDirection == LEFT && yDirection == CENTER)
        {
            m_Pos.x += (1<<CSF);
            m_Pos.y += (7<<STC);
        }
        else if(xDirection == CENTER && yDirection == UP)
        {
            m_Pos.y -= (1<<CSF);
            m_Pos.x += (7<<STC);
        }
        else if(xDirection == RIGHT && yDirection == CENTER)
        {
            m_Pos.x -= (1<<CSF);
            m_Pos.y += (7<<STC);
        }
        else if(xDirection == CENTER && yDirection == DOWN)
        {
            m_Pos.x += (7<<STC);
        }
    }

    if(ep > 4)
    {
        mNumAnimSprites = 4;
    }
    else
    {
        mNumAnimSprites = 2;
        dontdraw = true;
    }

    processState = &AutoShot::waiting;
    sprite = mBaseSprite;

    origin = getPosition();

    performCollisions();
}

void AutoShot::waiting()
{
    const int ep = g_pBehaviorEngine->getEpisode();
    if(ep > 4)
    {
        if(sprite < mBaseSprite + mNumAnimSprites + 1)
            sprite++;
        else
        {
            sprite = mBaseSprite;
            dontdraw = true;
        }
    }
    
    if( mTimer < WAIT_TIME )
    {
        mTimer++;
        return;
    }

  if(m_Pos != origin)
  {
    moveToForce(origin);  
    onslope = ( blockedd = ( blockedl = ( blockedr = (blockedu = false) ) ) );
  }
  
  dontdraw = false;
  sprite = mBaseSprite;
  mTimer = 0;
  processState = &AutoShot::flying;
  
  if(ep == 4)
  {
      playSound(SOUND_DARTGUN_SHOOT);
  }
  else if(ep == 5)
  {
      playSound(SOUND_ROBORED_SHOOT); // Yeah, it is the same sound!
  }
  else // keen 6
  {
      playSound(SOUND_AUTOGUN);
  }  
  
  dontdraw = false;
}


void AutoShot::getTouchedBy(CSpriteObject &theObject)
{
  if(processState != &AutoShot::flying)
    return;
    
  if(CPlayerBase *Player = dynamic_cast<CPlayerBase*>(&theObject))
  {
    Player->kill();
  }
}



void AutoShot::setWaitStatus()
{        
    // wait! in keen 4 it has to return        
    processState = &AutoShot::waiting;
    if(g_pBehaviorEngine->getEpisode() > 4)
    {
	sprite = mBaseSprite + mNumAnimSprites;
    }
    
    dontdraw = true;
}



// When autogun is waiting to shoot!
void AutoShot::flying()
{    
  moveXDir(xDirection*FLY_SPEED);
  moveYDir(yDirection*FLY_SPEED);
    
  sprite = mBaseSprite + (mTimer % mNumAnimSprites);
  
  if(yDirection == DOWN && blockedd)
    setWaitStatus();
  else if(yDirection == UP && blockedu)
    setWaitStatus();

  if(xDirection == LEFT && blockedl)
    setWaitStatus();
  else if(xDirection == RIGHT && blockedr)
    setWaitStatus();  
  
  if(onslope)
      setWaitStatus();
}

// When autoguns bullet is flying over the screen!
void AutoShot::process()
{
    mTimer++;
    
    (this->*processState)();
}

};
