/*
 * CDimDark.cpp
 *
 *  Created on: 29.11.2012
 *      Author: gerstrong
 *  This darken the whole screen with alpha and lighten up the upcoming one.
 */

#ifndef CDIMDARK_H
#define CDIMDARK_H

#include <base/video/GsEffectController.h>
#include <graphics/GsSurface.h>

#include <memory>

class CDimDark : public CEffects
{

public:
    CDimDark(const Uint8 speed);    

    void getSnapshot();

    void ponder(const float deltaT);
    void render();
    
    GsSurface &getDarkSfc()
    {  return mDarkSurface;  }
    
    GsSurface &getSfc()
    {  return mOldSurface;  }

    
private:
	
    int m_Speed;
    int m_Alpha;
	bool dimDark;

    GsSurface mOldSurface;
    GsSurface mDarkSurface;

    Uint32 mColorkey;
};

#endif // CDIMDARK_H
