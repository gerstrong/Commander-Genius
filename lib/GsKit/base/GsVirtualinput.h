#ifndef GSVIRTUALINPUT_H
#define GSVIRTUALINPUT_H

#include "graphics/GsSurface.h"
#include "graphics/GsTexture.h"


/**
 * @brief The GsVirtualInput class represents input devices which might be absent yet required for the program
 *        to run. For example Commander Genius as of Commander Keen need Keyboard or Joystick in general.
 *        If you happen to play this on a mobile device, the touch controls should be translated within that class
 *        for controls.
 *        Since Pelya created for SDL 1.2 an overlay is handling that, so this class will be made for SDL 2.0 only. Time to move on...
 */

class GsVirtualInput
{
public:
    /**
     * @brief GsVirtualInput basic constructor, just setup some variables.
     */
    GsVirtualInput() :
    mEnabled(true),
    mTranslucency(0.5f) {}


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
    virtual bool init() = 0;

    /**
     * @brief render is called when it's time to render this object
     * @param sfc Reference to surface on which it can be rendered.
     */
    virtual void render(GsWeakSurface &sfc) = 0;

    /**
     * @brief mouseDown     Mouse down event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    virtual void mouseDown(const Vector2D<float> &Pos) = 0;

    /**
     * @brief mouseDown     Mouse Up event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    virtual void mouseUp(const Vector2D<float> &Pos) = 0;

protected:

    bool mEnabled;

    GsSurface mOverlay;

    float mTranslucency;

};


#endif // GSVIRTUALINPUT_H
