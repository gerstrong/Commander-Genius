#include "CRope.h"

#include "../../common/ai/CPlayerBase.h"

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

const int THROW_TIME = 10;


CRope::CRope(CMap* pmap, Uint32 x, Uint32 y) :
CGalaxyActionSpriteObject(pmap, FOE_ID, x, y, 0),
mTimer(0)
{
    mActionMap[A_ROPE_ACTIVE] = (void (CGalaxyActionSpriteObject::*)()) &CRope::processActive;
    mActionMap[A_ROPE_THROWN] = (void (CGalaxyActionSpriteObject::*)()) &CRope::processThrown;

    setupGalaxyObjectOnMap(0x1C16, A_ROPE_THROWN);    
    solid=false;
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
    if(dead || theObject.dead)
        return;
}


void CRope::process()
{
    (this->*mp_processState)();

    if(!processActionRoutine())
            exists = false;    
}


};
