#include "CRope.h"

#include "../../common/ai/CPlayerBase.h"

#include <boost/property_tree/ptree.hpp>

/*
$1B9EW  #Keen throw rope
$1BBCW  #Keen throw rope
$1BDAW  #Keen climb rope
$1BF8W  #Keen climb rope
$1C16W  #Rope on map
$1C34W  #Rope being thrown 
$1C52W  #Invisible rope?
*/

namespace galaxy
{
    
enum ROPE_ACTION
{
A_ROPE_ACTIVE = 0,
A_ROPE_THROWN = 1
};

constexpr int THROW_TIME = 10;


CRope::CRope(CMap* pmap, Uint32 x, Uint32 y) :
CGalaxyActionSpriteObject(pmap, FOE_ID, x, y, 0),
mTimer(0)
{
    mActionMap[A_ROPE_ACTIVE] = (void (CGalaxyActionSpriteObject::*)()) &CRope::processActive;
    mActionMap[A_ROPE_THROWN] = (void (CGalaxyActionSpriteObject::*)()) &CRope::processThrown;

    setupGalaxyObjectOnMap(gBehaviorEngine.isDemo() ? 0x1A5C : 0x1C16, A_ROPE_THROWN);
    solid=false;
}


void
CRope::
serialize(boost::property_tree::ptree &node)
{
    auto &posNode = node.put("pos", "");
    posNode.put("<xmlattr>.x", m_Pos.x);
    posNode.put("<xmlattr>.y", m_Pos.y);
}


void
CRope::
deserialize(CSaveGameController &savedGame)
{
  savedGame.decodeData(m_Pos.x);
  savedGame.decodeData(m_Pos.y);
}

void
CRope::
serialize(CSaveGameController &savedGame)
{
  savedGame.encodeData(m_Pos.x);
  savedGame.encodeData(m_Pos.y);
}



void CRope::processActive()
{
    
}

void CRope::processThrown()
{
  mTimer++;
  
  if(mTimer < THROW_TIME)
    return;  
  
  mTimer = 0;
  
  setAction(A_ROPE_ACTIVE);
}

void CRope::getTouchedBy(CSpriteObject& theObject)
{
    if(mIsDead || theObject.mIsDead)
        return;
}


void CRope::process()
{
    (this->*mp_processState)();

    if(!processActionRoutine())
            exists = false;    
}


};
