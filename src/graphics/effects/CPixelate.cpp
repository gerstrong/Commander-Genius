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
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "sdl/CTimer.h"

#include "graphics/CGfxEngine.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>


CPixelate::CPixelate(unsigned short speed) :
mp_OldSurface(nullptr),
m_speed(speed)
{
	SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
	getSnapshot();

	m_line = 0;
	m_lines_completed = 0;
	m_drawmap = new bool[gameres.h*gameres.w];
	m_pixels_per_line = new unsigned short[gameres.h];

    for(int y=0 ; y<gameres.h ; y++)
	{
		m_pixels_per_line[y] = 0;
        for(int x=0 ; x<gameres.w ; x++)
			m_drawmap[y*gameres.w+x] = false;
	}
}

// get the Snapshot of the old surface, so the the effect can be applied on it!
void CPixelate::getSnapshot()
{
	g_pVideoDriver->collectSurfaces();
//#if SDL_VERSION_ATLEAST(2, 0, 0)

//#else
    if(!mp_OldSurface)
    {
        //mp_OldSurface = g_pVideoDriver->convertThroughBlitSfc(g_pVideoDriver->getBlitSurface());
        mp_OldSurface = g_pVideoDriver->convertThroughBlitSfc(g_pVideoDriver->getBlitSurface());
    }
//#endif

	//Map the color key
	mColorkey = SDL_MapRGB( mp_OldSurface->format, 0, 0xFF, 0xFF );
	//Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetColorKey( mp_OldSurface, SDL_TRUE, mColorkey );
#else
    SDL_SetColorKey( mp_OldSurface, SDL_SRCCOLORKEY, mColorkey );
#endif

}

// Effect cycle
void CPixelate::ponder()
{
	SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();

	if(m_line < gameres.h)
	{
		m_line+=m_speed;
		if(m_line > gameres.h) m_line=gameres.h;
	}

	if( m_lines_completed >= m_line )
	{
		// In this case the algorithm has finished
		mFinished = true;
	}
}

void CPixelate::render()
{
    SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();

    SDL_LockSurface(mp_OldSurface);

    // This is the algorithm for drawing the effect. It was invented by myself, so no guarantee,
    // it will look the same as in Keen Galaxy. We will see, when it's finished

    Uint8* pixels = static_cast<Uint8*> (mp_OldSurface->pixels);

    for(unsigned short y=m_lines_completed ; y<m_line ; y++)
    {
        for(unsigned short drawamt=0 ; drawamt < m_speed ; drawamt++ )
        {
            Uint16 x;

            do
            {	// get a random pixel between 0 and 320 which has not yet been occupied
                x = rand()%gameres.w;
            } while( m_drawmap[(Uint32)(y*gameres.w + x)] );

            // The y line gets one pixel painted at random x, between 0 and 320.
            //pixel = pixelstart + x*mp_OldSurface->format->BytesPerPixel;
            m_pixels_per_line[y]++;
            m_drawmap[y*gameres.w + x] = true;

            memcpy(pixels +
                   y*mp_OldSurface->pitch +
                   x*mp_OldSurface->format->BytesPerPixel,
                   &mColorkey,
                   mp_OldSurface->format->BytesPerPixel);

            // If there are no more pixels to draw in this line, m_lines_completed++, it won't be scanned again.
            // This will be checked against m_pixels_per_line
            if(m_pixels_per_line[y] >= gameres.w )
            {
                m_lines_completed++;
                break;
            }
        }
    }

    SDL_UnlockSurface(mp_OldSurface);

    SDL_BlitSurface( mp_OldSurface, NULL, g_pVideoDriver->getBlitSurface(), NULL );
}

CPixelate::~CPixelate()
{
	delete [] m_drawmap;
	delete [] m_pixels_per_line;
	if(mp_OldSurface) SDL_FreeSurface(mp_OldSurface);
}
