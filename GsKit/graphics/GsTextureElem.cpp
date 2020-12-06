
#include "GsTextureElem.h"

#include "base/video/CVideoDriver.h"

bool GsTextureElem::createCircle(const float radius,                                 
                                 const int res,
                                 const Uint8 r,
                                 const Uint8 g,
                                 const Uint8 b)

{
    mRect.dim.x = 2.0f*radius;
    mRect.dim.y = 2.0f*radius;
    return mTexture.createCircle(gVideoDriver.Renderer(),
                                 r,g,b, radius, 100);
}

void GsTextureElem::setPos(const GsVec2D<float> pos)
{
    mRect.pos = pos;
}
