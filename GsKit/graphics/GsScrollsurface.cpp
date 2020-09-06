#include "GsScrollsurface.h"

#include "base/video/CVideoDriver.h"

GsScrollSurface::GsScrollSurface()
{

}

bool GsScrollSurface::create(const unsigned int mode,
                             const unsigned int squareSize)
{
    mScrollSurface.create(mode,
                          squareSize,
                          squareSize, 32,
                          0x00FF0000,
                          0x0000FF00,
                          0x000000FF,
                          0x00000000);

    return true;
}

void GsScrollSurface::resetScrollbuffer()
{
    mSbufferx = 0;
    mSbuffery = 0;
}


void GsScrollSurface::UpdateScrollBufX(const Sint16 SBufferX)
{
    const int drawMask = getSquareSize()-1;
    mSbufferx = SBufferX&drawMask;
}

void GsScrollSurface::UpdateScrollBufY(const Sint16 SBufferY)
{
    const int drawMask = getSquareSize()-1;
    mSbuffery = SBufferY&drawMask;
}

void GsScrollSurface::readScrollBuffer(Sint16 &x, Sint16 &y) const
{
    x = mSbufferx; y = mSbuffery;
}

int GsScrollSurface::getSquareSize() const
{
    assert(mScrollSurface);
    return mScrollSurface.width();
}

GsSurface &GsScrollSurface::getScrollSurface()
{
    return mScrollSurface;
}

Uint32 GsScrollSurface::getFlags() const
{
    assert(mScrollSurface);
    return mScrollSurface.getSDLSurface()->flags;
}

void GsScrollSurface::blitScrollSurface(GsWeakSurface &blitSfc) // This is only for tiles
{									                            // The name should be changed
    SDL_Rect srGsRect, dstrect;
    int sbufferx, sbuffery;

    CVidConfig &vidConf = gVideoDriver.getVidConfig();
    SDL_Rect Gamerect = vidConf.mGameRect.SDLRect();

    const int scrollSfcSize = getSquareSize();
    const auto &visGA = gVideoDriver.mpVideoEngine->mRelativeVisGameArea;

    Gamerect.x = visGA.pos.x;
    Gamerect.y = visGA.pos.y;
    Gamerect.w = visGA.dim.x+16;
    Gamerect.h = visGA.dim.y+16;

    dstrect.x = Gamerect.x;
    dstrect.y = Gamerect.y;
    sbufferx = mSbufferx + dstrect.x;
    sbuffery = mSbuffery + dstrect.y;

    // Clip the scrollbuffer correctly
    if(sbufferx > scrollSfcSize)
        sbufferx -= scrollSfcSize;
    if(sbuffery > scrollSfcSize)
        sbuffery -= scrollSfcSize;

    // TODO: Not sure, if we shall do this.
    blitSfc.fillRGB(0, 0, 0);

    srGsRect.x = sbufferx;
    srGsRect.y = sbuffery;

    const bool wraphoz = (sbufferx+Gamerect.w > scrollSfcSize);
    const bool wrapvrt = (sbuffery+Gamerect.h > scrollSfcSize);

    // Upper-Left Part to draw from the Scrollbuffer
    srGsRect.w = wraphoz ? (scrollSfcSize-sbufferx) : Gamerect.w;
    srGsRect.h = wrapvrt ? (scrollSfcSize-sbuffery) : Gamerect.h;

    mScrollSurface.blitTo(blitSfc, srGsRect, dstrect);

    const Uint16 upperLeftW = srGsRect.w;
    const Uint16 upperLeftH = srGsRect.h;

    // upper-right part
    if (wraphoz)
    {
        srGsRect.w = Gamerect.w - upperLeftW;
        srGsRect.x = 0;
        dstrect.x = Gamerect.x + upperLeftW;

        mScrollSurface.blitTo(blitSfc, srGsRect, dstrect);
    }

    // lower-right part
    if (wrapvrt)
    {
        srGsRect.h = Gamerect.h - upperLeftH;
        srGsRect.y = 0;
        dstrect.y = Gamerect.y + upperLeftH;

        mScrollSurface.blitTo(blitSfc, srGsRect, dstrect);
    }

    if(!wraphoz || !wrapvrt)
        return;

    // and lower-left part
    srGsRect.x = sbufferx;
    srGsRect.y = 0;
    srGsRect.w = upperLeftW;

    dstrect.x = Gamerect.x;
    dstrect.y = Gamerect.y+upperLeftH;

    mScrollSurface.blitTo(blitSfc, srGsRect, dstrect);
}

