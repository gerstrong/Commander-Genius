
#include <base/utils/Geometry.h>
#include <base/video/CVideoDriver.h>
#include <base/GsLogging.h>

#include "GsSurface.h"
#include <cstring>


Uint32 GsWeakSurface::getPixel(const int x, const int y)
{
    SDL_LockSurface(mpSurface);

    int bpp = mpSurface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)mpSurface->pixels + y * mpSurface->pitch + x * bpp;
    Uint32 pixColor = 0;

    switch(bpp)
    {
    case 1:
        pixColor = *p;
        break;

    case 2:
        memcpy(&pixColor, p, sizeof(Uint16));
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        memcpy(&pixColor, p, sizeof(Uint32));
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }

    SDL_UnlockSurface(mpSurface);

    return pixColor;
}


GsRect<Uint16> GsWeakSurface::calcBlitRect(const GsRect<float> &rect)
{
    GsRect<Uint16> absRect(0, 0, mpSurface->w, mpSurface->h);

    absRect.transform(rect);
    return absRect;
}


void GsWeakSurface::drawRect(const GsRect<Uint16> &rect,
                         const int thickness,
                         const Uint32 contourColor )
{
    GsRect<Uint16> contRect = rect;

    // Upper side contour
    contRect.h = thickness;
    fill(contRect, contourColor);

    // Left side contour, if possible
    if(rect.h <= 2*thickness)
        return;

    contRect.y = rect.y + thickness;
    contRect.w = thickness;            
    contRect.h = rect.h - (2 * thickness);
    fill(contRect, contourColor);

    // Right side contour, if possible
    if(rect.w <= thickness)
        return;

    contRect.x = rect.x + rect.w - thickness;
    fill(contRect, contourColor);

    // And last but not least the lower side contour
    if(rect.h <= thickness)
        return;

    contRect.x = rect.x;
    contRect.y = rect.y + rect.h - thickness;
    contRect.w = rect.w;
    contRect.h = thickness;
    fill(contRect, contourColor);

}



void GsWeakSurface::drawFrameRect(const GsRect<Uint16> &rect,
              const int thickness,
              const Uint32 contourColor)
{
    // Create the proper rect for all this
    GsRect<Uint16> fillRect = rect;

    drawRect( fillRect, thickness, contourColor );

}


void GsWeakSurface::drawRect(const GsRect<Uint16> &rect,
                         const int thickness,
                         const Uint32 contourColor,
                         const Uint32 fillColor )
{
    // Create the proper rect for all this
    GsRect<Uint16> fillRect = rect;

    drawFrameRect(rect, thickness, contourColor);

    fillRect.x += thickness;
    fillRect.y += thickness;
    fillRect.w -= (2*thickness);
    fillRect.h -= (2*thickness);

    fill(fillRect, fillColor);
}


void GsSurface::createRGBSurface( const SDL_Rect &rect )
{
    auto *blit = gVideoDriver.getBlitSurface();
    SDL_PixelFormat *format = blit->format;

    create( SDL_SWSURFACE, rect.w, rect.h,
            RES_BPP,
            format->Rmask,
            format->Gmask,
            format->Bmask,
            format->Amask);
}

void GsSurface::makeBlitCompatible()
{
    // Do not call this if the surface is invalid
    assert(mpSurface != nullptr);

    auto compatibleSfc = gVideoDriver.convertThroughBlitSfc( mpSurface );

    SDL_FreeSurface(mpSurface);

    mpSurface = compatibleSfc;
}

bool GsSurface::loadImg(const std::string filepath)
{
    auto tempSurface = IMG_Load(filepath.c_str());

    if (!tempSurface)
    {
        return false;
    }

    if (!createFromSDLSfc(tempSurface))
    {
        return false;
    }

    return true;
}

bool GsSurface::loadBmp(const std::string filepath)
{
    auto tempSurface = SDL_LoadBMP(filepath.c_str());

    if (!tempSurface)
    {
        gLogging << "GsSurface::loadBmp failed: " << SDL_GetError();
        return false;
    }

    if (!createFromSDLSfc(tempSurface))
    {
        return false;
    }

    return true;
}

/**
 * @brief tiltSurface rotate by 90 degree or tilt
 * @param orig
 */
void GsSurface::tiltSurface()
{
    GsSurface src;

    src.createCopy(*this);

    auto format = mpSurface->format;

    create(mpSurface->flags,
           mpSurface->h, mpSurface->w,
           format->BitsPerPixel,
           format->Rmask,
           format->Gmask,
           format->Bmask,
           format->Amask);


    src.lock();
    lock();

    SDL_Surface *srcSfc = src.getSDLSurface();
    Uint8 *srcPxY = static_cast<Uint8*>(srcSfc->pixels);
    Uint8 *dstPxY = static_cast<Uint8*>(mpSurface->pixels);

    for(int y = 0 ; y<mpSurface->h ; y++ )
    {
        Uint8 *dstPx = dstPxY;

        for(int x = 0 ; x<mpSurface->w ; x++ )
        {
            const int bpp = mpSurface->format->BytesPerPixel;

            Uint8 *srcPx = srcPxY +
                           x*(srcSfc->pitch) +
                           y*(srcSfc->format->BytesPerPixel);

            memcpy(dstPx, srcPx, bpp);

            dstPx += bpp;
        }

        dstPxY += mpSurface->pitch;
    }

    unlock();
    src.unlock();
}


bool GsSurface::scaleTo(const GsRect<Uint16> &scaledRect, const filterOptionType filter)
{
    SDL_Rect newRect = scaledRect.SDLRect();

    if(newRect.w == mpSurface->w && newRect.h == mpSurface->h)
    {
        return true;
    }

    // Need to do that, otherwise it might not work in some cases
    makeBlitCompatible();

    auto sfcFormat = mpSurface->format;

    SDL_Surface *newSfc =
            SDL_CreateRGBSurface(mpSurface->flags,
                                 newRect.w, newRect.h,
                                 sfcFormat->BitsPerPixel,
                                 sfcFormat->Rmask,
                                 sfcFormat->Gmask,
                                 sfcFormat->Bmask,
                                 sfcFormat->Amask  );

    if(!newSfc)
    {
      return false;
    }

#if SDL_VERSION_ATLEAST(2, 0, 0)

    SDL_BlendMode blendMode;

    SDL_GetSurfaceBlendMode(mpSurface, &blendMode);
    SDL_SetSurfaceBlendMode(newSfc, blendMode);

#endif

    blitScaled(mpSurface,
               mpSurface->clip_rect,
               newSfc,
               newRect,
               filter);

    // Tear down old surface!
    SDL_FreeSurface(mpSurface);

    // And set the newly created and scaled one
    mpSurface = newSfc;

    return true;
}


void
GsWeakSurface::
setAlpha(const unsigned char alpha)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceBlendMode( mpSurface, SDL_BLENDMODE_BLEND );
    SDL_SetSurfaceAlphaMod( mpSurface, alpha);
#else
    //SDL_SetAlpha(mpSurface, SDL_SRCALPHA, alpha);
    SDL_SetAlpha(mpSurface, SDL_SRCALPHA, alpha);

    // Do not call this if the surface is invalid
    /*assert(mpSurface != nullptr);

    auto compatibleSfc = SDL_DisplayFormat( mpSurface );

    SDL_FreeSurface(mpSurface);

    mpSurface = compatibleSfc;*/

#endif
}

void
GsWeakSurface::
setColorKey(unsigned int key)
{
    #if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetColorKey(mpSurface, SDL_TRUE, key);
    #else
        SDL_SetColorKey(mpSurface, SDL_SRCCOLORKEY, key);
    #endif
}

void
GsWeakSurface::
setColorKey(const Uint8 r, const Uint8 g, const Uint8 b)
{
    auto colorkey = SDL_MapRGB( mpSurface->format, r, g, b );

    #if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetColorKey(mpSurface, SDL_TRUE, colorkey);
    #else
        SDL_SetColorKey(mpSurface, SDL_SRCCOLORKEY, colorkey);
    #endif
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
void GsWeakSurface::
readColorKey(Uint8 &,
             Uint8 &,
             Uint8 &)
{
    // Does not work in later versions of SDL.
}
#else
void GsWeakSurface::
readColorKey(Uint8 &r,
             Uint8 &g,
             Uint8 &b)
{
    SDL_GetRGB( mpSurface->format->colorkey,  mpSurface->format, &r, &g, &b);
}
#endif



void
GsWeakSurface::
applyDisplayFormat()
{
    // NOTE: For what I have observed, only version older than 2.0
    //       require this to be called.

#if SDL_VERSION_ATLEAST(2, 0, 0)
    // Nothing to do here!
#else
    // Do not call this if the surface is invalid
    assert(mpSurface != nullptr);

    auto compatibleSfc = SDL_DisplayFormat( mpSurface );

    SDL_FreeSurface(mpSurface);

    mpSurface = compatibleSfc;
#endif
}

