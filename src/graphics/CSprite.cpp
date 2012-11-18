/*
 * CSprite.cpp
 *
 *  Created on: 02.09.2009
 *      Author: gerstrong
 */

#include "CSprite.h"
#include "CPalette.h"
#include "FindFile.h"
#include <string.h>
#include "sdl/CVideoDriver.h"
#include "sdl/extensions.h"
#include "graphics/CGfxEngine.h"

CSprite::CSprite() :
m_alpha(255)
{
	m_xsize = m_ysize = 0;
	m_bboxX1 = m_bboxY1 = 0;
	m_bboxX2 = m_bboxY2 = 0;
	m_xoffset = m_yoffset = 0;
}

/////////////////////////////
// Initialization Routines //
/////////////////////////////

bool CSprite::createSurface(Uint32 flags, SDL_Color *Palette)
{
	mpSurface.reset(SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0), &SDL_FreeSurface);
	SDL_SetColors( mpSurface.get(), Palette, 0, 255);
	SDL_SetColorKey( mpSurface.get(), SDL_SRCCOLORKEY, COLORKEY ); // One black is the color key. There is another black, as normal color

	mpMasksurface.reset(SDL_CreateRGBSurface( flags, m_xsize, m_ysize, 8, 0, 0, 0, 0), &SDL_FreeSurface);
	SDL_SetColors( mpMasksurface.get(), Palette, 0, 255);
	SDL_SetColorKey( mpMasksurface.get(), SDL_SRCCOLORKEY, COLORKEY ); // color key.
	
	return ( !mpSurface && !mpMasksurface );
}

bool CSprite::optimizeSurface()
{
    if(mpSurface)
	mpSurface.reset(SDL_DisplayFormatAlpha(mpSurface.get()), &SDL_FreeSurface);

    return true;
}



void CSprite::generateSprite( const int points )
{
	Uint32 color = 0;
	Uint8 r,g,b,a;
	std::string pointStr = itoa(points);

	setSize( pointStr.size()*8, 8);

	createSurface( g_pVideoDriver->mpVideoEngine->getBlitSurface()->flags, g_pGfxEngine->Palette.m_Palette  );
	optimizeSurface();

	SDL_FillRect(mpSurface.get(), NULL, 0xFFFFFFFF);

	CFont &smallFont = g_pGfxEngine->getFont(2);

	// Create Text Borders TODO: Make this code to draw better looking fonts
	smallFont.drawFont( mpSurface.get(), pointStr, -1,  0 );
	smallFont.drawFont( mpSurface.get(), pointStr,  0, -1 );
	smallFont.drawFont( mpSurface.get(), pointStr,  1, 0 );
	smallFont.drawFont( mpSurface.get(), pointStr,  0, 1 );

	// Now draw the alternate font. It just has another color.
	smallFont.drawFont( mpSurface.get(), pointStr, 0,  0, true );

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



bool CSprite::loadHQSprite( const std::string& filename )
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
			SDL_BlitSurface(displaysurface.get(), NULL, mpSurface.get(), NULL);
			return true;
		}
	}
	return false;
}

/**
 * \brief Reads the mask of a created modkeen style bitmap und converts that mask to 8-bit
 * 		  so it can be applied to the others. This is for HQ Sprites, the other have in internal algorithm
 */
void CSprite::readMask(SDL_Surface *displaysurface)
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
void CSprite::readBBox(SDL_Surface *displaysurface)
{
	// TODO: That code need to be implemented
}

void CSprite::applyTransparency()
{
	Uint8 *pixel;
	Uint8 *maskpx;
	Uint32 colour, mask;
	Uint8 r,g,b,a;
	
	if( !mpSurface || !mpMasksurface ) return;

	if(mpSurface->format->BitsPerPixel == 8) // In case we did not call SDL_Displayformat before ???
	{
		SDL_BlitSurface(mpMasksurface.get(), NULL, mpSurface.get(), NULL);
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

void CSprite::applyTranslucency(Uint8 value)
{
	Uint8 *pixel;
	Uint32 colour = 0;
	Uint8 r,g,b,a;

	r = g = b = a = 0;		
	
	if( !mpSurface || g_pVideoDriver->getZoomValue() > 1) 
	    return;

	if(m_alpha == value)
		return;	
	
	
	SDL_PixelFormat *format = mpSurface->format;	
	
	if(format->BitsPerPixel < 24)
	{
	    SDL_SetAlpha(mpSurface.get(), SDL_SRCALPHA, value);
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
void CSprite::setBoundingBoxCoordinates( Sint32 bboxx1, Sint32 bboxy1, Sint32 bboxx2, Sint32 bboxy2 )
{
	m_bboxX1 = bboxx1;
	m_bboxY1 = bboxy1;
	m_bboxX2 = bboxx2;
	m_bboxY2 = bboxy2;
}

void CSprite::copy( CSprite &Destination, SDL_Color *Palette )
{
	Destination.m_bboxX1 = m_bboxX1;
	Destination.m_bboxY1 = m_bboxY1;
	Destination.m_bboxX2 = m_bboxX2;
	Destination.m_bboxY2 = m_bboxY2;
	Destination.setSize(m_xsize, m_ysize);
	
	Destination.createSurface( mpSurface->flags, Palette );
	
	SDL_FillRect(Destination.getSDLSurface(), NULL, COLORKEY);
	SDL_BlitSurface( mpSurface.get(), NULL, Destination.getSDLSurface(), NULL);
}

// replaces all instances of color find in sprite s with
// color replace, as long as the y is greater than miny
// NOTE: This only can be used, when the surface is at 8-bit colours
// and palettized. Don't use it, after it has been optimized
void CSprite::replaceSpriteColor( Uint16 find, Uint16 replace, Uint16 miny )
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

void blitMaskedSprite(SDL_Surface *dst, SDL_Surface *src, Uint32 color)
{
	Uint16 x,y;
	Uint32 *srcpixel, *dstpixel;

	if(SDL_MUSTLOCK(dst)) SDL_LockSurface(dst);
	if(SDL_MUSTLOCK(src)) SDL_LockSurface(src);
	dstpixel = (Uint32*) dst->pixels;
	srcpixel = (Uint32*) src->pixels;
	for(y=0;y<dst->h;y++)
	{
		for(x=0;x<dst->w;x++)
		{
			Uint8 r,g,b,a;

			SDL_GetRGBA(*srcpixel,src->format, &r, &g, &b, &a);
			SDL_GetRGB(color, src->format, &r, &g, &b);
			*dstpixel = SDL_MapRGBA(dst->format, r, g, b, a);

			dstpixel++;
			srcpixel++;
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
void CSprite::drawSprite( const Uint16 x, const Uint16 y, const Uint8 alpha )
{
	g_pVideoDriver->mDrawTasks.add( new DrawSpriteTask( this, x, y, alpha ) );
}

/**
 * \brief The function that blits the sprite to dst (Low-Level)
 * \param SDL_Surface 	Surface where the sprite will be drawn
 * \param x				X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void CSprite::_drawSprite( SDL_Surface *dst, const Uint16 x, const Uint16 y, const Uint8 alpha )
{
	SDL_Rect dst_rect, src_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	src_rect.x = 0;	src_rect.y = 0;
	
	const int max_width = g_pVideoDriver->getGameResolution().w;
	const int max_height = g_pVideoDriver->getGameResolution().h;

	if( m_xsize + x > max_width )
		dst_rect.w = max_width - x;

	if( m_ysize + y > max_height )
		dst_rect.h = max_height - y;

	src_rect.w = dst_rect.w;
	src_rect.h = dst_rect.h;

	
	SDL_Surface *src = mpSurface.get();
	
	
	if(g_pVideoDriver->getVidConfig().m_ScaleXFilter > 1)
	    applyTranslucency(alpha);
		
	SDL_BlitSurface( src, &src_rect, dst, &dst_rect );
}

/**
 * \brief The function that blits the sprite to dst
 * \param x	X-Coordinate, indicating the position on dst
 * \param y	Y-Coordinate, indicating the position on dst
 */
void CSprite::drawBlinkingSprite( Uint16 x, Uint16 y )
{
	g_pVideoDriver->mDrawTasks.add( new DrawBlinkingSpriteTask( this, x, y ) );
}


/**
 * \brief The function that blits the sprite to dst (lowlevel)
 * \param SDL_Surface 	Surface where the sprite will be drawn
 * \param x				X-Coordinate, indicating the position on dst
 * \param y				Y-Coordinate, indicating the position on dst
 */
void CSprite::_drawBlinkingSprite( SDL_Surface *dst, Uint16 x, Uint16 y )
{
	SDL_Rect dst_rect, src_rect;
	dst_rect.x = x;			dst_rect.y = y;
	dst_rect.w = m_xsize;	dst_rect.h = m_ysize;

	src_rect.x = 0;	src_rect.y = 0;
	src_rect.w = dst_rect.w;
	src_rect.h = dst_rect.h;

	SDL_Surface *blanksfc =	SDL_DisplayFormatAlpha(mpSurface.get());
	blitMaskedSprite(blanksfc, mpSurface.get(), 0xFFFFFF);
	SDL_BlitSurface( blanksfc, &src_rect, dst, &dst_rect );
	SDL_FreeSurface(blanksfc);
}
