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
