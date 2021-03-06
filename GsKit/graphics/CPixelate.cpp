/*
 * CPixelate.cpp
 *
 *  Created on: 09.03.2010
 *      Author: gerstrong
 *
 *  Pixelate Effect. This effect appears, when in Keen 4
 *  you are beginning to see the main title screen.
 *  The pixelation you there, is that effect coded here!
 */

#include "CPixelate.h"
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>

#include "graphics/GsGraphics.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>


CPixelate::CPixelate(const float speed,
                     const bool modern) :
mSpeed(speed),
mModernMode(modern)
{
	SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
	getSnapshot();

	m_line = 0;
	m_lines_completed = 0;
    mDrawMap.assign(static_cast<unsigned int>(gameres.h*gameres.w),false);
    mPixelsPerLine.assign(static_cast<unsigned int>(gameres.h), 0);
}

// get the Snapshot of the old surface, so the the effect can be applied on it!
void CPixelate::getSnapshot()
{
	gVideoDriver.collectSurfaces();

    SDL_Surface *blitSfc = gVideoDriver.getBlitSurface();

    if(mOldSurface.empty())
    {
        mOldSurface.createRGBSurface(blitSfc->clip_rect);
    }

    if(mOldSurface.getSDLSurface()->format->Amask != 0)
    {
        mColorkey = mOldSurface.mapRGBA( 0, 0, 0, 0 );
    }
    else
    {
        mColorkey = mOldSurface.mapRGB( 0, 0xFF, 0xFF );
        mOldSurface.setColorKey(mColorkey);
    }

    GsWeakSurface mainBlit(blitSfc);

    mainBlit.blitTo(mOldSurface);
}

// Effect cycle
void CPixelate::ponder(const float deltaT)
{
	SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();

	if(m_line < gameres.h)
	{
        m_line += static_cast<unsigned short>(mSpeed*deltaT);
        if(m_line > gameres.h || mFinished)
            m_line = static_cast<unsigned short>(gameres.h);
	}

	if( m_lines_completed >= m_line )
	{
		// In this case the algorithm has finished
		mFinished = true;
	}
}

void CPixelate::renderModern()
{
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    GsWeakSurface mainBlit(gVideoDriver.getBlitSurface());

    mOldSurface.lock();

    // This is the algorithm for drawing the effect. It was invented by myself, so no guarantee,
    // it will look the same as in Keen Galaxy. We will see, when it's finished

    auto *oldfSfcSdl = mOldSurface.getSDLSurface();
    Uint8* pixels = static_cast<Uint8*> (oldfSfcSdl->pixels);

    for(unsigned short y=m_lines_completed ; y<m_line ; y++)
    {
        for(unsigned short drawamt=0 ; drawamt < mSpeed ; drawamt++ )
        {
            Uint16 x;

            do
            {	// get a random pixel between 0 and 320 which has not yet been occupied
                x = Uint16(rand()%gameres.w);
            } while( mDrawMap[Uint32(y*gameres.w + x)] );

            Uint32 colorVar;
            memcpy(&colorVar,
                   pixels + y*oldfSfcSdl->pitch +
                   x*oldfSfcSdl->format->BytesPerPixel,
                   oldfSfcSdl->format->BytesPerPixel);

            GsColor color = mOldSurface.getColorAlpha(colorVar);

            const int alphaSpeed = 160;
            if(color.a < alphaSpeed)
            {
                color.a = 0;
                mPixelsPerLine[y]++;
                mDrawMap[y*gameres.w + x] = true;
            }
            else
            {
                color.a -= alphaSpeed;
            }

            colorVar = mOldSurface.mapColorAlpha(color);

            memcpy(pixels + y*oldfSfcSdl->pitch +
                   x*oldfSfcSdl->format->BytesPerPixel,
                   &colorVar,
                   oldfSfcSdl->format->BytesPerPixel);


            // If there are no more pixels to draw in this line, m_lines_completed++, it won't be scanned again.
            // This will be checked against m_pixels_per_line
            if(mPixelsPerLine[y] >= gameres.w )
            {
                m_lines_completed++;
                break;
            }
        }
    }

    mOldSurface.unlock();

    mOldSurface.blitTo(mainBlit);
}

void CPixelate::renderRetro()
{
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    GsWeakSurface mainBlit(gVideoDriver.getBlitSurface());

    mOldSurface.lock();

    // This is the algorithm for drawing the effect. It was invented by myself, so no guarantee,
    // it will look the same as in Keen Galaxy. We will see, when it's finished

    auto *oldfSfcSdl = mOldSurface.getSDLSurface();
    Uint8* pixels = static_cast<Uint8*> (oldfSfcSdl->pixels);

    for(unsigned short y=m_lines_completed ; y<m_line ; y++)
    {
        for(unsigned short drawamt=0 ; drawamt < mSpeed ; drawamt++ )
        {
            Uint16 x;

            do
            {	// get a random pixel between 0 and 320 which has not yet been occupied
                x = Uint16(rand()%gameres.w);
            } while( mDrawMap[Uint32(y*gameres.w + x)] );

            // The y line gets one pixel painted at random x, between 0 and 320.
            mPixelsPerLine[y]++;
            mDrawMap[Uint32(y*gameres.w + x)] = true;

            memcpy(pixels +
                   y*oldfSfcSdl->pitch +
                   x*oldfSfcSdl->format->BytesPerPixel,
                   &mColorkey,
                   oldfSfcSdl->format->BytesPerPixel);

            // If there are no more pixels to draw in this line, m_lines_completed++, it won't be scanned again.
            // This will be checked against m_pixels_per_line
            if(mPixelsPerLine[y] >= gameres.w )
            {
                m_lines_completed++;
                break;
            }
        }
    }

    mOldSurface.unlock();

    mOldSurface.blitTo(mainBlit);
}

void CPixelate::render()
{
    if (mModernMode)
    {
        renderModern();
    }
    else
    {
        renderRetro();
    }
}
