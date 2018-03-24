/*
 * Geometry.cpp
 *
 *  Created on: 25.01.2012
 *      Author: gerstrong
 */

#include "Geometry.h"



void drawRect( SDL_Surface *sfc,
			   SDL_Rect *rect,
			   const int thickness,
			   const Uint32 &ContourColor )
{
	SDL_Rect TempRect;

	// Get the proper Rect for drawing
	if(rect)
	{
		TempRect = *rect;
	}
	else
	{
		TempRect.x = 0;
		TempRect.y = 0;
		TempRect.w = sfc->w;
		TempRect.h = sfc->h;
	}

	SDL_Rect tempr;

	// Upper side contour
	tempr.x = TempRect.x;
	tempr.y = TempRect.y;
	tempr.w = TempRect.w;
	tempr.h = thickness;
	SDL_FillRect(sfc, &tempr, ContourColor );

	// Left side contour
	tempr.y = TempRect.y + thickness;
	tempr.w = thickness;
	tempr.h = TempRect.h - (2 * thickness);
	SDL_FillRect(sfc, &tempr, ContourColor);

	// Right side contour
	tempr.x = TempRect.x + TempRect.w - thickness;
	SDL_FillRect(sfc, &tempr, ContourColor);


	// Lower side contour
	tempr.x = TempRect.x;
	tempr.y = TempRect.y + TempRect.h - thickness;
	tempr.w = TempRect.w;
	tempr.h = thickness;
	SDL_FillRect(sfc, &tempr, ContourColor);

}


void drawRect( SDL_Surface *sfc,
			   SDL_Rect *rect,
			   const int thickness,
			   const Uint32 &ContourColor,
			   const Uint32 &FillColor )
{

	drawRect( sfc, rect, thickness, ContourColor );

	SDL_Rect TempRect;

	// Get the proper Rect for drawing
	if(rect)
	{
		TempRect = *rect;
	}
	else
	{
		TempRect.x = 0;
		TempRect.y = 0;
		TempRect.w = sfc->w;
		TempRect.h = sfc->h;
	}

	TempRect.x += thickness;
	TempRect.y += thickness;
	TempRect.w -= (2*thickness);
	TempRect.h -= (2*thickness);


	SDL_FillRect(sfc, &TempRect, FillColor);

}



