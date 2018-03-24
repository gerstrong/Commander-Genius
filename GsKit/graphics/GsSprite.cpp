/*
 * GsSprite.cpp
 *
 *  Created on: 02.09.2009
 *      Author: gerstrong
 */

#include "GsSprite.h"
#include "GsPalette.h"
#include <base/utils/FindFile.h>
#include <string.h>
#include <base/video/CVideoDriver.h>
#include "graphics/GsGraphics.h"
#include "graphics/GsGraphics.h"

GsSprite::GsSprite()
{
    m_bboxX1 = m_bboxY1 = 0;
	m_bboxX2 = m_bboxY2 = 0;
	m_xoffset = m_yoffset = 0;
}

GsSprite::GsSprite(const GsSprite& original)
{
    this->copy(original);
}

GsSprite GsSprite::operator=(const GsSprite& original)
{
    this->copy(original);
    return *this;
}



void GsSprite::copy(const GsSprite& original)
{
    m_alpha = original.getAlpha();
    original.readSize(m_xsize, m_ysize);
    original.readBBox(m_bboxX1, m_bboxY1,
                      m_bboxX2, m_bboxY2);
    original.readOffsets(m_xoffset, m_yoffset);

    mName = original.getName();

    const auto &origSfc =  original.Surface();
    const auto &origMaskSfc =  original.MaskSurface();

    // If the sprites are not yet loaded, the surfaces
    // do not exist yet. Only copy those if they exist
    if(origSfc)
    {
        mSurface.createCopy(origSfc);
    }

    if(origMaskSfc)
    {
        mMaskSurface.createCopy(origMaskSfc);
    }
}

void GsSprite::copyTilted(const GsSprite& original)
{
    // here the coordinates are swapped
    m_alpha = original.getAlpha();
    original.readSize(m_ysize, m_xsize);
    original.readBBox(m_bboxY1, m_bboxX1,
                      m_bboxY2, m_bboxX2);
    original.readOffsets(m_yoffset, m_xoffset);

    mName = original.getName();

    const auto &origSfc =  original.Surface();
    const auto &origMaskSfc =  original.MaskSurface();

    // If the sprites are not yet loaded, the surfaces
    // do not exist yet. Only copy those if they exist
    if(origSfc)
    {
        mSurface.createCopy(origSfc);
        mSurface.tiltSurface();
    }

    if(origMaskSfc)
    {
        mMaskSurface.createCopy(origMaskSfc);
        mMaskSurface.tiltSurface();
    }
}


bool GsSprite::createSurface(Uint32 flags, SDL_Color *Palette)
{        
    mSurface.create(flags, m_xsize, m_ysize, 8, 0, 0, 0, 0);
    mSurface.setPaletteColors(Palette);
    mSurface.setColorKey(COLORKEY);

    mMaskSurface.create(flags, m_xsize, m_ysize, 8, 0, 0, 0, 0);
    mMaskSurface.setPaletteColors(Palette);
    mMaskSurface.setColorKey(COLORKEY);

    return ( mSurface.empty() && mMaskSurface.empty() );
}

GsBitmap
GsSprite::exportBitmap()
{
    const GsWeakSurface weak = GsWeakSurface(mSurface);
    GsBitmap bmp(weak);

    bmp.trimWidth(bmp.width()/3);

    return bmp;
}

bool GsSprite::optimizeSurface()
{
    if(mSurface)
    {
        mSurface.makeBlitCompatible();
    }
    if(mMaskSurface)
    {
        mMaskSurface.makeBlitCompatible();
    }

    return true;
}

bool GsSprite::empty()
{
    if(!mSurface)
    {
        return true;
    }

    return false;
}


void GsSprite::generateSprite( const int points )
{
	Uint32 color = 0;
	Uint8 r,g,b,a;
	std::string pointStr = itoa(points);

    // +2 for the outline
    setSize( (pointStr.size()*8)+2, 8+2);

    SDL_Surface *blit = gVideoDriver.getBlitSurface();
    SDL_PixelFormat *format = blit->format;

    SDL_Rect rect;
    rect.w = m_xsize;
    rect.h = m_ysize;

    mSurface.createRGBSurface(rect);

	GsFont &smallFont = gGraphics.getFont(2);

	// Create Text Borders TODO: Make this code to draw better looking fonts
    smallFont.drawFont( mSurface, pointStr,  0, 1, false );
    smallFont.drawFont( mSurface, pointStr,  1, 0, false );
    smallFont.drawFont( mSurface, pointStr,  2, 1, false );
    smallFont.drawFont( mSurface, pointStr,  1, 2, false );

	// Now draw the alternate font. It just has another color.
    smallFont.drawFont( mSurface, pointStr, 1,  1, true );

    mSurface.lock();

    // This makes the white pixel transparent
    Uint8 *pixel = mSurface.PixelPtr();

    const size_t bpp = format->BytesPerPixel;

    for( Uint8 y=0 ; y<mSurface.height() ; y++ )
	{
        for( Uint8 x=0 ; x<mSurface.width() ; x++ )
		{
            memcpy( &color, pixel, bpp );

            mSurface.getRGBA(color, r, g, b, a);

			if( color == 0xFFFFFFFF ) // White
				a = 0;

            color = mSurface.mapRGBA(r, g, b, a);
            memcpy( pixel, &color, bpp );

            pixel += bpp;
		}
	}

    mSurface.unlock();


	m_bboxX1=0;
	m_bboxY1=0;
	m_bboxX2=getWidth();
	m_bboxY2=getHeight();
}



bool GsSprite::loadHQSprite( const std::string& filename )
{
    if(!IsFileAvailable(filename))
    {
        return false;
    }

    const std::string fullpath = GetFullFileName(filename);

    GsSurface bmpSfc;

    if(!bmpSfc.loadBmp(fullpath))
    {
        return false;
    }

    mSurface.createCopy(bmpSfc);
    mMaskSurface.createCopy(bmpSfc);

    m_xsize = bmpSfc.width()/3;
    m_ysize = bmpSfc.height();

    readMask(bmpSfc.getSDLSurface());
    readBBox(bmpSfc.getSDLSurface());

    mSurface.makeBlitCompatible();

    return true;
}

/**
 * \brief Reads the mask of a created modkeen style bitmap und converts that mask to 8-bit
 * 		  so it can be applied to the others. This is for HQ Sprites, the other ones have an internal algorithm
 */
void GsSprite::readMask(SDL_Surface *srcSfc)
{
    assert(srcSfc);

	Uint8 *maskpx, *pixel;
	Uint32 color = 0;
	Uint8 mask = 0;
	Uint8 r,g,b,a;

    // We have three fragments in one bitmap. The second fragment is the mask
    Uint16 w = (srcSfc->w)/3;

    assert(w>0);

    if(SDL_MUSTLOCK(srcSfc))
    {
        SDL_LockSurface(srcSfc);
    }

    mMaskSurface.lock();

    auto maskSfc = mMaskSurface.getSDLSurface();

    // In same occasions the dimensions are not exact. In that case, apply the minimum
    const auto minW = std::min(Uint16(srcSfc->w/3), mMaskSurface.width());
    const auto minH = std::min(Uint16(srcSfc->h), mMaskSurface.height());

    Uint8* srcLinePtr = static_cast<Uint8*>(srcSfc->pixels) + minW*srcSfc->format->BytesPerPixel;
    Uint8* maskLinePtr = static_cast<Uint8*>(maskSfc->pixels);

    for( Uint8 y=0 ; y<minH ; y++ )
	{                
        pixel = srcLinePtr;
        maskpx = maskLinePtr;

        for( Uint8 x=0 ; x<minW ; x++ )
		{
            memcpy( &color, pixel, srcSfc->format->BytesPerPixel );

            SDL_GetRGBA( color, srcSfc->format, &r, &g, &b, &a );

			Uint32 mask32 = (r+g+b)/(3*16);
			mask = 15-mask32;

            memcpy( maskpx, &mask, srcSfc->format->BytesPerPixel );

            pixel  += srcSfc->format->BytesPerPixel;
            maskpx += maskSfc->format->BytesPerPixel;
		}

        srcLinePtr  += srcSfc->w * srcSfc->format->BytesPerPixel;
        maskLinePtr += maskSfc->w * maskSfc->format->BytesPerPixel;
	}

    mMaskSurface.unlock();

    if(SDL_MUSTLOCK(srcSfc))
    {
        SDL_UnlockSurface(srcSfc);
    }
}

/**
 * \brief Reads the bounding box of a created modkeen style bitmap and assigns new coordinates
 */
void GsSprite::readBBox(SDL_Surface *displaysurface)
{
	// TODO: That code need to be implemented
}

void GsSprite::applyTransparency()
{
    if( mSurface.empty() || mMaskSurface.empty() ) return;

    mSurface.lock();
    mMaskSurface.lock();
	
    const auto bpp = mSurface.getSDLSurface()->format->BytesPerPixel;
    const auto maskBpp = mMaskSurface.getSDLSurface()->format->BytesPerPixel;

    // In same occasions the dimensions are not exact. In that case, apply the minimum
    const auto minW = std::min(Uint16(mSurface.width()), mMaskSurface.width());
    const auto minH = std::min(Uint16(mSurface.height()), mMaskSurface.height());

    for( int y=0 ; y<minH ; y++ )
	{
        Uint8 *pixel  = mSurface.PixelPtr() + y*mSurface.getSDLSurface()->pitch;
        Uint8 *maskpx = mMaskSurface.PixelPtr() + y*mMaskSurface.getSDLSurface()->pitch;

        for( int x=0 ; x<minW ; x++ )
		{
            Uint32 colour;
            Uint8 r,g,b,a;
            memcpy( &colour, pixel, bpp );
			
            mSurface.getRGBA(colour, r, g, b, a);

            const Uint8 maskCol = *maskpx;

            if(maskCol<16)
            {
                a = (255*maskCol)/15;
            }
            else
            {
                a = 255;
            }

            colour = mSurface.mapRGBA(r, g, b, a);

            memcpy( pixel, &colour, bpp );
			
            pixel  += bpp;
            maskpx += maskBpp;
		}
	}

    mMaskSurface.unlock();
    mSurface.unlock();
}

void GsSprite::applyTranslucency(Uint8 value)
{
	Uint32 colour = 0;
	Uint8 r,g,b,a;

	r = g = b = a = 0;		
	
    if( !mSurface || gVideoDriver.getZoomValue() > 1)
	    return;

	if(m_alpha == value)
		return;	
	
	
    SDL_PixelFormat *format = mSurface.getSDLSurface()->format;
	
	if(format->BitsPerPixel < 24)
	{
        mSurface.makeBlitCompatible();
        mSurface.setAlpha(value);
	    m_alpha = value;
	    return;
	}
	
    mSurface.lock();


	for( Uint8 y=0 ; y<m_ysize ; y++ )
	{
        auto pixel = mSurface.PixelPtr() + y*mSurface.getSDLSurface()->pitch;

		for( Uint8 x=0 ; x<m_xsize ; x++ )
		{
			memcpy( &colour, pixel, format->BytesPerPixel );

			SDL_GetRGBA( colour, format, &r, &g, &b, &a );

			if(a!=0) a = value;

			colour = SDL_MapRGBA( format, r, g, b, a );

			memcpy( pixel, &colour, format->BytesPerPixel );

			pixel += format->BytesPerPixel;
		}
	}

    mSurface.unlock();

	m_alpha = value;
}

///
// Getters and Setters
///
void GsSprite::setBoundingBoxCoordinates( Sint32 bboxx1, Sint32 bboxy1, Sint32 bboxx2, Sint32 bboxy2 )
{
	m_bboxX1 = bboxx1;
	m_bboxY1 = bboxy1;
	m_bboxX2 = bboxx2;
	m_bboxY2 = bboxy2;
}

void GsSprite::copy( GsSprite &Destination, SDL_Color *Palette )
{
	Destination.m_bboxX1 = m_bboxX1;
	Destination.m_bboxY1 = m_bboxY1;
	Destination.m_bboxX2 = m_bboxX2;
	Destination.m_bboxY2 = m_bboxY2;
	Destination.setSize(m_xsize, m_ysize);
	
    Destination.createSurface( mSurface.getSDLSurface()->flags, Palette );

    auto &destSfc = Destination.Surface();

    destSfc.fill(COLORKEY);
    mSurface.blitTo(destSfc);
}

// replaces all instances of color find in sprite s with
// color replace, as long as the y is greater than miny
// NOTE: This only can be used, when the surface is at 8-bit colours
// and palettized. Don't use it, after it has been optimized
void GsSprite::replaceSpriteColor( Uint16 find, Uint16 replace, Uint16 miny )
{
    mSurface.lock();
    const auto pixel = mSurface.PixelPtr();

    for(Uint16  y=miny ; y<m_ysize ; y++)
	{
        for(Uint16  x=0 ; x<m_xsize ; x++)
		{
            if (pixel[y*m_xsize + x] == find)
            {
				pixel[y*m_xsize + x] = replace;
            }
		}
	}

    mSurface.unlock();
}



void GsSprite::exchangeSpriteColor( const Uint16 find1, const Uint16 find2, Uint16 miny )
{        
    mSurface.lock();

    auto pixel = mSurface.PixelPtr();
    for(Uint16 y=miny ; y<m_ysize ; y++)
    {
        for(Uint16 x=0; x<m_xsize ; x++)
        {
            if (pixel[y*m_xsize + x] == find1)
            {
                pixel[y*m_xsize + x] = find2;
            }
            else if (pixel[y*m_xsize + x] == find2)
            {
                pixel[y*m_xsize + x] = find1;
            }
        }
    }

    mSurface.unlock();
}



void blitMaskedSprite(SDL_Surface *dst, SDL_Surface *src, Uint32 color)
{
	if(SDL_MUSTLOCK(dst)) SDL_LockSurface(dst);
	if(SDL_MUSTLOCK(src)) SDL_LockSurface(src);
	
	const int bytePPdst = dst->format->BytesPerPixel;
	const int pitchdst = dst->pitch;
	const int bytePPsrc = src->format->BytesPerPixel;
	const int pitchsrc = src->pitch;

    Uint32 colorkey;

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_GetColorKey(dst, &colorkey);
#else
    colorkey = dst->format->colorkey;
#endif
	
    for(int y=0 ; y<dst->h ; y++)
	{
        for(int x=0 ; x<dst->w ; x++)
		{
			Uint8 r,g,b,a;			
			r = g = b = a = 255;
			
			byte *srcPtr = (byte*)src->pixels;
			srcPtr += (pitchsrc*y+x*bytePPsrc);

            Uint32 curColor;
            memcpy( &curColor, srcPtr, bytePPdst );

            Uint8 curR, curG, curB, curA;

            SDL_GetRGBA(curColor, src->format, &curR, &curG, &curB, &curA);
			
            if(curA > 0x8F || colorkey == curColor)
			{			
			  Uint32 newValue = SDL_MapRGBA(dst->format, r, g, b, a);
			
			  byte *dstPtr = (byte*)dst->pixels;
			  dstPtr += (pitchdst*y+x*bytePPdst);
			
			  memcpy( dstPtr, &newValue, bytePPdst );			  
			}
		}
	}
	if(SDL_MUSTLOCK(src)) SDL_UnlockSurface(src);
	if(SDL_MUSTLOCK(dst)) SDL_UnlockSurface(dst);
}


///
// Drawing Routines
///

/**
 * \brief The function that blits the sprite to dst
 * \param SDL_Surface 	Surface where the sprite will be drawn
 * \param x				X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void GsSprite::drawSprite(const int x,
                          const int y,
                          const int w,
                          const int h,
                          const Uint8 alpha )
{
    mSurface.setAlpha(alpha);
    drawSprite( gVideoDriver.getBlitSurface(), x, y, w, h );
}

void GsSprite::drawSprite( SDL_Surface *dst,
                           const int x,
                           const int y,
                           const int w,
                           const int h )
{
    if(mSurface.empty())
    {
        return;
    }

	SDL_Rect dst_rect, src_rect;

    src_rect.x = 0;	        src_rect.y = 0;
	dst_rect.x = x;			dst_rect.y = y;
    dst_rect.w = w;	        dst_rect.h = w;


    const auto &gameRes = gVideoDriver.getGameResolution();
	
    const int max_width  = gameRes.w;
    const int max_height = gameRes.h;

	if( m_xsize + x > max_width )
		dst_rect.w = max_width - x;

	if( m_ysize + y > max_height )
        dst_rect.h = max_height - y;

    src_rect.w = dst_rect.w = w;
    src_rect.h = dst_rect.h = h;

    if( dst_rect.x < 0 )
    {
        src_rect.x = -x;
        src_rect.w += x;
        dst_rect.x = 0;
    }

    if( dst_rect.y < 0 )
    {
        src_rect.y = -y;
        src_rect.h += y;
        dst_rect.y = 0;
    }


    SDL_Surface *src = mSurface.getSDLSurface();

	BlitSurface( src, &src_rect, dst, &dst_rect );
}

/**
 * \brief The function that blits the sprite to dst
 * \param x	X-Coordinate, indicating the position on dst
 * \param y	Y-Coordinate, indicating the position on dst
 */
void GsSprite::drawBlinkingSprite( const int x, const int y )
{
    _drawBlinkingSprite(gVideoDriver.getBlitSurface(), x, y);
}


/**
 * \brief The function that blits the sprite to dst (lowlevel)
 * \param SDL_Surface 	Surface where the sprite will be drawn
 * \param x				X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void GsSprite::_drawBlinkingSprite( SDL_Surface *dst,
                                    const int x,
                                    const int y )
{
    GsWeakSurface blit(dst);

    GsRect<Uint16> dstRect(x, y, m_xsize, m_ysize);

    GsSurface blankSfc;

    blankSfc.createCopy(mSurface);

    blitMaskedSprite(blankSfc.getSDLSurface(), mSurface.getSDLSurface(), 0xFFFFFF);

    blankSfc.blitTo(blit, dstRect);
}
