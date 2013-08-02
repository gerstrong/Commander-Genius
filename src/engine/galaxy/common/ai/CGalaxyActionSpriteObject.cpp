#include "CGalaxyActionSpriteObject.h"

namespace galaxy
{

CGalaxyActionSpriteObject::CGalaxyActionSpriteObject(CMap* pmap, const Uint16 foeID, Uint32 x, Uint32 y, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
mp_processState(nullptr)
{

}

void CGalaxyActionSpriteObject::setActionForce(const size_t ActionNumber)
{    
	CGalaxySpriteObject::setActionForce(ActionNumber);

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
		mp_processState = mActionMap[ActionNumber];
	else
		CGalaxySpriteObject::setActionForce(0); // This might happen, when the action-map is incomplete
}
    
};
