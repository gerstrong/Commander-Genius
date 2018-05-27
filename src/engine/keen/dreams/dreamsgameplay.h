#ifndef DREAMSGAMEPLAY_H
#define DREAMSGAMEPLAY_H

#include <base/GsEngine.h>


namespace dreams
{

//struct QuitApp : CEvent {};

struct SwitchToGamePlay : CEvent {};

struct GoIntoPlayLoop : CEvent {};

struct CompleteLevel : CEvent {};



class DreamsGamePlay  : public GsEngine
{
public:
    DreamsGamePlay();

    /**
     * @brief start called the first time usually when the instance was created
     */
    bool start() override;

    /**
     * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
     *                  If there are underlying objects using events, they are passed
     * @param evPtr
     */
    void pumpEvent(const CEvent *evPtr) override;

    /**
     * @brief ponder
     */
    void ponder(const float deltaT) override;

    /**
     * @brief render
     */
    void render() override;

private:

    void (*mPlayloopPtr)() = nullptr; /** Small hack so we do not get another loop getting stuck */
    void (*mPlayloopRenderPtr)() = nullptr; /** Small hack so we do not get another loop getting stuck (Rendering) */

};

}

#endif // DREAMSGAMEPLAY_H
