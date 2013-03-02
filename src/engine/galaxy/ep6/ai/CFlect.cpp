#include "CFlect.h"

#include "engine/galaxy/common/ai/CPlayerBase.h"

namespace galaxy
{
  
CFlect::CFlect(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{

}


void CFlect::processWalking()
{

}

void CFlect::getTouchedBy(CSpriteObject& theObject)
{

}


int CFlect::checkSolidD(int x1, int x2, int y2, const bool push_mode)
{
  turnAroundOnCliff( x1, x2, y2 );

  return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}

void CFlect::process()
{

}


};