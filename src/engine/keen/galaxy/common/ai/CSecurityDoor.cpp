#include "CSecurityDoor.h"

constexpr int TIME_FOR_NEXT_FRAME = 25;

CSecurityDoor::CSecurityDoor(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pMap, foeID, x, y, 0),
mTimer(0),
mFrameCount(0)
{
    // No real sprite object but still interacts
    dontdraw = true;
}


void CSecurityDoor::process()
{
    mTimer--;
    
    if(mTimer > 0)
	return;
        
    mTimer = TIME_FOR_NEXT_FRAME;
    
    // Now move the frames
    const int startX = getXPosition()>>CSF;
    const int startY = getYPosition()>>CSF;
    //const int endX = startX+4;
    //const int endY = startY+4;
    const int endX = startX+3;
    const int endY = startY+4;

    for(int y = startY ; y<endY ; y++)
    {
        for(int x = startX ; x<endX ; x++)
        {
            const int newTile = mpMap->getPlaneDataAt(1, x<<CSF, y<<CSF);
            mpMap->setTile(x, y, newTile-4, true, 1);
        }
    }
    
    mFrameCount++;
    
    if(mFrameCount >= 3)
    {
        exists = false;
        mIsDead = true;
    }

    
}

