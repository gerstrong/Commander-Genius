#include "CRope.h"

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
A_GIK_ACTIVE = 0,
A_GIK_THROWN = 1,
A_GIK_INVISIBLE = 2
};


CRope::CRope(CMap* pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxyActionSpriteObject(pmap, foeID, x, y)
{
    mActionMap[A_GIK_ACTIVE] = (void (CGalaxyActionSpriteObject::*)()) &CRope::processActive;
    mActionMap[A_GIK_THROWN] = (void (CGalaxyActionSpriteObject::*)()) &CRope::processThrown;
    mActionMap[A_GIK_INVISIBLE] = (void (CGalaxyActionSpriteObject::*)()) &CRope::processHidden;

    setupGalaxyObjectOnMap(0x1C16, A_GIK_ACTIVE);    
}


void CRope::processActive()
{
    
}

void CRope::processThrown()
{
    
}

void CRope::processHidden()
{
    
}



void CRope::getTouchedBy(CSpriteObject& theObject)
{

}


void CRope::process()
{
    (this->*mp_processState)();

    if(!processActionRoutine())
            exists = false;    
}


};
