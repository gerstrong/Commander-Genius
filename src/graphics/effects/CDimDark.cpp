/*
 * CDimDark.cpp
 *
 *  Created on: 29.11.2012
 *      Author: gerstrong
 */

#include "CDimDark.h"
#include <base/video/CVideoDriver.h>
#include <base/GsApp.h>

CDimDark::CDimDark(const Uint8 speed) :
m_Speed(speed),
m_Alpha(0),
dimDark(true)
{}


// get the Snapshot of the old surface, so the the effect can be applied on it!
void CDimDark::getSnapshot()
{
    gApp.render();

    GsWeakSurface blitSfc(gVideoDriver.getBlitSurface());

    auto blitFormat = blitSfc.getSDLSurface()->format;

    mOldSurface.create(SDL_SWSURFACE,
                       blitSfc.width(),
                       blitSfc.height(),
                       32,
                       blitFormat->Rmask,
                       blitFormat->Gmask,
                       blitFormat->Bmask,
                       0);

    // Surface might have alpha mask. In that case the mColorKey can be zero
    /*auto format = mOldSurface.getSDLSurface()->format;
    if(format->Amask != 0)
    {
        mColorkey = SDL_MapRGBA( format, 0, 0, 0, 0 );
    }
    else
    {
        //Map the color key
        //Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent

        mColorkey = SDL_MapRGB( format, 0, 0xFF, 0xFF );
    #if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetColorKey( mOldSurface.getSDLSurface(), SDL_TRUE, mColorkey );
    #else
        SDL_SetColorKey( mOldSurface.getSDLSurface(), SDL_SRCCOLORKEY, mColorkey );
    #endif
    }*/

    blitSfc.blitTo(mOldSurface);    

    //mOldSurface.fillRGB(0, 255, 0);

    mDarkSurface.create(SDL_SWSURFACE,
                        blitSfc.width(),
                        blitSfc.height(),
                        RES_BPP,
                        blitFormat->Rmask,
                        blitFormat->Gmask,
                        blitFormat->Bmask,
                        0);

    mDarkSurface.fillRGB(0,0,0);
}

void CDimDark::ponder(const float deltaT)
{
    // If there is no pointer set, do it now.
    if(!mOldSurface)
    {
        getSnapshot();
    }

    if(dimDark) // dim dark here
    {
        mDarkSurface.setAlpha(m_Alpha);

#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetSurfaceAlphaMod( mDarkSurface.getSDLSurface(), m_Alpha );
#else
        SDL_SetAlpha( mDarkSurface.getSDLSurface(), SDL_SRCALPHA, m_Alpha );
#endif

        const int sum = m_Alpha + m_Speed;

        if(sum > 255)
        {
            m_Alpha = 0;
            dimDark = false;
        }
        else
        {
            m_Alpha += m_Speed;
        }
    }
    else // Undim the upcoming surface.
    {
        // Process the effect
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetSurfaceAlphaMod( mDarkSurface.getSDLSurface(), 255-m_Alpha );
#else
        SDL_SetAlpha( mDarkSurface.getSDLSurface(), SDL_SRCALPHA, 255-m_Alpha );
#endif

        const int sum = m_Alpha + m_Speed;

        if(sum > 255)
        {
            m_Alpha = 255;
            mFinished = true;
        }
        else
        {
            m_Alpha += m_Speed;
        }
    }
}

void CDimDark::render()
{
    // If there is no pointer set, do it now.
    if(!mOldSurface)
    {
        getSnapshot();
    }

    GsWeakSurface blitSfc(gVideoDriver.getBlitSurface());


    if(dimDark) // dim dark here
    {
        // Process the effect
        mOldSurface.blitTo(blitSfc);
    }

    mDarkSurface.blitTo(blitSfc);
}

