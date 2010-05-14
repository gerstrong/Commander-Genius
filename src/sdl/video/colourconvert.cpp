/*
 * colourconvert.cpp
 *
 *  Created on: 18.03.2009
 *      Author: gerstrong
 */

#include <SDL.h>
#include "colourtable.h"

Uint16 convert4to16BPPcolor(Uint8 c, SDL_Surface *BlitSurface)
{
	// This function converts the colour from the keen palette to 16-bit format
	int alpha = 128;

	if(c == 16)
		return SDL_MapRGBA(BlitSurface->format,1,0,0,alpha);
	else if(c == COLOUR_BLACK)
		return 0;
	else if(c == 1)
		return SDL_MapRGBA(BlitSurface->format,0,0,170,alpha);
	else if(c == 2)
		return SDL_MapRGBA(BlitSurface->format,0,170,0,alpha);
	else if(c == 3)
		return SDL_MapRGBA(BlitSurface->format,0,170,170,alpha);
	else if(c == COLOUR_DARKRED)
		return SDL_MapRGBA(BlitSurface->format,170,0,0,alpha);
	else if(c == 5)
		return SDL_MapRGBA(BlitSurface->format,170,0,170,alpha);
	else if(c == 6)
		return SDL_MapRGBA(BlitSurface->format,170,85,0,alpha);
	else if(c == COLOUR_GREY)
		return SDL_MapRGBA(BlitSurface->format,170,170,170,alpha);
	else if(c == 8)
		return SDL_MapRGBA(BlitSurface->format,85,85,85,alpha);
	else if(c == 9)
		return SDL_MapRGBA(BlitSurface->format,85,85,255,alpha);
	else if(c == 10)
		return SDL_MapRGBA(BlitSurface->format,85,255,85,alpha);
	else if(c == COLOUR_CYAN)
		return SDL_MapRGBA(BlitSurface->format,85,255,255,alpha);
	else if(c == 12)
		return SDL_MapRGBA(BlitSurface->format,255,85,85,alpha);
	else if(c == 13)
		return SDL_MapRGBA(BlitSurface->format,255,85,255,alpha);
	else if(c == 14)
		return SDL_MapRGBA(BlitSurface->format,255,255,85,alpha);
	else if(c == 15)
		return SDL_MapRGBA(BlitSurface->format,255,255,255,alpha);
	else
		return SDL_MapRGBA(BlitSurface->format,128,128,128,alpha);
	return 0;
}

Uint32 convert4to32BPPcolor(Uint8 c, SDL_Surface *BlitSurface)
{
	// This function converts the colour from the keen palette to 16-bit format
	int alpha;

	alpha = 255;

	if(c == 16)
		return SDL_MapRGBA(BlitSurface->format,1,0,0,alpha);
	else if(c == COLOUR_BLACK)
		return 0;
	else if(c == 1)
		return SDL_MapRGBA(BlitSurface->format,0,0,170,alpha);
	else if(c == 2)
		return SDL_MapRGBA(BlitSurface->format,0,170,0,alpha);
	else if(c == 3)
		return SDL_MapRGBA(BlitSurface->format,0,170,170,alpha);
	else if(c == COLOUR_DARKRED)
		return SDL_MapRGBA(BlitSurface->format,170,0,0,alpha);
	else if(c == 5)
		return SDL_MapRGBA(BlitSurface->format,170,0,170,alpha);
	else if(c == 6)
		return SDL_MapRGBA(BlitSurface->format,170,85,0,alpha);
	else if(c == COLOUR_GREY)
		return SDL_MapRGBA(BlitSurface->format,170,170,170,alpha);
	else if(c == 8)
		return SDL_MapRGBA(BlitSurface->format,85,85,85,alpha);
	else if(c == 9)
		return SDL_MapRGBA(BlitSurface->format,85,85,255,alpha);
	else if(c == 10)
		return SDL_MapRGBA(BlitSurface->format,85,255,85,alpha);
	else if(c == COLOUR_CYAN)
		return SDL_MapRGBA(BlitSurface->format,85,255,255,alpha);
	else if(c == 12)
		return SDL_MapRGBA(BlitSurface->format,255,85,85,alpha);
	else if(c == 13)
		return SDL_MapRGBA(BlitSurface->format,255,85,255,alpha);
	else if(c == 14)
		return SDL_MapRGBA(BlitSurface->format,255,255,85,alpha);
	else if(c == 15)
		return SDL_MapRGBA(BlitSurface->format,255,255,255,alpha);
	else
		return SDL_MapRGBA(BlitSurface->format,128,128,128,alpha);
	return 0;
}

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
