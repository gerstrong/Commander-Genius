/*
 * Geometry.h
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 *  Header files for Geometry and some functions which will help in some calculations
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "../Vector2D.h"

#include <SDL_rect.h>
#include <string>
#include <sstream>
#include <algorithm>

struct SDL_Surface;

/**
 * This structure defines the resolution composed of width height and depth
 */
template <typename T>
struct GsRect
{
    template <typename T2>
    GsRect( GsRect<T2> rect )
    {
        pos.x = rect.pos.x;		pos.y = rect.pos.y;
        dim.x = rect.dim.x;		dim.y = rect.dim.y;
    }


    GsRect( const T lwidth = 0,
           const T lheight = 0 )
        {
            pos.x=0 ; pos.y=0;
            dim.x=lwidth; dim.y=lheight;
        }

    GsRect( const T lx,
           const T ly,
           const T lw,
           const T lh ) :
        pos(lx,ly),
        dim(lw,lh)
        {}

    GsRect( const SDL_Rect &sdlRect ) :
        pos(T(sdlRect.x), T(sdlRect.y)),
        dim(T(sdlRect.w), T(sdlRect.h))
    {}

    GsRect( const std::string &value )
    {
        std::string s = value;
        s.erase( std::remove(s.begin(), s.end(), ','), s.end() );
        s.erase( std::remove(s.begin(), s.end(), 'f'), s.end() );

        std::stringstream ss;

        ss << s;

        ss >> pos.x;
        ss >> pos.y;
        ss >> dim.x;
        ss >> dim.y;
    }

    GsRect<float> asFloat() const
    {
        return GsRect<float>(float(pos.x), float(pos.y),
                             float(dim.x), float(dim.y));
    }


    template <typename T2>
    GsRect operator/(const T2 scale) const
    {
        GsRect rect;
        rect.dim = this->dim / scale;
        rect.pos = this->pos / scale;
        return rect;
    }


    /**
     * @brief empty if rect is empty nothing need to be drawn
     * @return true if empty, otherwise false.
     */
    bool empty() const
    {
        return (dim.y==0 || dim.x==0);
    }

    bool operator==( const GsRect &target )
    {
        return (target.pos.x == pos.x && target.pos.y == pos.y &&
                target.dim.x == dim.x && target.dim.y == dim.y);
    }

    float aspectRatio() const
    {
        return (float(dim.x)/float(dim.y));
    }

    template <typename T2>
    GsRect<T>& operator=( const GsRect<T2> &newRect )
    {
        GsRect<T> retRect;
        pos.x = static_cast<T>(newRect.pos.x);
        pos.y = static_cast<T>(newRect.pos.y);
        dim.x = static_cast<T>(newRect.dim.x);
        dim.y = static_cast<T>(newRect.dim.y);
        return *this;
    }

    SDL_Rect SDLRect() const
    {
        SDL_Rect Rect;
        Rect.x = pos.x;
        Rect.y = pos.y;
        Rect.w = dim.x;
        Rect.h = dim.y;
        return Rect;
    }

    void scale(const int value)
    {
        pos.x *= value;
        pos.y *= value;
        dim.x *= value;
        dim.y *= value;
    }

    void transformInverse(const GsRect &scaleRect)
    {
        dim.x /= scaleRect.dim.x;
        dim.y /= scaleRect.dim.y;
        pos.x -= scaleRect.pos.x;
        pos.x /= scaleRect.dim.x;
        pos.y -= scaleRect.pos.y;
        pos.y /= scaleRect.dim.y;
    }

    GsRect transformInversed(const GsRect &scaleRect)
    {
        GsRect result = *this;
        result.transformInverse(scaleRect);
        return result;
    }

    void transform(const GsRect &scaleRect)
    {
        pos.x *= scaleRect.dim.x;
        pos.y *= scaleRect.dim.y;

        pos += scaleRect.pos;

        dim.x *= scaleRect.dim.x;
        dim.y *= scaleRect.dim.y;
    }

    GsRect transformed(const GsRect &scaleRect) const
    {
        GsRect result;
        result.pos.x  = pos.x + dim.x * scaleRect.pos.x;
        result.pos.y  = pos.y + dim.y * scaleRect.pos.y;
        result.dim.x  = dim.x * scaleRect.dim.x;
        result.dim.y  = dim.y * scaleRect.dim.y;

        return result;
    }



    template <typename T2>
    void transform(const GsRect<T2> &scaleRect)
    {
        GsRect<T> TRect;
        TRect = scaleRect;
        transform(TRect);
    }


    // Check whether a point given by a Vector is with that Rect.
    // The operation is simple but very often used.
    bool _HasPoint(const GsVec2D<T>& Pos) const
    {
        // check X coordinate. is it outside, return false
        if( Pos.x < pos.x || Pos.x >= pos.x+dim.x )
            return false;

        // check Y coordinate. is it outside, return false
        if( Pos.y < pos.y || Pos.y >= pos.y+dim.y )
            return false;

        return true;
    }

    template <typename T2>
    bool HasPoint(const GsVec2D<T2>& Pos) const
    {
        GsVec2D<T> NewPos;
        NewPos.x = static_cast<T>(Pos.x);
        NewPos.y = static_cast<T>(Pos.y);
        return _HasPoint(NewPos);
    }




    void intersect(const GsRect &other)
    {
        if (other.pos.x > pos.x)
        {
            const int firstX2 = pos.x + dim.x;

            if( firstX2 > other.pos.x + other.dim.x )
            {
                dim.x = other.dim.x;
            }
            else
            {
                const int newWidth = firstX2 - other.pos.x;

                if(newWidth < dim.x)
                    dim.x = newWidth;
            }

            pos.x = other.pos.x;
        }
        else
        {
            const int secondX2 = other.pos.x + other.dim.x;
            const int newWidth = secondX2 - pos.x;

            if(newWidth < dim.x)
                dim.x = newWidth;
        }

        if (other.pos.y > pos.y)
        {
            const int firstY2 = pos.y + dim.y;

            if( firstY2 > other.pos.y + other.dim.y )
            {
                dim.y = other.dim.y;
            }
            else
            {
                const int newHeight = firstY2 - other.pos.y;

                if(newHeight < dim.y)
                    dim.y = newHeight;
            }

            pos.y = other.pos.y;
        }
        else
        {
            const int secondY2 = other.pos.y + other.dim.y;
            const int newHeight = secondY2 - pos.y;

            if(newHeight < dim.y)
                dim.y = newHeight;
        }

        if(dim.x < 0)
            dim.x = 0;
        if(dim.y < 0)
            dim.y = 0;
    }

    GsRect clipped(const GsRect &other) const
    {
        auto result = *this;
        result.intersect(other);
        return result;
    }        

    GsVec2D<T> pos, dim;
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
void drawRect(SDL_Surface *sfc,
              SDL_Rect *rect,
              const int thickness,
              const Uint32 &ContourColor,
              const Uint32 &FillColor );






#endif /* GEOMETRY_H_ */
