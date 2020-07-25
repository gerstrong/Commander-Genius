#include "GsDynColor.h"

GsDynColor::GsDynColor()
{

}

GsDynColor::GsDynColor(const float red,
           const float green,
           const float blue) :
mTargetColor(Uint8(red*255.0f), Uint8(green*255.0f), Uint8(blue*255.0f))
{}


Uint32 GsDynColor::toUint32(GsWeakSurface &blitsfc) const
{    
    return blitsfc.mapRGBA( mCurrentColor.r,
                            mCurrentColor.g,
                            mCurrentColor.b,
                            0xFF);
}

void GsDynColor::setTargetColor(const GsColor &newColor)
{
    mTargetColor = newColor;
}

void GsDynColor::ponder(const float dt)
{    
    (void) dt;

    mCurrentColor = mTargetColor;
}
