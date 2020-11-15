#ifndef GSSCROLLSURFACE_H
#define GSSCROLLSURFACE_H

#include "graphics/GsSurface.h"

/**
 * @brief The GsScrollSurface class
 * @author Gerstrong
 *
 * Replaces the classic scrollsurface routines and will add features, such as
 * multiscroll surfaces (For example for parallax scrolling)
 */

class GsScrollSurface
{
public:
    GsScrollSurface();

    bool create(const unsigned int mode,
                const unsigned int squareSize);

    void resetScrollbuffer();

    void UpdateScrollBufX(const Sint16 SBufferX);
    void UpdateScrollBufY(const Sint16 SBufferY);
    void readScrollBuffer(Sint16 &x, Sint16 &y) const;

    int getSquareSize() const;

    void blitScrollSurface(GsWeakSurface &blitSfc);

    GsSurface &getScrollSurface();

    Uint32 getFlags() const;

private:

    GsSurface   mScrollSurface; // Squared scroll buffer

    Sint16 mSbufferx = 0;
    Sint16 mSbuffery = 0;

};

#endif // GSSCROLLSURFACE_H
