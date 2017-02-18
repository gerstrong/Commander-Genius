
#ifndef GS_SURFACE
#define GS_SURFACE

//#include <base/video/scaler/CScaler.h>
#include <base/utils/Geometry.h>
#include <base/video/scaler/CScaler.h>
#include <memory>

#include <cassert>

static inline int BlitSurface(SDL_Surface *src, SDL_Rect *srcrect,
                       SDL_Surface *dst, SDL_Rect *dstrect)
{
    assert(src);    assert(dst);
    return SDL_BlitSurface(src, srcrect, dst, dstrect);
}




class GsWeakSurface
{
public:

    GsWeakSurface() :
        mpSurface(nullptr) {}

    GsWeakSurface(SDL_Surface *sfc) :
        mpSurface(sfc) {}

    /**
     * @brief setPtr    Sets the pointer of the surface.
     *                  This should only be used, if you set the Video Mode through SDL_VideoMode
     *                  it manages the allocation by itself. So Weak will not destroy it.
     * @param pSfc
     */
    void setPtr(SDL_Surface* pSfc)
    {
        mpSurface = pSfc;
    }

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


    /**SDLKey
     * @brief getPixel given the coordinate it will return the pixel color as Uint32.
     * @param x
     * @param y
     * @return Pixel which still needs conversion through SDL_MapRGB;
     */
    Uint32 getPixel(const int x, const int y);

    GsRect<Uint16> calcBlitRect(const GsRect<float> &rect);

    int blitTo(GsWeakSurface &sfc) const
    {
        assert(sfc.mpSurface);
        return BlitSurface( mpSurface, nullptr, sfc.mpSurface, nullptr );
    }

    int blitTo(GsWeakSurface &sfc, GsRect<Uint16> &dstRect)
    {
        assert(mpSurface);
        assert(sfc.mpSurface);
        SDL_Rect sdlRect = dstRect.SDLRect();
        return BlitSurface( mpSurface, nullptr, sfc.mpSurface, &sdlRect );
    }

    int blitTo(GsWeakSurface &sfc, const SDL_Rect &sdlRect)
    {
        assert(mpSurface);
        assert(sfc.mpSurface);
        return BlitSurface( mpSurface, nullptr, sfc.mpSurface, const_cast<SDL_Rect*>(&sdlRect) );
    }


    void blitScaledTo(GsWeakSurface &sfc)
    {        
        SDL_Surface *dst = sfc.getSDLSurface();
        assert(dst);
        blitScaled(mpSurface,
                   mpSurface->clip_rect,
                   dst,
                   dst->clip_rect,
                   NONE);
    }
    // TODO: We still need a blit scaled operation here!


    Uint32 mapRGB(const Uint8 r, const Uint8 g, const Uint8 b)
    {
        return SDL_MapRGB( mpSurface->format, r, g, b );
    }

    Uint32 mapRGBA(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
    {
        return SDL_MapRGBA( mpSurface->format, r, g, b, a );
    }

    void getRGB(const Uint32 color, Uint8 &r, Uint8 &g, Uint8 &b)
    {
        SDL_GetRGB( color, mpSurface->format, &r, &g, &b );
    }

    void getRGBA(const Uint32 color, Uint8 &r, Uint8 &g, Uint8 &b, Uint8 &a)
    {
        SDL_GetRGBA( color, mpSurface->format, &r, &g, &b, &a );
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

#if SDL_VERSION_ATLEAST(2, 0, 0)
    void setBlendMode(const SDL_BlendMode mode)
    {
        SDL_SetSurfaceBlendMode( mpSurface, mode );
    }
#endif


    void setPaletteColors(SDL_Color *Palette)
    {
        #if SDL_VERSION_ATLEAST(2, 0, 0)
            SDL_SetPaletteColors(mpSurface->format->palette, Palette, 0, 255);
        #else
            SDL_SetColors(mpSurface, Palette, 0, 255);
        #endif
    }

    void setColorKey(unsigned int key)
    {
        #if SDL_VERSION_ATLEAST(2, 0, 0)
            SDL_SetColorKey(mpSurface, SDL_TRUE, key);
        #else
            SDL_SetColorKey(mpSurface, SDL_SRCCOLORKEY, key);
        #endif
    }

    void setColorKey(const Uint8 r, const Uint8 g, const Uint8 b)
    {
        auto colorkey = SDL_MapRGB( mpSurface->format, r, g, b );

        #if SDL_VERSION_ATLEAST(2, 0, 0)
            SDL_SetColorKey(mpSurface, SDL_TRUE, colorkey);
        #else
            SDL_SetColorKey(mpSurface, SDL_SRCCOLORKEY, colorkey);
        #endif
    }


    Uint8 getAlpha()
    {
    #if SDL_VERSION_ATLEAST(2, 0, 0)
        Uint8 alpha = 0;
        SDL_GetSurfaceAlphaMod(mpSurface, &alpha);
        return alpha;
    #else
        return mpSurface->format->alpha;
    #endif
    }

    void setAlpha(const unsigned char alpha)
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceBlendMode( mpSurface, SDL_BLENDMODE_BLEND );
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

    SDL_Surface *getSDLSurface() const
    {
        return mpSurface;
    }

    /**
     * @brief empty tells if the surface exists or nothing
     *              is alloced through create or a previously passed pointer
     * @return true if no surface is created, otherwise false
     */
    bool empty()
    {
        return (mpSurface==nullptr);
    }

    /**
     * @brief locks the Surface for direct pixel access
     */
    void lock()
    {
        SDL_LockSurface(mpSurface);
    }

    /**
     * @brief unlocks the Surface from direct pixel access
     */
    void unlock()
    {
        SDL_UnlockSurface(mpSurface);
    }


    /**
     * @brief flip Flips surface like SDL is supposed to do so...
     */
    void flip()
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
        SDL_Flip(mpSurface);
#endif
    }


protected:

    SDL_Surface *mpSurface = nullptr;
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

    /**
     * @brief operator = will set the pass the an existing sfc to this one,
     *        the other one gets emptied in this process
     * @param sfc
     */
    void operator=(GsSurface &sfc)
    {
        mpSurface = sfc.getSDLSurface();
        sfc.disownSfc();
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
        {
            SDL_FreeSurface(mpSurface);
        }

        mpSurface = SDL_CreateRGBSurface(flags, width, height, depth,
                                         Rmask, Gmask, Bmask, Amask);
    }

    /**
     * @brief createRGBSurface  Just create a simple RGB Surface
     * @param rect  Rect for the new surface
     */
    void createRGBSurface( const SDL_Rect &rect );


    /**
     * @brief makeBlitCompatible    Ensure the given surface will be compatible to what it blitted
     */
    void makeBlitCompatible();

    /**
     * @brief createFromSDLSfc      Call this if you want to use another SDL_Surface creation function which returns
     *                              to be managed by this class
     * @param sfc   pointer to the surface created.
     */
    bool createFromSDLSfc(SDL_Surface *sfc)
    {
        if(mpSurface)
        {
            SDL_FreeSurface(mpSurface);
        }

        mpSurface = sfc;

        if(mpSurface)
            return true;
        else
            return false;
    }


    /**
     * @brief createCopy will create a copy of another surface
     * @param orig Orignal surface to copy
     */
    void createCopy(const GsWeakSurface &orig)
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

#endif // GS_SURFACE
