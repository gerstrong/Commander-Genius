#ifndef GSDYNCOLOR_H
#define GSDYNCOLOR_H

#include <base/utils/Color.h>
#include <graphics/GsSurface.h>

/**
 * @brief The GsDynColor class
 * besides setting a color, it controls a certain blend
 * effect used for buttons and the alike.
 * It works like this:
 * 1. If you set a color, it will slowly blend to that color
 * 2. The blending works by the passing process() method, you have to call
 *    that one in your classes
 */

class GsDynColor
{
public:
    GsDynColor();

    void ponder(const float dt);

    Uint32 toUint32(GsWeakSurface &blitsfc) const;

private:

    GsColor mTargetColor; // Where it blends to
    GsColor mCurrentColor; // The color it currently holds
};

#endif // GSDYNCOLOR_H
