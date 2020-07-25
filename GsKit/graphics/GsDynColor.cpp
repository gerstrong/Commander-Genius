#include "GsDynColor.h"

GsDynColor::GsDynColor()
{

}

Uint32 GsDynColor::toUint32(GsWeakSurface &blitsfc) const
{
    return blitsfc.mapRGBA( mCurrentColor.r,
                            mCurrentColor.g,
                            mCurrentColor.b,
                            0xFF);
}

void GsDynColor::ponder(const float dt)
{
    (void) dt;
}
