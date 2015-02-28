#ifndef VGAMEPADSIMPLE_H
#define VGAMEPADSIMPLE_H

#include <base/GsVirtualinput.h>

class VirtualKeenControl : public GsVirtualInput
{
public:

    // OK => Only show Ok Button
    // WMAP => Shows Enter Button for the world map and another one for the item box and one for the main menu
    // ACTION => Shows the three action button for jumping, pogo and fire as well as the item box button and the main menu button
    enum BUTTON_MODE
    {
        OK = 0,
        WMAP = 1,
        ACTION = 2
    };

    /**
     * @brief VirtualKeenControl Will initialize
     */
    VirtualKeenControl() :
        mShowDPad(true),
        mHideEnterButton(false),
        mButtonMode(OK) {}


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
    void mouseDown(const Vector2D<float> &Pos)
    {
        mouseState(Pos, true);
    }

    /**
     * @brief mouseDown     Mouse Up event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    void mouseUp(const Vector2D<float> &Pos)
    {
        mouseState(Pos, false);
    }

#if SDL_VERSION_ATLEAST(2, 0, 0)

    GsTexture mDPadTexture;
    GsTexture mConfirmButtonTexture;
    GsTexture mStartButtonTexture;

    // Control buttons
    GsTexture mJumpButtonTexture;

#endif

    bool mShowDPad;
    bool mHideEnterButton;

    BUTTON_MODE mButtonMode;
};

#endif // VGAMEPADSIMPLE_H
