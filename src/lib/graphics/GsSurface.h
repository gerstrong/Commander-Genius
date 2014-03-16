
#ifndef __GS_SURFACE__
#define __GS_SURFACE__

#include <base/video/scaler/CScaler.h>
#include <base/utils/Geometry.h>
#include <memory>


class GsWeakSurface
{
public:

    GsWeakSurface() :
        mpSurface(nullptr) {}

    GsWeakSurface(SDL_Surface *sfc) :
        mpSurface(sfc) {}

    /**
     * \brief Draws rect different than the SDL_Fillrect, because it has a contour and is filled
     * \param sfc Surface where to draw it
     * \param rect pointer to the given rect in which it has to be drawn. If it's NULL, than the dimensions
     * 			   of sfc is used
     * \param thickness Thickness of the contour
     * \param ContourColor Color of the contour
     */
    void drawRect(const GsRect<Uint16> &rect,
                  const int thickness,
                  const Uint32 contourColor );


    /**
     * \brief Draws rect different than the SDL_Fillrect, because it has a contour and is filled
     * \param sfc Surface where to draw it
     * \param rect pointer to the given rect in which it has to be drawn. If it's NULL, than the dimensions
     * 			   of sfc is used
     * \param thickness Thickness of the contour
     * \param ContourColor Color of the contour
     * \param FillColor Fill-color of the rect
     */
    void drawRect(const GsRect<Uint16> &rect,
                  const int thickness,
                  const Uint32 contourColor,
                  const Uint32 fillColor );


    /**
     * \brief Draws rect different than the SDL_Fillrect, because it has only a contour
     * \param sfc Surface where to draw it
     * \param rect pointer to the given rect in which it has to be drawn. If it's NULL, than the dimensions
     * 			   of sfc is used
     * \param thickness Thickness of the contour
     * \param ContourColor Color of the contour
     */
    void drawFrameRect(const GsRect<Uint16> &rect,
                  const int thickness,
                  const Uint32 contourColor);


    GsRect<Uint16> calcBlitRect(const GsRect<float> &rect);

    int blitTo(GsWeakSurface &sfc)
    {
        return SDL_BlitSurface( mpSurface, nullptr, sfc.mpSurface, nullptr );
    }

    int blitTo(GsWeakSurface &sfc, GsRect<Uint16> &dstRect)
    {
        SDL_Rect sdlRect = dstRect.SDLRect();
        return SDL_BlitSurface( mpSurface, nullptr, sfc.mpSurface, &sdlRect );
    }

    int blitTo(GsWeakSurface &sfc, const SDL_Rect &sdlRect)
    {
        return SDL_BlitSurface( mpSurface, nullptr, sfc.mpSurface, const_cast<SDL_Rect*>(&sdlRect) );
    }

    Uint32 mapRGB(const Uint8 r, const Uint8 g, const Uint8 b)
    {
        return SDL_MapRGB( mpSurface->format, r, g, b );
    }

    Uint32 mapRGBA(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
    {
        return SDL_MapRGBA( mpSurface->format, r, g, b, a );
    }


    void fillRGB(const Uint8 r, const Uint8 g, const Uint8 b)
    {
        SDL_FillRect( mpSurface, &mpSurface->clip_rect, SDL_MapRGB( mpSurface->format, r, g, b ) );
    }

    void fillRGB(const GsRect<Uint16> &rect, const Uint8 r, const Uint8 g, const Uint8 b)
    {
        fill( rect, mapRGB(r,g,b) );
    }

    void fillRGBA(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
    {
        SDL_FillRect( mpSurface, &mpSurface->clip_rect, SDL_MapRGBA( mpSurface->format, r, g, b, a ) );
    }

    void fillRGBA(const GsRect<Uint16> &rect, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
    {
        fill( rect, mapRGBA(r,g,b,a) );
    }

    void fill(const GsRect<Uint16> &rect, const Uint32 color)
    {
        SDL_Rect sdlRect = rect.SDLRect();
        SDL_FillRect( mpSurface, &sdlRect, color );
    }

    void fill(const Uint32 color)
    {
        SDL_FillRect( mpSurface, nullptr, color );
    }

    void setColorMask(const unsigned char r,
                      const unsigned char g,
                      const unsigned char b)
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetColorKey(mpSurface, SDL_TRUE, mapRGB(r, g, b));
    SDL_SetSurfaceBlendMode(mpSurface, SDL_BLENDMODE_BLEND);
#else
    SDL_SetColorKey( mpSurface, SDL_SRCCOLORKEY, mapRGB(r, g, b) );
#endif
    }

    void setAlpha(const unsigned char alpha)
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceAlphaMod( mpSurface, alpha);
#else
    SDL_SetAlpha(mpSurface, SDL_SRCALPHA, alpha);
#endif
    }

    Uint16 width() const
    {   return mpSurface->w;   }

    Uint16 height() const
    {   return mpSurface->h;   }

    // bool operator
    operator bool() const
    {
        return (mpSurface!=nullptr);
    }

    SDL_Surface *getSDLSurface()
    {
        return mpSurface;
    }


protected:

    SDL_Surface *mpSurface;
};

// Normal Surface

class GsSurface : public GsWeakSurface
{
public:

    GsSurface() : GsWeakSurface() {}

    GsSurface(SDL_Surface *sfc) :
        GsWeakSurface(sfc) {}

    GsSurface(GsSurface *sfc) :
        GsWeakSurface(sfc->getSDLSurface())
    {
        sfc->disownSfc();
    }


    ~GsSurface()
    {
        if(mpSurface != nullptr)
            SDL_FreeSurface(mpSurface);
    }


    // Use this, if you don't need to access the surface and another process
    // will free it. If you used setVideoMode(...)
    // please call this before you call SDL_Quit and exit the program.
    void disownSfc()
    {
        mpSurface = nullptr;
    }

    void create(Uint32 flags, int width, int height, int depth,
                Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
    {
        if(mpSurface)
            SDL_FreeSurface(mpSurface);

        mpSurface = SDL_CreateRGBSurface(flags, width, height, depth,
                                         Rmask, Gmask, Bmask, Amask);
    }



    /**
     * @brief createCopy will create a copy of another surface
     * @param orig Orignal surface to copy
     */
    void createCopy(GsWeakSurface &orig)
    {
        SDL_Surface *sdlSfc = orig.getSDLSurface();
        SDL_PixelFormat *format = sdlSfc->format;
        create(sdlSfc->flags,
               sdlSfc->w,
               sdlSfc->h,
               format->BitsPerPixel,
               format->Rmask,
               format->Gmask,
               format->Bmask,
               format->Amask);

        orig.blitTo(*this);
    }

    /// Scale routines
    /**
     * @brief scaleTo will scale to rect to the given newRect Size
     * @param scaledRect x and y coordinates are ignored. Only w and h will be taken for scaling
     * @return true, if scaling was performed, otherwise false
     */
    bool scaleTo(const GsRect<Uint16> &scaledRect, const filterOptionType filter);

};


class GsVideoSurface : public GsWeakSurface
{

public:

    // Wrapper for SDL_SetVideoMode
    int setVideoMode(const int width,
                 const int height,
                 const int bpp,
                 const Uint32 flags)
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
        mpSurface = SDL_SetVideoMode(width, height, bpp, flags);
#endif
        return 0;
    }

    // Wrapper for SDL_Flip
    bool flip()
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        return true;
#else
        return (SDL_Flip(mpSurface)!=-1);
#endif
    }

};

#endif // __GS_SURFACE__
