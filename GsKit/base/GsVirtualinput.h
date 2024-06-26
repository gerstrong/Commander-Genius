#ifndef GSVIRTUALINPUT_H
#define GSVIRTUALINPUT_H

#include "graphics/GsSurface.h"
#include "graphics/GsTexture.h"
#include "base/Vector2D.h"

#include <set>

class TouchButton
{

public:

    /**
     * @brief isInside Is given coordinate inside the button?
     */    
    template<class T>
    bool isInside(const T _x, const T _y) const
    {
        return
        x <= _x && _x < x + w &&
        y <= _y && _y < y + h;
    }

    template<class T>
    bool isInside(const GsVec2D<T> _pos) const
    {
        return isInside(_pos.x, _pos.y);
    }


    /**
     * @brief setRect   set coordinates and dimensions of the loaded texture
     * @param rect      variable that does that
     */
    auto setRect(const GsRect<float> &rect) -> void
    {
        this->x = rect.pos.x; this->y = rect.pos.y;
        this->w = rect.dim.x; this->h = rect.dim.y;
    }

    auto Rect() const -> auto
    {
        return GsRect<float>(this->x, this->y,
                             this->w, this->h);
    }

    /**
     * @brief loadPicture   load the button-picture from a given file
     * @param picFile       file to load
     */
    bool loadPicture(const std::string &picFile);

    /**
     * @brief loadEmdbeddedPicture  Load picture from internal memory
     * @param data
     * @return
     */
    bool loadEmdbeddedPicture(const unsigned char *data,
                              const unsigned int size);


    /**
     * @brief handleFingerEvent handle finger event (down/up)
     *                          and decide the state of the touch button
     * @param Pos
     * @param fingerID
     * @param fingerDown
     * @return false if the touch button is not affected otherwise true.
     */
    bool handleFingerEvent(const GsVec2D<float> &Pos,
                           const Sint64 fingerID,
                           const bool fingerDown);

    void clearFingers();

    void insertFingerId(const Sint64 fid);

    void removeFingerId(const Sint64 fid);

    bool hasFingers() const
    {
        return !mFingerSet.empty();
    }


    int immediateIndex = 0;

    float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f;
    bool invisible = true;
    bool isDown = false;

    GsTexture mTexture;
    GsTexture mHighlightTexture;

private:

    std::set<Sint64> mFingerSet;
};

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
    GsVirtualInput() {}


    virtual ~GsVirtualInput() {}

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
     * @brief ponder Performs some logic like transformations.
     * @return
     */
    virtual bool ponder() = 0;


    /**
     * @brief flush
     */
    virtual void flush() = 0;

    /**
     * @brief renderConfig  Render configuration dialog of the dialog
     */
    virtual void processConfig() = 0;


    /**
     * @brief renderConfig  Render configuration dialog of the dialog
     */
    virtual void renderConfig() = 0;

    /**
     * @brief render is called when it's time to render this object
     * @param sfc Reference to surface on which it can be rendered.
     */
    virtual void render(GsWeakSurface &sfc) = 0;

    /**
     * @brief mouseDown     Mouse down event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    virtual bool mouseDown(const GsVec2D<float> &Pos) = 0;

    /**
     * @brief mouseUp     Mouse Up event when sent when touch event triggered or mouse sends that.
     * @param Pos           Position of the mouse event
     */
    virtual bool mouseUp(const GsVec2D<float> &Pos) = 0;


#if SDL_VERSION_ATLEAST(2, 0, 0)

    /**
     * @brief mouseFingerState Event Fingers on touch surfaces
     * @param Pos position
     * @param touchFingerEvent event sent by SDL
     * @return true if something was processed here
     */
    virtual bool mouseFingerState(const GsVec2D<float> &Pos,
                                  const bool isFinger,
                                  const SDL_TouchFingerEvent &touchFingerEvent,
                                  const bool down) = 0;

#endif

    /**
     * @brief active    Checks if click events happened in the virtual dpad
     * @return
     */
    virtual bool isInside(const GsVec2D<float> &Pos) const = 0;


protected:

    bool mEnabled = true;

    float mTranslucency = 0.5f;

};


#endif // GSVIRTUALINPUT_H
