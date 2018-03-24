#ifndef GSPROGRESSBAR_H
#define GSPROGRESSBAR_H

#include "GsControl.h"

/**
 * @brief The GsProgressBar class   Nice Progress bar for Menus
 *        and Commander Keen GUI. It is mainly used by the download manager.
 */
class GsProgressBar : public GsControl
{
public:

    GsProgressBar(const int &progress) :
        mProgress(progress) {}


    void processLogic() override;

    void processRender(const GsRect<float> &RectDispCoordFloat) override;

    const bool finished() const
    {
        return mFinished;
    }

    void enableFancyAnimation(const bool value)
    {
        mDoFancyAnimation = value;
    }

    void setBad(const bool value)
    {
        mBad = value;
    }

private:

    /**
     * @brief applyFancyAnimation Nice animation showing that something is happening
     * @param pBlitsurface      Surface where to blit
     * @param progressSDLRect   Rect where to blit that fancy animation
     * @param color1            First color
     * @param color2            Second color
     */
    void applyFancyAnimation(SDL_Surface *pBlitsurface,
                             const SDL_Rect &progressSDLRect,
                             const Uint32 color1,
                             const Uint32 color2);

    const int &mProgress;

    float mProgressToRender = 0.0f; // Progress to render. It might be behind "mProgress"
                                   // when a nice animation takes place

    int mAnimationPos = 0; // Position of a nice animation making it look like something
                                // is happening while progressing :-)

    bool mFinished = false;

    const int mFuelAnimationWidth = 10;

    // Flashy animation when finished downloading!
    int mFlashBlue = 128;
    int mFlashAmt = 1; // Amount to flash, changes to -1 when mas reached
    int mFlashTimer = 120; // 120 cycles are about one second of time for flashing

    bool mDoFancyAnimation = false;

    // Bad will render the control red and show different message
    bool mBad = false;
};

#endif // GSPROGRESSBAR_H
