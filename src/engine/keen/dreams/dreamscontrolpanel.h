#ifndef DREAMSCONTROLPANEL_H
#define DREAMSCONTROLPANEL_H

#include <base/GsEngine.h>

#include "dreamsgameplay.h"


namespace dreams
{

struct LaunchControlPanel : CEvent {};



struct OpenLineInput : CEvent
{
    OpenLineInput(int x, int y,
    char *buf,
    const char *def,
    bool escok,
    int maxchars,
    int maxwidth) :
        x(x),
        y(y),
        buf(buf),
        def(def),
        escok(escok),
        maxchars(maxchars),
        maxwidth(maxwidth) {}

    int x, y;
    char *buf;
    const char *def;
    bool escok;
    int maxchars;
    int maxwidth;
};


/**
 * @brief The LineInput class   processes the input of text when saving a game as open loops
 *                              for pondering and rendering.
 */
class LineInput
{
public:

    void start(int x, int y,
               char *buf, const char *def,
               bool escok, int maxchars,
               int maxwidth);

    void ponder();

    void render();

private:

    bool mCursorvis = false;
    bool mDone = false;
    bool mRedraw = true;
    bool mResult;
    bool mCursorMoved = true;
    int mCursor;

    std::string mStr;

    int mx, my;

    uint8_t mSc;

    char mC;
    char *mBuf;

};


class DreamsControlPanel : public GsEngine
{
public:

    DreamsControlPanel();


    ~DreamsControlPanel();

    /**
     * @brief start called the first time usually when the instance was created
     */
    void start();

    /**
     * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
     *                  If there are underlying objects using events, they are passed
     * @param evPtr
     */
    void pumpEvent(const CEvent *evPtr);

    /**
     * @brief ponder
     */
    void ponder(const float deltaT);

    /**
     * @brief render
     */
    void render();

private:

    std::unique_ptr<LineInput> mpLineInput;

};

}

#endif // DREAMSCONTROLPANEL_H
