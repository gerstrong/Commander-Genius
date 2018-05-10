#include "Autogun.h"
#include "CPlayerBase.h"

#include <boost/property_tree/ptree.hpp>

namespace galaxy
{  


AutoShot::AutoShot(CMap *pmap, const Uint32 foeID, const Uint32 x, const Uint32 y,
         direction_t horDir, direction_t vertDir, int basesprite, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar)
{
    // Coding for autogun. It covers Keen 4 Darts in Pyramids and the auto shooting guns in Keen 5

    const int ep = gBehaviorEngine.getEpisode();

    xDirection = horDir;
    yDirection = vertDir;

    mBaseSprite = basesprite;

    if(ep == 4)
    {
        if(xDirection == LEFT && yDirection == CENTER)
        {
            m_Pos.x += (7<<STC);
            m_Pos.y += (7<<STC);
        }
        else if(xDirection == CENTER && yDirection == UP)
        {
            m_Pos.y -= (7<<STC);
            m_Pos.x += (7<<STC);
        }
        else if(xDirection == RIGHT && yDirection == CENTER)
        {
            m_Pos.x -= (7<<STC);
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
    mSpriteIdx = mBaseSprite;

    origin = getPosition();

    performCollisions();
}

void AutoShot::deserialize(boost::property_tree::ptree &node)
{
    mTimer = node.get("<xmlattr>.timer", mTimer);
    auto &posNode = node.put("pos", "");
    m_Pos.x = posNode.get<Uint32>("<xmlattr>.x", m_Pos.x);
    m_Pos.y = posNode.get<Uint32>("<xmlattr>.y", m_Pos.y);
    auto &originNode = node.put("origin", "");
    origin.x = originNode.get<Uint32>("<xmlattr>.x", origin.x);
    origin.y = originNode.get<Uint32>("<xmlattr>.y", origin.y);
}

void AutoShot::serialize(boost::property_tree::ptree &node)
{
    node.put("<xmlattr>.timer", mTimer);
    auto &posNode = node.put("pos", "");
    posNode.put("<xmlattr>.x", m_Pos.x);
    posNode.put("<xmlattr>.y", m_Pos.y);
    auto &originNode = node.put("origin", "");
    originNode.put("<xmlattr>.x", origin.x);
    originNode.put("<xmlattr>.y", origin.y);
}

void AutoShot::waiting()
{
    const int ep = gBehaviorEngine.getEpisode();
    if(ep > 4)
    {
        if(mSpriteIdx < mBaseSprite + mNumAnimSprites + 1)
            mSpriteIdx++;
        else
        {
            mSpriteIdx = mBaseSprite;
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
    mSpriteIdx = mBaseSprite;
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
        Player->kill(false);
    }
}



void AutoShot::setWaitStatus()
{        
    // wait! in keen 4 it has to return
    processState = &AutoShot::waiting;
    if(gBehaviorEngine.getEpisode() > 4)
    {
        mSpriteIdx = mBaseSprite + mNumAnimSprites;
    }
    
    dontdraw = true;
}



// When autogun is waiting to shoot!
void AutoShot::flying()
{    
  moveXDir(xDirection*FLY_SPEED);
  moveYDir(yDirection*FLY_SPEED);
    
  mSpriteIdx = mBaseSprite + (mTimer % mNumAnimSprites);
  
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
