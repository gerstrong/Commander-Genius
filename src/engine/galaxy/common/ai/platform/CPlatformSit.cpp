#include "CPlatformSit.h"

namespace galaxy {

CPlatformSit::CPlatformSit(CMap *pmap, const Uint16 foeID, 
                 const Uint32 x, const Uint32 y, const int actionOff, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
CPlatform(pmap, foeID, x, y)
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


