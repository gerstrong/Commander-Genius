#include "GsBitmapButton.h"

#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>
#include <graphics/GsBitmap.h>

GsBitmapButton::GsBitmapButton(const std::string& text,
               const GsRect<float> &rect,
               CEvent *ev,
               const int fontId,
               const GsColor &color) :
GsButton(text, rect, ev, fontId, color)
{}


bool GsBitmapButton::loadBgBitmap(const std::string &filepath)
{
    mpBitmap.reset(new GsBitmap);
    auto ok = mpBitmap->loadImg(filepath);

    if(ok)
        return true;

    gLogging.ftextOut("Unable to load \"%s\"<br>", filepath.c_str());

    mpBitmap = nullptr;
    return false;
}

bool GsBitmapButton::loadBgBitmapFromMem(const unsigned char *data,
                                   const std::string &name,
                                   const int size)
{
    mpBitmap.reset(new GsBitmap);
    auto ok = mpBitmap->loadImgFromMem(data, name, size);

    if(ok)
        return true;

    gLogging.ftextOut("Unable to load \"%s\"<br>", name.c_str());

    mpBitmap = nullptr;
    return false;
}

void GsBitmapButton::drawNoStyle(const SDL_Rect& lRect)
{
    if(lRect.h == 0 || lRect.w == 0)
        return;

    GsButton::drawNoStyle(lRect);

    GsRect<Uint16> rect(lRect);

    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    if(mpBitmap)
    {
        GsRect<Uint16> bmpRect = {0, 0,
                                  rect.dim.x, rect.dim.y};

        if(mKeepAspectRatio)
        {
            float resX = rect.dim.x;
            float resY = rect.dim.y;

            if(rect.dim.x < rect.dim.y)
            {
                resY = (resX*float(mpBitmap->height()))/
                        (float(mpBitmap->width()));
            }
            else if(rect.dim.x > rect.dim.y)
            {
                resX = (resY*float(mpBitmap->width()))/
                        (float(mpBitmap->height()));
            }
            bmpRect = {0, 0, Uint16(resX), Uint16(resY)};
        }
        mpBitmap->scaleTo(bmpRect);

        mpBitmap->draw(rect.pos.x, rect.pos.y, blitsfc);
    }
}

