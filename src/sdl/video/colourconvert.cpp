/*
 * colourconvert.cpp
 *
 *  Created on: 18.03.2009
 *      Author: gerstrong
 */

#include <SDL.h>

/**
 * \brief	This function fades one color channel to c_dest
 * \param	c		Color channel it will get
 * \param	c_dest	Destination color to where the colour we want to get
 */
void fadecolorChannel(Uint8 &c, Uint8 c_dest, Uint8 speed)
{
	Sint16 c16 = c;
	if( c16<c_dest )
	{
		c16 += speed;
		if( c16>c_dest )
			c16 = c_dest;
	}
	else if( c16>c_dest )
	{
		c16 -= speed;
		if( c16<c_dest )
			c16 = c_dest;
	}

	if(c16 > 255)
		c = 0xFF;
	if(c16 < 0)
		c = 0;
	else
		c = static_cast<Uint8>(c16);
}

/**
 * \brief	This function fades the entire color to m_colour
 * \param	m_colour	Color it will get
 * \param	dest_colour	Destination color to where the color we want to get
 * \param	speed		fading speed
 */
void fadetoColour(Uint32 &m_colour, Uint32 dest_colour, Uint8 speed)
{
	Uint8 r, g, b;
	Uint8 r_dest, g_dest, b_dest;

	if(m_colour == dest_colour)
		return;

	// Get the colour components. As the operations are relative
	// we don't need to care about endianess.
	r = (m_colour & 0xFF0000) >> 16;
	g = (m_colour & 0x00FF00) >> 8;
	b = (m_colour & 0x0000FF);

	r_dest = (dest_colour & 0xFF0000) >> 16;
	g_dest = (dest_colour & 0x00FF00) >> 8;
	b_dest = (dest_colour & 0x0000FF);

	fadecolorChannel(r, r_dest, speed);
	fadecolorChannel(g, g_dest, speed);
	fadecolorChannel(b, b_dest, speed);

	m_colour = static_cast<Uint32>(r)<<16;
	m_colour += static_cast<Uint32>(g)<<8;
	m_colour += static_cast<Uint32>(b);
}

/**
 * \brief This function merges every pixel of src to the dst.
 * 		  I think it is the same as BlitSurface by SDL, but in some
 * 		  cases that version cannot Apply the Alpha channel.
 * 		  If someone get's an idea how to get it working, he is welcome
 * 		  I searched high and low in the SDL documentation and got Lost...
 * 		  This only does an OR operation on the surface, so be careful!
 */
void BlitSurfaceMerge( SDL_Surface *src, SDL_Rect *p_src_rect, SDL_Surface *dst, SDL_Rect *p_dst_rect )
{
	SDL_LockSurface(src);
	SDL_LockSurface(dst);

	SDL_Rect src_rect, dst_rect;

	if(p_src_rect == NULL)
	{
		src_rect.x = src_rect.y = 0;
		src_rect.h = src->h;
		src_rect.w = src->w;
	}
	else
		src_rect = *p_src_rect;

	if(p_dst_rect == NULL)
	{
		dst_rect.x = dst_rect.y = 0;
		dst_rect.h = dst->h;
		dst_rect.w = dst->w;
	}
	else
		dst_rect = *p_dst_rect;

	dst_rect.h = src_rect.h;
	dst_rect.w = src_rect.w;

	// trim clipping rects
	if( dst->h < dst_rect.y+dst_rect.h  )
		src_rect.h = dst->h-dst_rect.y;

	if( dst->w < dst_rect.x+dst_rect.w  )
		src_rect.w = dst->w-dst_rect.x;

	Uint8 *pixel_src;
	Uint8 *pixel_dst;
	Uint8 sr,sg,sb,sa;
	Uint8 dr,dg,db,da;

#ifdef ANDROID
	typedef Uint16 PixelFormatType; // We always have 16-bit pixel, and if we'll try to dereference non-4-byte-aligned pointer to Uint32 on ARM CPU we'll get garbage
#else
	typedef Uint32 PixelFormatType;
#endif
	PixelFormatType *pixel_src32;
	PixelFormatType *pixel_dst32;

	for( size_t y=0 ; y<src_rect.h ; y++ )
	{
		pixel_src = (Uint8*)src->pixels + (src_rect.x)*src->format->BytesPerPixel + (src_rect.y+y)*src->pitch;
		pixel_dst = (Uint8*)dst->pixels + (dst_rect.x)*dst->format->BytesPerPixel + (dst_rect.y+y)*dst->pitch;

		for( size_t x=0 ; x<src_rect.w ; x++ )
		{
			pixel_src32 = (PixelFormatType*) (void *)pixel_src;
			pixel_dst32 = (PixelFormatType*) (void *)pixel_dst;
			SDL_GetRGBA(*pixel_src32, src->format, &sr, &sg, &sb, &sa);
			SDL_GetRGBA(*pixel_dst32, dst->format, &dr, &dg, &db, &da);

			dr |= sr;
			dg |= sg;
			db |= sb;
			da |= sa;
			*pixel_dst32 = SDL_MapRGBA(dst->format, dr, dg, db, da);
			pixel_src += src->format->BytesPerPixel;
			pixel_dst += dst->format->BytesPerPixel;
		}
	}

	SDL_UnlockSurface(dst);
	SDL_UnlockSurface(src);
}
