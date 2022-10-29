
#ifndef GS_SURFACE
#define GS_SURFACE

#include <base/utils/Color.h>
#include <base/utils/Geometry.h>
#include <base/video/scaler/CScaler.h>
#include <memory>

#include <cassert>

#include <SDL_surface.h>

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

    VidFilter mFilter = VidFilter::NONE;

    void setFilter(const VidFilter filter)
    {
        mFilter = filter;
    }

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

    void drawRect(const GsRect<Uint16> &rect,
                  const Uint32 fillColor);

    /**
     * @brief drawRectRounded
     * @param rect
     * @param roundRadius
     * @param contourColor
     * @param fillColor
     * @param borderColor
     */
    void drawRectRounded(const GsRect<Uint16> &rect,
                         const int roundRadius,
                         const Uint32 contourColor,
                         const Uint32 fillColor,
                         const Uint32 borderColor);

    /**
     * @brief drawCircle draws of a given surface a circle
     * @param fillColor  Color to use for filling the circle
     */
    void drawCircle(const Uint32 fillColor);

    /**
     * @brief drawCircle
     * @param pos
     * @param radius
     * @param fillColor
     * @param borderColor
     */
    void drawCircle(const GsVec2D<Uint16> pos,
                    const int radius,
                    const Uint32 fillColor,
                    const Uint32 borderColor);

    /**
     * @brief drawCirclePart
     * @param pos
     * @param radius
     * @param fillColor
     * @param borderColor
     * @param corner 0b00=UpperLeft,
     *               0b01=UpperRight,
     *               0b10=BottomLeft,
     *               0b11=Bottomight,
     */
    void drawCircleQuart(const GsVec2D<Uint16> pos,
                        const int radius,
                        const Uint32 fillColor,
                        const Uint32 borderColor,
                        const char corner);

    /**
     * \brief Draws the countour of the given rect and thickness
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
        assert(mpSurface);
        assert(sfc.mpSurface);

        return SDL_BlitSurface(mpSurface, nullptr, sfc.mpSurface, nullptr);
    }

    int blitTo(GsWeakSurface &sfc, const GsRect<Uint16> &dstRect) const
    {
        assert(mpSurface);
        assert(sfc.mpSurface);
        SDL_Rect sdlRect = dstRect.SDLRect();
        return SDL_BlitSurface( mpSurface, nullptr, sfc.mpSurface, &sdlRect);
    }

    int blitTo(GsWeakSurface &sfc, const SDL_Rect &sdlRect)
    {
        assert(mpSurface);
        assert(sfc.mpSurface);
        return SDL_BlitSurface( mpSurface,
                                nullptr,
                                sfc.mpSurface,
                                const_cast<SDL_Rect*>(&sdlRect) );
    }

    int blitTo(GsWeakSurface &sfc,
               const SDL_Rect &sdlSrcRect, const SDL_Rect &sdlDstRect) const
    {
        assert(mpSurface);
        assert(sfc.mpSurface);
        return SDL_BlitSurface( mpSurface,
                                const_cast<SDL_Rect*>(&sdlSrcRect),
                                sfc.mpSurface,
                                const_cast<SDL_Rect*>(&sdlDstRect) );
    }


    void blitScaledTo(GsWeakSurface &sfc)
    {
        SDL_Surface *dst = sfc.getSDLSurface();
        assert(dst);
        assert(mpSurface);
        blitScaled(mpSurface,
                   mpSurface->clip_rect,
                   dst,
                   dst->clip_rect,
                   VidFilter::NONE);
    }

    void blitScaledTo(GsWeakSurface &sfc,
                      SDL_Rect &sdlSrcRect,
                      SDL_Rect &sdlDstRect) const
    {
        SDL_Surface *dst = sfc.getSDLSurface();
        assert(dst);
        assert(mpSurface);
        blitScaled(mpSurface,
                   sdlSrcRect,
                   dst,
                   sdlDstRect,
                   mFilter);
    }

    void blitScaledToWithFilter(GsWeakSurface &sfc,
                      SDL_Rect &sdlSrcRect,
                      SDL_Rect &sdlDstRect,
                      const VidFilter filter) const
    {
        SDL_Surface *dst = sfc.getSDLSurface();
        assert(dst);
        assert(mpSurface);
        blitScaled(mpSurface,
                   sdlSrcRect,
                   dst,
                   sdlDstRect,
                   filter);
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

    Uint32 mapColorAlpha(const GsColor &color) const
    {
        const Uint8 r = color.r;
        const Uint8 g = color.g;
        const Uint8 b = color.b;
        const Uint8 a = color.a;
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

    GsColor getColorAlpha(const Uint32 colorVar) const
    {
        GsColor color;
        SDL_GetRGBA( colorVar, mpSurface->format,
                     &color.r, &color.g, &color.b, &color.a );
        return color;
    }



    void fillRGB(const Uint8 r, const Uint8 g, const Uint8 b)
    {
        SDL_FillRect( mpSurface, &mpSurface->clip_rect, SDL_MapRGB( mpSurface->format, r, g, b ) );
    }

    void fillRGB(const GsRect<Uint16> &rect, const Uint8 r, const Uint8 g, const Uint8 b)
    {
        fill( rect, mapRGB(r,g,b) );
    }

    void fillRGBA(const GsRect<Uint16> &rect, const GsColor &color)
    {
        fill( rect, mapRGBA(color.r, color.g, color.b, color.a) );
    }

    void fillRGBA(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
    {
        SDL_FillRect( mpSurface, &mpSurface->clip_rect, SDL_MapRGBA( mpSurface->format, r, g, b, a ) );
    }

    void fillRGBA(const GsColor &color)
    {
        fillRGBA( color.r, color.g, color.b, color.a );
    }


    void fillRGBA(const GsRect<Uint16> &rect,
                  const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
    {
        fill( rect, mapRGBA(r,g,b,a) );
    }

    void fill(const GsRect<Uint16> &rect,
              const Uint32 color)
    {
        SDL_Rect sdlRect = rect.SDLRect();
        SDL_FillRect( mpSurface, &sdlRect, color );
    }

    void fill(const GsRect<Uint16> &rect,
              const GsColor &color)
    {
        fill( rect, color.get(mpSurface->format) );
    }


    void fill(const Uint32 color)
    {
        SDL_FillRect( mpSurface, nullptr, color );
    }

    void setColorMask(const unsigned char r,
                      const unsigned char g,
                      const unsigned char b);

    void setPaletteColors(void *Palette, const int numColors);

    void setColorKey(unsigned int key);

    void setColorKey(const Uint8 r, const Uint8 g, const Uint8 b);

    void readColorKey(Uint8 &r,
                      Uint8 &g,
                      Uint8 &b);


    Uint8 getAlpha();

    void setBlendMode(const int mode);

    int getBlendMode() const;

    /**
     * @brief setAlpha  Sets per Surface alpha
     * @param alpha value of alpha to set
     */
    void setAlpha(const unsigned char alpha);

    void applyDisplayFormat();

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
        if(SDL_MUSTLOCK(mpSurface))
        {
            SDL_LockSurface(mpSurface);
        }
    }

    /**
     * @brief unlocks the Surface from direct pixel access
     */
    void unlock()
    {
        if(SDL_MUSTLOCK(mpSurface))
        {
            SDL_UnlockSurface(mpSurface);
        }
    }

    Uint8 *PixelPtr() const
    {
        return static_cast<Uint8*>(mpSurface->pixels);
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

    void tintColor(const GsColor &gsColor);



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
     * @brief operator = will pass the an existing sfc to this one,
     *        the other one gets emptied in this process
     * @param sfc
     */
    GsSurface& operator=(GsSurface &sfc) = delete;

    void tryToDestroy()
    {
        if(mpSurface != nullptr)
        {
            SDL_FreeSurface(mpSurface);
            mpSurface = nullptr;
        }
    }


    ~GsSurface()
    {
        tryToDestroy();
    }


    // Use this, if you don't need to access the surface and another process
    // will free it. If you used setVideoMode(...)
    // please call this before you call SDL_Quit and exit the program.
    void disownSfc()
    {
        mpSurface = nullptr;
    }

    void create(Uint32 flags, int width, int height, int depth,
                Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

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
     * @brief loadBmp Load a bmp image file
     * @param filepath  path the file to load
     * @return true if everything went fine, otherwise false
     */
    bool loadBmp(const std::string filepath);

    bool loadImgInternal(const unsigned char *data,
                         const std::string &name,
                         const int size);

    /**
     * @brief loadImg  Load a picture file using SDL_image
     * @param filepath  path the file to load
     * @return true if everything went fine, otherwise false
     */
    bool loadImg(const std::string filepath);

    /**
     * @brief createFromSDLSfc      Call this if you want to use another
     *                              SDL_Surface creation function which will
     *                              be managed by this class. Here we supposse,
     *                              that you already allocated the SDL Surface
     *                              by yourself and the class destroys it by calling
     *                              SDL_Free Surface
     * @param sfc   pointer to the surface created.
     */
    bool createFromSDLSfc(SDL_Surface *sfc)
    {
        tryToDestroy();

        mpSurface = sfc;

        if(mpSurface)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * @brief tiltSurface rotate by 90 degree or tilt
     */
    void tiltSurface();

    /**
     * @brief mirrorSurfaceHoriz mirrors the surface
     *        at horizontal axis
     */
    void mirrorSurfaceHoriz();


    /**
     * @brief createCopy will create a copy of another surface
     * @param orig Orignal surface to copy
     */
    void createCopy(const GsWeakSurface &orig)
    {
        assert(orig);

        SDL_Surface *sdlSfc = orig.getSDLSurface();
        SDL_PixelFormat *format = sdlSfc->format;

        tryToDestroy();

        mpSurface = SDL_ConvertSurface(sdlSfc,
                                       format,
                                       sdlSfc->flags);
    }


    /// Scale routines

    bool requiresScaling(const GsRect<Uint16> &scaledRect);

    /**
     * @brief scaleTo will scale to rect to the given newRect Size
     * @param scaledRect x and y coordinates are ignored. Only w and h will be taken for scaling
     * @return true, if scaling was performed, otherwise false
     */
    bool scaleTo(const GsRect<Uint16> &scaledRect, const VidFilter filter);
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
