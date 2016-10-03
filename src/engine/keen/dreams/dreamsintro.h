#ifndef DREAMSINTRO_H
#define DREAMSINTRO_H

#include <base/GsEngine.h>

#include <string>

namespace dreams
{

struct SwitchToIntro : CEvent {};

/**
 * @brief The DreamsIntro class     Class of the graphical after the text based intro was shown.
 *                                  At this stage graphics and audio are already loaded.
 *                                  The main intro screen and high scores are actually shown here.
 */
class DreamsIntro : public GsEngine
{
public:
    DreamsIntro();

    /**
     * @brief start Usually this is started before anything else but still after the construction.
     */
    void start();

    /**
     * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
     * @param evPtr
     */
    void pumpEvent(const CEvent *evPtr);

    /**
     * @brief ponder    Logic cycle run usually at 120 LPS
     * @param deltaT    how much time of logic to do
     */
    void ponder(const float deltaT) ;

    /**
     * @brief render Everything that needs to be rendered representing the current state of the object
     */
    void render();

private:

    int mStep = 1;
    int mTimer = 0;
};

}

#endif // DREAMSINTRO_H
