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
//#include "sdl/extensions.h"
#include "graphics/GsGraphics.h"

GsSprite::GsSprite() :
m_alpha(255)
{
	m_xsize = m_ysize = 0;
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


std::shared_ptr<SDL_Surface> GsSprite::createCopySDLSurface(
        const std::shared_ptr<SDL_Surface>& original)
{
    std::shared_ptr<SDL_Surface> surface;

    if(original->format->BitsPerPixel < 16)
    {
        surface.reset(SDL_CreateRGBSurface( 0, m_xsize, m_ysize, 8, 0, 0, 0, 0), &SDL_FreeSurface);
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetPaletteColors(surface->format->palette, original->format->palette->colors, 0, 255);
        SDL_SetColorKey(surface.get(), SDL_TRUE, COLORKEY);
#else
        SDL_SetColors( surface.get(), original->format->palette->colors, 0, 255);
        SDL_SetColorKey( surface.get(), SDL_SRCCOLORKEY, COLORKEY ); // One black is the color key. There is another black, as normal color
#endif

        auto *origSfcPtr = original.get();

        SDL_FillRect( surface.get(), NULL, COLORKEY );
        BlitSurface( origSfcPtr, NULL, surface.get(), NULL);
    }
    else
    {
        surface.reset(SDL_ConvertSurface(original.get(),original->format,0));
    }

    return surface;
}


void GsSprite::copy(const GsSprite& original)
{
    m_alpha = original.getAlpha();
    original.readSize(m_xsize, m_ysize);
    original.readBBox(m_bboxX1, m_bboxY1,
                      m_bboxX2, m_bboxY2);
    original.readOffsets(m_xoffset, m_yoffset);

    auto origSfc = original.getSmartSDLSurface();
    auto origMaskSfc = original.getSmartSDLMaskSurface();

    mName = original.getName();

    if(origSfc)
    {
        mpSurface = createCopySDLSurface( origSfc );
    }

    if(origMaskSfc)
    {
        mpMasksurface = createCopySDLSurface( origMaskSfc );
    }

}

/////////////////////////////
// Initialization Routines //
/////////////////////////////

bool GsSprite::createSurface(Uint32 flags, SDL_Color *Palette)
{        
	mpSurface.reset(SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0), &SDL_FreeSurface);
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetPaletteColors(mpSurface->format->palette, Palette, 0, 255);
    SDL_SetColorKey(mpSurface.get(), SDL_TRUE, COLORKEY);
#else
    SDL_SetColors( mpSurface.get(), Palette, 0, 255);
	SDL_SetColorKey( mpSurface.get(), SDL_SRCCOLORKEY, COLORKEY ); // One black is the color key. There is another black, as normal color
#endif

	mpMasksurface.reset(SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0), &SDL_FreeSurface);
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetPaletteColors(mpMasksurface->format->palette, Palette, 0, 255);
    SDL_SetColorKey(mpMasksurface.get(), SDL_TRUE, COLORKEY);
#else
    SDL_SetColors( mpMasksurface.get(), Palette, 0, 255);
	SDL_SetColorKey( mpMasksurface.get(), SDL_SRCCOLORKEY, COLORKEY ); // color key.
#endif
	
	return ( !mpSurface && !mpMasksurface );
}

bool GsSprite::optimizeSurface()
{
    if(mpSurface)
    {
        mpSurface.reset(gVideoDriver.convertThroughBlitSfc(mpSurface.get()), &SDL_FreeSurface);
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

    mpSurface.reset(SDL_CreateRGBSurface( 0, m_xsize, m_ysize, 32,
                                          format->Rmask,
                                          format->Gmask,
                                          format->Bmask,
                                          format->Amask),
                    &SDL_FreeSurface);

	GsFont &smallFont = gGraphics.getFont(2);

	// Create Text Borders TODO: Make this code to draw better looking fonts
    smallFont.drawFont( mpSurface.get(), pointStr,  0,  1 );
    smallFont.drawFont( mpSurface.get(), pointStr,  1, 0 );
    smallFont.drawFont( mpSurface.get(), pointStr,  2, 1 );
    smallFont.drawFont( mpSurface.get(), pointStr,  1, 2 );

	// Now draw the alternate font. It just has another color.
    smallFont.drawFont( mpSurface.get(), pointStr, 1,  1, true );

	if(SDL_MUSTLOCK(mpSurface.get())) SDL_LockSurface(mpSurface.get());

	// This makes the white pixel transparent TODO: This and other must get more elegant
	Uint8 *pixel = (Uint8*)mpSurface->pixels;

	for( Uint8 y=0 ; y<mpSurface->h ; y++ )
	{
		for( Uint8 x=0 ; x<mpSurface->w ; x++ )
		{
			memcpy( &color, pixel, mpSurface->format->BytesPerPixel );

			SDL_GetRGBA( color, mpSurface->format, &r, &g, &b, &a );

			if( color == 0xFFFFFFFF ) // White
				a = 0;

			color = SDL_MapRGBA( mpSurface->format, r, g, b, a );

			memcpy( pixel, &color, mpSurface->format->BytesPerPixel );

			pixel += mpSurface->format->BytesPerPixel;
		}
	}
	if(SDL_MUSTLOCK(mpSurface.get())) SDL_LockSurface(mpSurface.get());


	m_bboxX1=0;
	m_bboxY1=0;
	m_bboxX2=getWidth();
	m_bboxY2=getHeight();
}



bool GsSprite::loadHQSprite( const std::string& filename )
{
	if(!IsFileAvailable(filename))
		return false;

	if(mpSurface)
	{
		const std::string fullpath = GetFullFileName(filename);

		std::unique_ptr<SDL_Surface, SDL_Surface_Deleter> temp_surface(SDL_LoadBMP( fullpath.c_str() ));
		if(temp_surface)
		{
			std::unique_ptr<SDL_Surface, SDL_Surface_Deleter> 
			      displaysurface( SDL_ConvertSurface(temp_surface.get(), mpSurface->format, mpSurface->flags));
			readMask(displaysurface.get());
			readBBox(displaysurface.get());
			BlitSurface(displaysurface.get(), NULL, mpSurface.get(), NULL);
			return true;
		}
	}
	return false;
}

/**
 * \brief Reads the mask of a created modkeen style bitmap und converts that mask to 8-bit
 * 		  so it can be applied to the others. This is for HQ Sprites, the other have in internal algorithm
 */
void GsSprite::readMask(SDL_Surface *displaysurface)
{
	Uint8 *maskpx, *pixel;
	Uint32 color = 0;
	Uint8 mask = 0;
	Uint8 r,g,b,a;
	Uint16 h,w;

	h = displaysurface->h;
	w = (displaysurface->w)/3;

	if(SDL_MUSTLOCK(displaysurface))
		SDL_LockSurface(displaysurface);
	if(SDL_MUSTLOCK(mpMasksurface.get()))
		SDL_LockSurface(mpMasksurface.get());

	maskpx = (Uint8*)mpMasksurface->pixels;
	pixel = (Uint8*)displaysurface->pixels + (displaysurface->w/3)*displaysurface->format->BytesPerPixel;

	for( Uint8 y=0 ; y<h ; y++ )
	{
		for( Uint8 x=0 ; x<w ; x++ )
		{
			memcpy( &color, pixel, displaysurface->format->BytesPerPixel );

			SDL_GetRGBA( color, displaysurface->format, &r, &g, &b, &a );

			Uint32 mask32 = (r+g+b)/(3*16);
			mask = 15-mask32;

			memcpy( maskpx, &mask, 1 );

			pixel += mpSurface->format->BytesPerPixel;
			maskpx += mpMasksurface->format->BytesPerPixel;
		}
		pixel += 2*w*mpSurface->format->BytesPerPixel;
	}

	if(SDL_MUSTLOCK(mpMasksurface.get()))
		SDL_UnlockSurface(mpMasksurface.get());
	if(SDL_MUSTLOCK(displaysurface))
		SDL_UnlockSurface(displaysurface);
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
	Uint8 *pixel;
	Uint8 *maskpx;
	Uint32 colour, mask;
	Uint8 r,g,b,a;
	
	if( !mpSurface || !mpMasksurface ) return;

    if(mpSurface->format->BitsPerPixel == 8) // In case we did not call SDL_Displayformat before ???
	{
		BlitSurface(mpMasksurface.get(), NULL, mpSurface.get(), NULL);
		return;
    }
	
	if(SDL_MUSTLOCK(mpSurface.get())) SDL_LockSurface(mpSurface.get());
	if(SDL_MUSTLOCK(mpMasksurface.get())) SDL_LockSurface(mpMasksurface.get());
	
	pixel = (Uint8*)mpSurface->pixels;
	maskpx = (Uint8*)mpMasksurface->pixels;
	
	for( Uint8 y=0 ; y<m_ysize ; y++ )
	{
		for( Uint8 x=0 ; x<m_xsize ; x++ )
		{
			memcpy( &colour, pixel, mpSurface->format->BytesPerPixel );
			memcpy( &mask, pixel, mpSurface->format->BytesPerPixel );
			
			SDL_GetRGBA( colour, mpSurface->format, &r, &g, &b, &a );

			if(*maskpx<16)
				a = (255*(*maskpx))/15;
			else a = 255;

			colour = SDL_MapRGBA( mpSurface->format, r, g, b, a );

			memcpy( pixel, &colour, mpSurface->format->BytesPerPixel );
			
			pixel += mpSurface->format->BytesPerPixel;
			maskpx += mpMasksurface->format->BytesPerPixel;
		}
	}
	if(SDL_MUSTLOCK(mpMasksurface)) SDL_LockSurface(mpMasksurface.get());
	if(SDL_MUSTLOCK(mpSurface)) SDL_LockSurface(mpSurface.get());
}

void GsSprite::applyTranslucency(Uint8 value)
{
	Uint8 *pixel;
	Uint32 colour = 0;
	Uint8 r,g,b,a;

	r = g = b = a = 0;		
	
	if( !mpSurface || gVideoDriver.getZoomValue() > 1) 
	    return;

	if(m_alpha == value)
		return;	
	
	
	SDL_PixelFormat *format = mpSurface->format;	
	
	if(format->BitsPerPixel < 24)
	{

        mpSurface.reset(gVideoDriver.convertThroughBlitSfc(mpSurface.get()), &SDL_FreeSurface);

#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetSurfaceAlphaMod(mpSurface.get(), value);
#else
        SDL_SetAlpha(mpSurface.get(), SDL_SRCALPHA, value);
#endif
	    m_alpha = value;
	    return;
	}
	

	if(SDL_MUSTLOCK(mpSurface.get())) SDL_LockSurface(mpSurface.get());

	pixel = (Uint8*)mpSurface->pixels;

	for( Uint8 y=0 ; y<m_ysize ; y++ )
	{
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

	if(SDL_MUSTLOCK(mpSurface.get())) SDL_LockSurface(mpSurface.get());

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
	
	Destination.createSurface( mpSurface->flags, Palette );
	
	SDL_FillRect(Destination.getSDLSurface(), NULL, COLORKEY);
	BlitSurface( mpSurface.get(), NULL, Destination.getSDLSurface(), NULL);
}

// replaces all instances of color find in sprite s with
// color replace, as long as the y is greater than miny
// NOTE: This only can be used, when the surface is at 8-bit colours
// and palettized. Don't use it, after it has been optimized
void GsSprite::replaceSpriteColor( Uint16 find, Uint16 replace, Uint16 miny )
{
	Uint16 x,y;
	Uint8* pixel;
	
	if(SDL_MUSTLOCK(mpSurface.get())) SDL_LockSurface(mpSurface.get());
	pixel = (Uint8*) mpSurface->pixels;
	for(y=miny;y<m_ysize;y++)
	{
		for(x=0;x<m_xsize;x++)
		{
			if (pixel[y*m_xsize + x] ==find)
				pixel[y*m_xsize + x] = replace;
		}
	}
	if(SDL_MUSTLOCK(mpSurface.get())) SDL_UnlockSurface(mpSurface.get());
}



void GsSprite::exchangeSpriteColor( const Uint16 find1, const Uint16 find2, Uint16 miny )
{
    if(!mpSurface) // TODO: This should throw an exception
        return;

    Uint16 x,y;
    Uint8* pixel;

    if(SDL_MUSTLOCK(mpSurface.get())) SDL_LockSurface(mpSurface.get());
    pixel = (Uint8*) mpSurface->pixels;
    for(y=miny;y<m_ysize;y++)
    {
        for(x=0;x<m_xsize;x++)
        {
            if (pixel[y*m_xsize + x] == find1)
                pixel[y*m_xsize + x] = find2;
            else if (pixel[y*m_xsize + x] == find2)
                pixel[y*m_xsize + x] = find1;
        }
    }
    if(SDL_MUSTLOCK(mpSurface.get())) SDL_UnlockSurface(mpSurface.get());
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
	
	for(int y=0;y<dst->h;y++)
	{
		for(int x=0;x<dst->w;x++)
		{
			Uint8 r,g,b,a;
			
			r = g = b = a = 255;
			
			byte *srcPtr = (byte*)src->pixels;
			srcPtr += (pitchsrc*y+x*bytePPsrc);
			
            if(colorkey == *srcPtr)
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
void GsSprite::drawSprite(const int x, const int y, const int w, const int h, const Uint8 alpha )
{

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceAlphaMod( mpSurface.get(), alpha);
#else
    SDL_SetAlpha(mpSurface.get(), SDL_SRCALPHA, alpha);
#endif

    drawSprite( gVideoDriver.getBlitSurface(), x, y, w, h );
}

void GsSprite::drawSprite( SDL_Surface *dst, const int x, const int y, const int w, const int h )
{
	SDL_Rect dst_rect, src_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	src_rect.x = 0;	src_rect.y = 0;
	
	const int max_width = gVideoDriver.getGameResolution().w;
	const int max_height = gVideoDriver.getGameResolution().h;

	if( m_xsize + x > max_width )
		dst_rect.w = max_width - x;

	if( m_ysize + y > max_height )
        dst_rect.h = max_height - y;

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

    src_rect.w = dst_rect.w = w;
    src_rect.h = dst_rect.h = h;

	SDL_Surface *src = mpSurface.get();

	BlitSurface( src, &src_rect, dst, &dst_rect );
}

/**
 * \brief The function that blits the sprite to dst
 * \param x	X-Coordinate, indicating the position on dst
 * \param y	Y-Coordinate, indicating the position on dst
 */
void GsSprite::drawBlinkingSprite( int x, int y )
{
    _drawBlinkingSprite(gVideoDriver.getBlitSurface(), x, y);
}


/**
 * \brief The function that blits the sprite to dst (lowlevel)
 * \param SDL_Surface 	Surface where the sprite will be drawn
 * \param x				X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void GsSprite::_drawBlinkingSprite( SDL_Surface *dst, Uint16 x, Uint16 y )
{
	SDL_Rect dst_rect, src_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	src_rect.x = 0;	src_rect.y = 0;
	src_rect.w = dst_rect.w;
	src_rect.h = dst_rect.h;

//#if SDL_VERSION_ATLEAST(2, 0, 0)
    
//#else
    SDL_Surface *blanksfc = gVideoDriver.convertThroughBlitSfc(mpSurface.get());
	blitMaskedSprite(blanksfc, mpSurface.get(), 0xFFFFFF);
	BlitSurface( blanksfc, &src_rect, dst, &dst_rect );
	SDL_FreeSurface(blanksfc);
//#endif
}
