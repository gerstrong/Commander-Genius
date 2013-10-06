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
#include <CVec.h>

/**
 * This structure defines the resolution composed of width height and depth
 */
template <typename T>
struct CRect
{
	template <typename T2>
	CRect( CRect<T2> rect )
	{
		x = rect.x;		y = rect.y;
		w = rect.w;		h = rect.h;
	}


	CRect( const T lwidth = 0,
		   const T lheight = 0 )
		{ w=lwidth; h=lheight; }

	CRect( const T lx,
		   const T ly,
		   const T lw,
		   const T lh )
		{ x=lx; y=ly; w=lw; h=lh; }

#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
    CRect( const SDL_VideoInfo* InfoPtr )
		{ w=InfoPtr->current_w; h=InfoPtr->current_h; };
#endif

	bool operator==( const CRect &target )
	{
		return (target.x == x && target.y == y &&
				target.w == w && target.h == h);
	}

	float aspectRatio() const
	{
		return (float(w)/float(h));
	}

	template <typename T2>
	CRect<T>& operator=( const CRect<T2> &newRect )
	{
		CRect<T> retRect;
		x = static_cast<T>(newRect.x);
		y = static_cast<T>(newRect.y);
		w = static_cast<T>(newRect.w);
		h = static_cast<T>(newRect.h);
		return *this;
	}

	SDL_Rect SDLRect() const
	{
		SDL_Rect Rect;
		Rect.x = x;
		Rect.y = y;
		Rect.w = w;
		Rect.h = h;
		return Rect;
	}

	void transformInverse(const CRect &scaleRect)
	{
		x /= scaleRect.w;
		x -= scaleRect.x;
		y /= scaleRect.h;
		y -= scaleRect.y;
		w /= scaleRect.w;
		h /= scaleRect.h;
	}

	void transform(const CRect &scaleRect)
	{
		x *= scaleRect.w;
		x += scaleRect.x;
		y *= scaleRect.h;
		y += scaleRect.y;
		w *= scaleRect.w;
		h *= scaleRect.h;
	}


	template <typename T2>
	void transform(const CRect<T2> &scaleRect)
	{
		CRect<T> TRect;
		TRect = scaleRect;
		transform(TRect);
	}


	// Check whether a point given by a Vector is with that Rect.
	// The operation is simple but very often used.
	bool HasPoint(VectorD2<T>& Pos)
	{
		// check X coordinate. is it outside, return false
		if( Pos.x < x || Pos.x > x+w )
			return false;

		// check Y coordinate. is it outside, return false
		if( Pos.y < y || Pos.y > y+h )
			return false;

		return true;
	}

	template <typename T2>
	bool HasPoint(VectorD2<T2>& Pos)
	{
		VectorD2<T> NewPos;
		NewPos.x = static_cast<T>(Pos.x);
		NewPos.y = static_cast<T>(Pos.y);
		return HasPoint(NewPos);
	}


    void intersect(const CRect &other)
    {
        if (other.x > x)
        {
            const int firstX2 = x + w;

            if( firstX2 > other.x + other.w )
            {
                w = other.w;
            }
            else
            {
                const int newWidth = firstX2 - other.x;

                if(newWidth < w)
                    w = newWidth;
            }

            x = other.x;
        }
        else
        {
            const int secondX2 = other.x + other.w;
            const int newWidth = secondX2 - x;

            if(newWidth < w)
                w = newWidth;
        }

        if (other.y > y)
        {
            const int firstY2 = y + h;

            if( firstY2 > other.y + other.h )
            {
                h = other.h;
            }
            else
            {
                const int newHeight = firstY2 - other.y;

                if(newHeight < h)
                    h = newHeight;
            }

            y = other.y;
        }
        else
        {
            const int secondY2 = other.y + other.h;
            const int newHeight = secondY2 - y;

            if(newHeight < h)
                h = newHeight;
        }
    }


	T x, y, w, h;
};



/**
 * \brief Draws rect different than the SDL_Fillrect, because it has a contour and is filled
 * \param sfc Surface where to draw it
 * \param rect pointer to the given rect in which it has to be drawn. If it's NULL, than the dimensions
 * 			   of sfc is used
 * \param thickness Thickness of the contour
 * \param ContourColor Color of the contour
 */
void drawRect( SDL_Surface *sfc,
			   SDL_Rect *rect,
			   const int thickness,
			   const Uint32 &ContourColor );

/**
 * \brief Draws rect different than the SDL_Fillrect, because it has a contour and is filled
 * \param sfc Surface where to draw it
 * \param rect pointer to the given rect in which it has to be drawn. If it's NULL, than the dimensions
 * 			   of sfc is used
 * \param thickness Thickness of the contour
 * \param ContourColor Color of the contour
 * \param FillColor Fill-color of the rect
 */
void drawRect( SDL_Surface *sfc,
			   SDL_Rect *rect,
			   const int thickness,
			   const Uint32 &ContourColor,
			   const Uint32 &FillColor );






#endif /* GEOMETRY_H_ */
