#ifndef VGAMEPADSIMPLE_H
#define VGAMEPADSIMPLE_H

#include <base/GsVirtualinput.h>

class VirtualMenuControl : public GsVirtualInput
{
public:


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

#endif

};

#endif // VGAMEPADSIMPLE_H
