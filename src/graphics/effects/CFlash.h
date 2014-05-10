/*
 * CFlash.h
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 *
 *  This class has been designed for the Flash
 *  Effect that is seen, when a vorticon is shot in EP1
 *  or a Vortelite in EP2
 */

#ifndef CFLASH_H_
#define CFLASH_H_

#include <memory>
#include <base/video/GsEffectController.h>
#include <graphics/GsSurface.h>

#include <base/GsTimer.h>

class CFlash : public CEffects
{
public:

	enum fade_styles{ FADE_NORMAL, FADE_PULSE };

	CFlash(Uint32 msecs, Uint8 speed=1,Uint32 color=0xFFFFFF );
	CFlash(Uint32 msecs, Uint8 speed=1,Uint32 color=0xFFFFFF, Uint8 m_maxalpha=128 );

    void ponder(const float deltaT);
    void render();

private:

	enum fade_dir{ FADE_IN, FADE_OUT };

	Uint32 m_StartTime;
	Uint32 m_RunTime;

	Uint8 m_Speed;
	Uint32 m_Color;
	Uint8 m_Alpha;
	fade_dir m_FadeDir;
	Uint8 m_Style;
	Uint8 m_MaxAlpha;   
    GsSurface mFadeSurface;
};

#endif /* CFLASH_H_ */
