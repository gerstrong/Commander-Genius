/*
 * CFlash.h
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 *
 *  This class has been designed for the Flash
 *  Effect that is seen, when a vorticon is shot in EP1
 *  or a Vortelite in EP2 for example
 */

#ifndef CFLASH_H_
#define CFLASH_H_

#include <memory>
#include <base/video/GsEffectController.h>
#include <graphics/GsSurface.h>

#include <base/GsTimer.h>
#include <base/utils/Color.h>

class CFlash : public CEffects
{
public:

    enum fade_styles{ FADE_NORMAL, FADE_PULSE };

    CFlash(Uint32 msecs, Uint8 speed=1, const GsColor color=GsColor(0xFF, 0xFF, 0xFF) );
    CFlash(Uint32 msecs, Uint8 speed=1, const GsColor color=GsColor(0xFF, 0xFF, 0xFF), Uint8 m_maxalpha=128 );

    void ponder(const float deltaT);
    void render();

private:

    enum fade_dir{ FADE_IN, FADE_OUT };

    Uint32 m_StartTime;
    Uint32 m_RunTime;

    Uint8 m_Speed;
    GsColor m_Color;
    Uint8 m_Alpha = 0;
    fade_dir m_FadeDir = FADE_IN;
    Uint8 m_Style = FADE_PULSE;
    Uint8 m_MaxAlpha = 255;
    GsSurface mFadeSurface;
};

#endif /* CFLASH_H_ */
