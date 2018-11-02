#ifndef VGAMEPADSIMPLE_H
#define VGAMEPADSIMPLE_H

#include <base/GsVirtualinput.h>
#include <base/CInput.h>
#include <utility>
#include <set>

class VirtualKeenControl : public GsVirtualInput
{
public:

    virtual ~VirtualKeenControl() override;

    // OK => Only show Ok Button
    // WMAP => Shows Enter Button for the world map and another one for the item box and one for the main menu
    // ACTION => Shows the three action button for jumping, pogo and fire as well as the item box button and the main menu button
    enum class BUTTON_MODE
    {  OK, WMAP, ACTION  };

    /**
     * @brief init initialize the object
     * @return true if everything went right, otherwise false.
     */
    bool init() override;

    /**
     * @brief ponder Performs some logic like transformations
     * @return
     */
    bool ponder() override;


    /**
     * @brief renderConfig  Render configuration dialog of the dialog
     */
    void processConfig() override;


    /**
     * @brief renderConfig  Render configuration dialog of the dialog
     */
    void renderConfig() override;


    /**
     * @brief render is called when it's time to render this object
     * @param sfc Reference to surface on which it can be rendered.
     */
    void render(GsWeakSurface &sfc) override;

    /**
     * @brief mouseState    Mouse state processing. Since the up and down code are similar,
     *                      they are just redirected here with the down state
     * @param Pos           Position
     * @param down          true if event indicates mouse button down, otherwise up
     */
    bool mouseState(const Vector2D<float> &Pos, const bool down);

    /**
     * @brief mouseDown     Mouse down event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    bool mouseDown(const Vector2D<float> &Pos) override
    {
        return mouseState(Pos, true);
    }

    /**
     * @brief mouseDown     Mouse Up event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    bool mouseUp(const Vector2D<float> &Pos) override
    {
        return mouseState(Pos, false);
    }

    /**
     * @brief flush Flush stuck events
     */
    void flush() override;

#if SDL_VERSION_ATLEAST(2, 0, 0)

    bool mouseFingerState(const Vector2D<float> &Pos,
                          const SDL_TouchFingerEvent &touchFingerEvent,
                          const bool down) override;


#endif
    /**
     * @brief active    Checks if click events happened in the virtual dpad
     * @return
     */
    bool isInside(const Vector2D<float> &Pos) const override
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        return mPadBackground.isInside(Pos.x, Pos.y);
#else
        return false;
#endif
    }



    /**
     * @brief hideAllButtons    Hide all the virtual button
     */
    void hideAllButtons();

    /**
     * @brief hideEverything    Hide the whole pad.
     */
    void hideEverything();


    TouchButton mPadBackground;
    TouchButton mDPad;
    TouchButton mConfirmButton;
    TouchButton mStartButton;

    // Control buttons
    TouchButton mJumpButton;
    TouchButton mShootButton;
    TouchButton mPogoButton;
    TouchButton mStatusButton;        

#if SDL_VERSION_ATLEAST(2, 0, 0)
    std::set< SDL_FingerID > mFingerSet;
#endif    

};

#endif // VGAMEPADSIMPLE_H
