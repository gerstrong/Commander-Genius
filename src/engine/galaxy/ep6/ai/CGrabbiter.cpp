#include "CGrabbiter.h"

namespace galaxy
{
    

enum GRABBITERACTIONS
{
    A_GRABBITER_HUNGRY = 0,    
    A_GRABBITER_NAPPING = 2,
};


CGrabbiter::CGrabbiter(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) : 
CStunnable(pmap, foeID, x, y)
{
	mActionMap[A_GRABBITER_HUNGRY] = (void (CStunnable::*)()) &CGrabbiter::processHungry;
	mActionMap[A_GRABBITER_NAPPING] = (void (CStunnable::*)()) &CGrabbiter::processNapping;
	
	setupGalaxyObjectOnMap(0x1A90, A_GRABBITER_HUNGRY);
}


void CGrabbiter::processHungry()
{

}


void CGrabbiter::processNapping()
{

}


void CGrabbiter::getTouchedBy(CSpriteObject& theObject)
{
    
}


void CGrabbiter::process()
{
	performCollisions();
	
	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();    
}

}