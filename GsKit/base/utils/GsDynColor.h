#ifndef GSDYNCOLOR_H
#define GSDYNCOLOR_H

#include <base/utils/Color.h>

/**
 * @brief The GsDynColor class
 * besides settings a color it controls for a certain blend
 * effect used for buttons and the alike.
 * It works like this:
 * 1. If you set a color, it will slowly blend to that color
 * 2. The speed is determined by the passed method process(), you have to call
 *    that one in your classes
 */

class GsDynColor
{
public:
    GsDynColor();

private:

    GsColor mTargetColor; // Where it blends to
    GsColor mCurrentColor; // The color it currently holds
};

#endif // GSDYNCOLOR_H
