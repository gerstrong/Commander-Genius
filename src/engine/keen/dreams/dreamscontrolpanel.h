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
    const char *sig,
    bool escok,
    int maxchars,
    int maxwidth,
    int n) :
        x(x),
        y(y),
        buf(buf),
        def(def),
        signature(sig),
        escok(escok),
        maxchars(maxchars),
        maxwidth(maxwidth),
        mN(n)
        {}

    int x, y;
    char *buf;
    const char *def;
    std::string signature;
    bool escok;
    int maxchars;
    int maxwidth;
    int mN;
};


struct CloseLineInput : CEvent {};


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
               int maxwidth, int n);

    void ponder();

    void render();

private:

    bool mCursorvis = false;
    bool mDone = false;
    bool mRedraw = true;
    bool mResult;
    bool mCursorMoved = true;
    int mCursor;

    int mMaxchars, mMaxwidth;

    bool mEscok;

    std::string mStr;

    int mx, my;
    int mN;

    uint8_t mSc;

    char mC;
    char *mBuf;

    int	mLasttime;
};


struct DoHelp : CEvent
{
    DoHelp(const void *text, int len) :
        mLen(len)
    {
        mText = std::string( static_cast<const char*>(text) );
    }

    bool init();

    bool ponder();

    std::string mText;
    int mLen;
    int mLines;
    bool mReleased = false;
    bool mDone = false;
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

    std::shared_ptr<DoHelp> mpDoHelpEvent;

};

}

#endif // DREAMSCONTROLPANEL_H
