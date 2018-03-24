/*
 * GsBitmap.cpp
 *
 *  Created on: 03.09.2009
 *      Author: gerstrong
 */

#include "GsBitmap.h"
#include "GsPalette.h"
#include <base/utils/FindFile.h>
#include <base/video/CVideoDriver.h>

#include <base/GsLogging.h>

GsBitmap::GsBitmap()
{}


GsBitmap::GsBitmap(const GsWeakSurface &sfc)
{
    mBitmapSurface.createCopy(sfc);
}


GsBitmap::GsBitmap(const GsBitmap &bitmap) :
mName(bitmap.getName())
{    
    if(bitmap.empty())
    {
        return;
    }

    mBitmapSurface.createCopy(bitmap.mBitmapSurface);
}

GsBitmap::GsBitmap(GsBitmap &bitmap) :
mName(bitmap.getName())
{
    if(bitmap.empty())
    {
        return;
    }

    mBitmapSurface.createCopy(bitmap.mBitmapSurface);
}


/**
 * @brief Copy bitmap using the assignment operator
 * @param bmp
 */
GsBitmap & GsBitmap::operator=(const GsBitmap &bmp)
{
    mName = bmp.getName();

    if(!bmp.empty())
    {
        mBitmapSurface.createCopy(bmp.mBitmapSurface);
    }

    return *this;
}


///
// Creation Routines
///
bool GsBitmap::createSurface(Uint32 flags, SDL_Rect rect, SDL_Color *Palette)
{        
    mBitmapSurface.create( flags, rect.w, rect.h, 8, 0, 0, 0, 0 );

    mBitmapSurface.setPaletteColors(Palette);
    mBitmapSurface.setColorKey(COLORKEY);

    if(mBitmapSurface)
    {
	  return true;
    }
	else
    {
	  return false;
    }
}

void GsBitmap::trimWidth(const int newWidth)
{
    SDL_Rect rect = {        0, 0,
                      newWidth, height() };

    GsSurface sfc;
    sfc.createRGBSurface(rect);

    mBitmapSurface.blitTo(sfc, rect);

    mBitmapSurface.createCopy(sfc);
}

bool GsBitmap::optimizeSurface()
{
    if(mBitmapSurface)
	{
        mBitmapSurface.makeBlitCompatible();
		return true;
	}
	else
    {
		return false;
    }
}

bool GsBitmap::loadImg(const std::string& filename)
{
    if(!IsFileAvailable(filename))
    {
        return false;
    }

    //if(mBitmapSurface)
    {
        if (mBitmapSurface.loadImg(GetFullFileName(filename)))
        {
            mBitmapSurface.makeBlitCompatible();
            return true;
        }
    }
    return false;
}

bool GsBitmap::loadHQBitmap( const std::string& filename )
{
	if(!IsFileAvailable(filename))
    {
		return false;
    }

    //if(mBitmapSurface)
	{
        if (mBitmapSurface.loadBmp(GetFullFileName(filename)))
        {
            mBitmapSurface.makeBlitCompatible();
            return true;
        }
	}
	return false;
}

void GsBitmap::exchangeColor( const Uint32 oldColor, const Uint32 newColor )
{    
    mBitmapSurface.lock();

    SDL_Surface *bmpSfc = mBitmapSurface.getSDLSurface();

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

    mBitmapSurface.unlock();
}

void GsBitmap::exchangeColor( const Uint8 oldR, const Uint8 oldG, const Uint8 oldB,
                             const Uint8 newR, const Uint8 newG, const Uint8 newB)
{
    const auto oldColor = mBitmapSurface.mapRGB(oldR, oldG, oldB);
    const auto newColor = mBitmapSurface.mapRGB(newR, newG, newB);

    exchangeColor( oldColor, newColor );
}


void GsBitmap::setPerSurfaceAlpha(const Uint8 alpha)
{
    mBitmapSurface.setAlpha(alpha);

    Uint8 r, g, b;
    mBitmapSurface.readColorKey(r, g, b);
    mBitmapSurface.applyDisplayFormat();
    mBitmapSurface.setColorKey( r, g, b );
}

void GsBitmap::setColorKey(const Uint8 r, const Uint8 g, const Uint8 b)
{
    const auto colorkey = mBitmapSurface.mapRGB(r, g, b);
    mBitmapSurface.setColorKey(colorkey);
}

bool GsBitmap::scaleTo(const GsRect<Uint16> &destRes)
{
    CVidConfig &vidConf = gVideoDriver.getVidConfig();

    if(mBitmapSurface.scaleTo(destRes, vidConf.m_ScaleXFilter))
    {
        return true;
    }
    else
    {
        return false;
    }
}


///
// Drawing Routines
///


void GsBitmap::draw(const int x, const int y) const
{
    _draw( x, y, gVideoDriver.getBlitSurface() );
}


void GsBitmap::draw(const int x,
                    const int y,
                    GsWeakSurface &sfc) const
{
    _draw( x, y, sfc.getSDLSurface() );
}



/**
 * \brief The function that blits the sprite to dst
 * \param x		 		X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 * \param dst           Surface where to blit that Bitmap
 */
void GsBitmap::_draw(const int x,
                     const int y,
                     SDL_Surface *dst) const
{
    SDL_Rect dst_rect, src_rect;

    assert(dst);

    GsWeakSurface weakDst(dst);

    dst_rect.x = x;	dst_rect.y = y;
    src_rect.x = 0; src_rect.y = 0;
    src_rect.w = mBitmapSurface.width();
    src_rect.h = mBitmapSurface.height();

    if(dst_rect.x < 0)
    {
        src_rect.x = -dst_rect.x;
        dst_rect.x = 0;
        src_rect.w -= src_rect.x;
    }

    if(dst_rect.x + src_rect.w > dst->w )
    {
        // src Surface too large, trim it!
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
        // src Surface too large, trim it!
        src_rect.h = dst->h - dst_rect.y;
    }

    mBitmapSurface.blitTo(weakDst, src_rect, dst_rect);
}


