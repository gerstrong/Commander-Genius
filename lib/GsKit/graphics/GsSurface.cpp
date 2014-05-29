
#include <base/utils/Geometry.h>

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




bool GsSurface::scaleTo(const GsRect<Uint16> &scaledRect, const filterOptionType filter)
{
    SDL_Rect newRect = scaledRect.SDLRect();

    if(newRect.w == mpSurface->w && newRect.h == mpSurface->h)
        return true;


    // Need to do that, otherwise it won't work.
    //optimizeSurface();

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
      return false;

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
