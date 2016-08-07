#ifndef DREAMSGAMEPLAY_H
#define DREAMSGAMEPLAY_H

#include <base/GsEngine.h>


namespace dreams
{

struct LaunchGamePlay : CEvent {};

struct GoIntoPlayLoop : CEvent {};

struct CompleteLevel : CEvent {};



class DreamsGamePlay  : public GsEngine
{
public:
    DreamsGamePlay();

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

    void (*mPlayloopPtr)() = nullptr; /** Small hack so we do not get another loop getting stuck */
    void (*mPlayloopRenderPtr)() = nullptr; /** Small hack so we do not get another loop getting stuck (Rendering) */

};

}

#endif // DREAMSGAMEPLAY_H
