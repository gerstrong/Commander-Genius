#if defined(USE_VIRTUALPAD)

#ifndef VGAMEPADSIMPLE_H
#define VGAMEPADSIMPLE_H

#include "graphics/GsTextureElem.h"

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
    bool mouseState(const GsVec2D<float> &Pos, const bool down);

    /**
     * @brief mouseDown     Mouse down event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    bool mouseDown(const GsVec2D<float> &Pos) override
    {
        return mouseState(Pos, true);
    }

    /**
     * @brief mouseDown     Mouse Up event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    bool mouseUp(const GsVec2D<float> &Pos) override
    {
        return mouseState(Pos, false);
    }

    /**
     * @brief flush Flush stuck events
     */
    void flush() override;

#if SDL_VERSION_ATLEAST(2, 0, 0)

    bool allInvisible();

    bool handleDPad(const GsVec2D<float> &Pos,
                    const Sint64 fingerID,
                    const bool down);

    bool mouseFingerState(const GsVec2D<float> &Pos,
                          const bool isFinger,
                          const SDL_TouchFingerEvent &touchFingerEvent,
                          const bool down) override;

#endif

    /**
     * @brief active    Checks if click events happened in the virtual dpad
     * @return
     */

    bool isInside(const GsVec2D<float> &Pos) const override
    {
#if SDL_VERSION_ATLEAST(2, 0, 0)
        return true;
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

#if SDL_VERSION_ATLEAST(2, 0, 0)

    // Directional pad with control disc on top
    TouchButton mDPad;
    GsTextureElem mDiscTexture;

    // Misc buttons for menu, etc
    TouchButton mConfirmButton;
    TouchButton mStartButton;
    TouchButton mMenuButton;

    // Usual keen action buttons
    TouchButton mJumpButton;
    TouchButton mShootButton;
    TouchButton mPogoButton;
    TouchButton mStatusButton;        

    // Tracking set for used fingers or the mouse cursor
    std::set< SDL_FingerID > mFingerSet;

#endif    


};

#endif // VGAMEPADSIMPLE_H

#endif // VIRTUALPAD
