/*
 * Geometry.h
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *  Header files for Geometry and some functions which will help in some calculations
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <SDL.h>

/**
 * This structure defines the resolution composed of width height and depth
 */
template <typename T>
struct CRect : public SDL_Rect
{
	CRect( const T lwidth = 0,
		   const T lheight = 0 )
		{ w=lwidth; h=lheight; }

	CRect( const T lx,
		   const T ly,
		   const T lw,
		   const T lh )
		{ x=lx; y=ly; w=lw; h=lh; }

	CRect( const SDL_VideoInfo* InfoPtr )
		{ w=InfoPtr->current_w; h=InfoPtr->current_w; };

	bool operator==( const CRect &target )
	{
		return (target.x == x && target.y == y &&
				target.w == w && target.h == h);
	}

	float aspectRatio() const
	{
		return (float(w)/float(h));
	}

	SDL_Rect SDLRect()
	{
		SDL_Rect Rect;
		Rect.x = x;
		Rect.y = y;
		Rect.w = w;
		Rect.h = h;
		return Rect;
	}

	T x, y, w, h;
};

#endif /* GEOMETRY_H_ */
