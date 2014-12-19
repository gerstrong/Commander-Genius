#ifndef GSVIRTUALINPUT_H
#define GSVIRTUALINPUT_H

#include "graphics/GsSurface.h"
#include "graphics/GsTexture.h"


// TODO: Virtual Pad Base Class which in which the derived is managed by the launched game itself.
// GsVirtualInput should only get a shared pointer to that derived class which can be exchanged at anytime.

/*enum VPadMode
{
    VPADMODE_MENU,
    VPADMODE_WORLDMAP, // Show buttons
    VPADMODE_GAMEPLAY
};*/


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

    /**
     * @brief mouseState    Mouse state processing. Since the up and down code are similar,
     *                      they are just redirected here with the down state
     * @param Pos           Position
     * @param down          true if event indicates mouse button down, otherwise up
     */
    void mouseState(const Vector2D<float> &Pos, const bool down);

    /**
     * @brief mouseDown     Mouse down event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    void mouseDown(const Vector2D<float> &Pos);

    /**
     * @brief mouseDown     Mouse Up event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    void mouseUp(const Vector2D<float> &Pos);

private:

    bool mEnabled;

    GsSurface mOverlay;

    float mTranslucency;

#if SDL_VERSION_ATLEAST(2, 0, 0)

    GsTexture mDPadTexture;

#endif



};


#endif // GSVIRTUALINPUT_H
