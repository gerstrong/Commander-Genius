#include "CPlatformSit.h"

namespace galaxy {

CPlatformSit::CPlatformSit(std::shared_ptr<CMap> pMap, const Uint16 foeID,
                 const Uint32 x, const Uint32 y, const int actionOff, const int sprVar) :
CGalaxySpriteObject(pMap, foeID, x, y, sprVar),
CPlatform(pMap, foeID, x, y)
{
	m_ActionBaseOffset = actionOff;
	solid = false;
	setActionForce(0);
	setActionSprite();
	calcBoundingBoxes();
}

void CPlatformSit::process()
{    
	CPlatform::process();
}

}


