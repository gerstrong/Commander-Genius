#include "GsDynColor.h"

GsDynColor::GsDynColor()
{

}

GsDynColor::GsDynColor(const float red,
           const float green,
           const float blue) :
mTargetColor(Uint8(red*255.0f), Uint8(green*255.0f), Uint8(blue*255.0f))
{
    mPrevColor = mCurrentColor = mTargetColor;
}


Uint32 GsDynColor::toUint32(GsWeakSurface &blitsfc) const
{    
    return blitsfc.mapRGBA( mCurrentColor.r,
                            mCurrentColor.g,
                            mCurrentColor.b,
                            0xFF);
}

void GsDynColor::setTargetColor(const GsColor &newColor)
{
    if(newColor == mTargetColor)
        return;

    mPrevColor = mTargetColor;
    mTargetColor = newColor;
    mBlendAmount = 0.0f;
}

void GsDynColor::ponder(const float amt)
{    
    if(mBlendAmount < 1.0f)
    {
        mCurrentColor = mPrevColor;

        mCurrentColor.converge(mTargetColor, mBlendAmount);

        mBlendAmount += amt;

        if(mBlendAmount > 1.0f)
            mBlendAmount = 1.0f;
    }
    else
    {
        mCurrentColor = mTargetColor;
    }

}
