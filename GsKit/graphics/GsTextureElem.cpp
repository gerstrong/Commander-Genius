
#include "GsTextureElem.h"

#include "base/video/CVideoDriver.h"

bool GsTextureElem::createCircle(const float radius,
                                 const int res)

{
    mTexture.fillRGB(gVideoDriver.Renderer(), 100, 0, 0);

    mRect = { 0.0f, 0.0f,
              radius, radius };

    return true;
}

void GsTextureElem::setPos(const GsVec2D<float> pos)
{
    mRect.pos = pos;
}
