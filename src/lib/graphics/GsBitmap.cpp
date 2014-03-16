/*
 * GsBitmap.cpp
 *
 *  Created on: 03.09.2009
 *      Author: gerstrong
 */

#include "GsBitmap.h"
#include "GsPalette.h"
#include <base/FindFile.h>
#include <base/video/CVideoDriver.h>

#include <lib/base/GsLogging.h>

GsBitmap::GsBitmap()
{}


GsBitmap::GsBitmap(const std::shared_ptr<SDL_Surface> &bmpSfc)
{
	mpBitmapSurface = bmpSfc;
}

GsBitmap::GsBitmap(const GsBitmap &bitmap) :
mName(bitmap.getName())
{
	SDL_Surface *sfc = bitmap.getSDLSurface();

    if( sfc != nullptr )
    {
        // Create a copy of the surface for that bitmap
        SDL_Surface *copySfc = SDL_ConvertSurface(sfc, sfc->format, 0 );

        mpBitmapSurface.reset( copySfc, &SDL_FreeSurface );
    }
}

///
// Creation Routines
///
bool GsBitmap::createSurface(Uint32 flags, SDL_Rect rect, SDL_Color *Palette)
{        
    mpBitmapSurface.reset( SDL_CreateRGBSurface(flags, rect.w, rect.h, 8, 0, 0, 0, 0 ), &SDL_FreeSurface );

    auto bmpSfc = mpBitmapSurface.get();

#if SDL_VERSION_ATLEAST(2, 0, 0)   
    SDL_SetPaletteColors(bmpSfc->format->palette, Palette, 0, 255);
    SDL_SetColorKey(bmpSfc, SDL_TRUE, COLORKEY);
#else
    SDL_SetColors(bmpSfc, Palette, 0, 255);
    SDL_SetColorKey(bmpSfc, SDL_SRCCOLORKEY, COLORKEY);
#endif


    if(mpBitmapSurface)
	  return true;
	else
	  return false;
}

bool GsBitmap::optimizeSurface()
{
	if(mpBitmapSurface)
	{
		SDL_Surface *temp_surface;
        temp_surface = gVideoDriver.convertThroughBlitSfc(mpBitmapSurface.get());
		mpBitmapSurface.reset( temp_surface, &SDL_FreeSurface );
		return true;
	}
	else
		return false;
}

bool GsBitmap::loadHQBitmap( const std::string& filename )
{
	if(!IsFileAvailable(filename))
		return false;

	if( mpBitmapSurface )
	{
		SDL_Surface *tempSurface = SDL_LoadBMP(GetFullFileName(filename).c_str());
		if(tempSurface)
		{
            SDL_Surface *displaysurface = SDL_ConvertSurface(tempSurface,
                                                             mpBitmapSurface->format,
                                                             mpBitmapSurface->flags);
			SDL_BlitSurface(displaysurface, NULL, mpBitmapSurface.get(), NULL);
			SDL_FreeSurface(displaysurface);
			SDL_FreeSurface(tempSurface);
			return true;
		}
	}
	return false;
}

void GsBitmap::exchangeColor( const Uint32 oldColor, const Uint32 newColor )
{
    SDL_Surface *bmpSfc = mpBitmapSurface.get();

    if(SDL_MUSTLOCK(bmpSfc))
        SDL_LockSurface(bmpSfc);

    Uint8* pixel = (Uint8*) bmpSfc->pixels;

    const int width = bmpSfc->w;
    const int height = bmpSfc->h;

    Uint32 color = 0;
    auto format = bmpSfc->format;

    const int pixelSize = format->BytesPerPixel;

    for( int y=0 ; y<height ; y++ )
    {
        for( int x=0; x<width ; x++ )
        {
            memcpy(&color, pixel, pixelSize );

            if(color == oldColor) // Here we make it transparent
            {
                memcpy(pixel, &newColor, pixelSize );
            }

            pixel += pixelSize;
        }
    }

    if(SDL_MUSTLOCK(bmpSfc))
        SDL_UnlockSurface(bmpSfc);
}

void GsBitmap::exchangeColor( const Uint8 oldR, const Uint8 oldG, const Uint8 oldB,
                             const Uint8 newR, const Uint8 newG, const Uint8 newB)
{
    SDL_Surface *bmpSfc = mpBitmapSurface.get();

    const Uint32 oldColor = SDL_MapRGB( bmpSfc->format, oldR, oldG, oldB );
    const Uint32 newColor = SDL_MapRGB( bmpSfc->format, newR, newG, newB );

    exchangeColor( oldColor, newColor );
}

void GsBitmap::setColorKey(const Uint8 r, const Uint8 g, const Uint8 b)
{
    auto bmpSfc = mpBitmapSurface.get();
    auto format = bmpSfc->format;
    auto colorkey = SDL_MapRGB( format, r, g, b );

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetColorKey(bmpSfc, SDL_TRUE, colorkey);
#else
    SDL_SetColorKey(bmpSfc, SDL_SRCCOLORKEY, colorkey);
#endif
}

bool GsBitmap::scaleTo(const GsRect<Uint16> &destRes)
{
    SDL_Rect newRect = destRes.SDLRect();

    if(newRect.w == mpBitmapSurface->w && newRect.h == mpBitmapSurface->h)
        return true;

    std::shared_ptr<SDL_Surface> newSfc;

    // Need to do that, otherwise it won't work.
    optimizeSurface();

    auto bmpSfc = mpBitmapSurface.get();
    auto bmpFormat = bmpSfc->format;

    newSfc.reset( SDL_CreateRGBSurface(bmpSfc->flags,
                                       newRect.w, newRect.h,
                                       bmpFormat->BitsPerPixel,
                                       bmpFormat->Rmask,
                                       bmpFormat->Gmask,
                                       bmpFormat->Bmask,
                                       bmpFormat->Amask ),
                    &SDL_FreeSurface );

    if(!newSfc)
      return false;

#if SDL_VERSION_ATLEAST(2, 0, 0)

    SDL_BlendMode blendMode;

    SDL_GetSurfaceBlendMode(bmpSfc, &blendMode);
    SDL_SetSurfaceBlendMode(newSfc.get(), blendMode);

#endif

    CVidConfig &vidConf = gVideoDriver.getVidConfig();


    blitScaled(bmpSfc,
               bmpSfc->clip_rect,
               newSfc.get(),
               newRect,
               vidConf.m_ScaleXFilter);

    mpBitmapSurface = newSfc;

    return true;
}


///
// Drawing Routines
///

/**
 * \brief The function that blits the sprite to the main blitSurface
 * \param x		 		X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void GsBitmap::draw(const int x, const int y)
{
    _draw( x, y, gVideoDriver.getBlitSurface() );
}

/**
 * \brief The function that blits the sprite to dst
 * \param x		 		X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 * \param dst           Surface where to blit that Bitmap
 */
void GsBitmap::_draw(const int x, const int y, SDL_Surface *dst) const
{
    SDL_Rect dst_rect, src_rect;

    SDL_Surface *bmpPtr = mpBitmapSurface.get();

    dst_rect.x = x;	dst_rect.y = y;
    src_rect.x = 0; src_rect.y = 0;
    src_rect.w = bmpPtr->w;
    src_rect.h = bmpPtr->h;

    if(dst_rect.x < 0)
    {
        src_rect.x = -dst_rect.x;
        dst_rect.x = 0;
        src_rect.w -= src_rect.x;
    }

    if(dst_rect.x + src_rect.w > dst->w )
    {
        // src Surface too large! Clip it down!
        src_rect.w = dst->w - dst_rect.x;
    }

    if(dst_rect.y < 0)
    {
        src_rect.y = -dst_rect.y;
        dst_rect.y = 0;
        src_rect.h -= src_rect.y;
    }

    if(dst_rect.y + src_rect.h > dst->h )
    {
        // src Surface too large! Clip it down!
        src_rect.h = dst->h - dst_rect.y;
    }

    SDL_BlitSurface( bmpPtr, &src_rect, dst, &dst_rect );
}


