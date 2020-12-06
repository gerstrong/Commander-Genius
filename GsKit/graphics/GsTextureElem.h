#ifndef GSTEXTUREELEM_H
#define GSTEXTUREELEM_H

#include "GsTexture.h"
#include "base/utils/Geometry.h"



/**
 * @brief The GsTextureElem class is composed by GsTexture and has a Rect which
 *        describes the coordinates and size of the element.
 */
class GsTextureElem
{
  public:

    /**
     * @brief createCircle Creates a circle
     * @param radius    Radius of the circle which comes down
     *                  the height and width of the texture
     * @param res       determines the number of slices used for creating
     *                  the circle image. The higher the value the less pixelated
     *                  it looks on high res screens, but also more memory is consumed
     * @return true, if the circle could be created, otherwise false.
     */
    bool createCircle(const float radius,
                      const int res);

    /**
     * @brief setPos    Sets thje position of the texture element
     * @param pos       Position relative to screen where to place the element
     */
    void setPos(const GsVec2D<float> pos);

    GsRect<float> Rect() const
    { return mRect; }

    GsTexture &Texture()
    { return mTexture; }

  private:
    GsRect<float> mRect;
    GsTexture mTexture;
};

#endif // #define GSTEXTUREELEM_H
