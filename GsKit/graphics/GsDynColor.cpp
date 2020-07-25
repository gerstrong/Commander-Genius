#include "GsDynColor.h"

GsDynColor::GsDynColor()
{

}

GsDynColor::GsDynColor(const float red,
           const float green,
           const float blue) :
mTargetColor(red, green, blue)
{}


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
