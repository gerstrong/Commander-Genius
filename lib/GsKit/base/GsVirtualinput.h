#ifndef GSVIRTUALINPUT_H
#define GSVIRTUALINPUT_H



#include "graphics/GsSurface.h"

/**
 * @brief The GsVirtualInput class represents input devices which might be absent yet required for the program
 *        to run. For example Commander Genius as of Commander Keen need Keyboard or Joystick.
 *        If you happen to play this on a mobile device, the touch controls should be translated within that class
 *        for controls.
 *        Since Pelya created for SDL 1.2 an overlay handling that, this will be made for SDL 2.0 only. Time to move on...
 */

class GsVirtualInput
{
public:
    GsVirtualInput();

    /**
     * @brief active
     * @return tells wheter Virtual Input is enabled.
     */
    bool active() const
    {
        return mEnabled;
    }

    /**
     * @brief init initialize the object
     * @return true if everything went right, otherwise false.
     */
    bool init();

    /**
     * @brief render is called when it's time to render this object
     * @param sfc Reference to surface on which it can be rendered.
     */
    void render(GsWeakSurface &sfc);

private:

    bool mEnabled;

    GsSurface mOverlay;

};


#endif // GSVIRTUALINPUT_H
