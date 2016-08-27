#ifndef DREAMSCONTROLPANEL_H
#define DREAMSCONTROLPANEL_H

#include <base/GsEngine.h>

#include "dreamsgameplay.h"


namespace dreams
{

struct LaunchControlPanel : CEvent {};


struct OpenLineInput : CEvent {};


/**
 * @brief The LineInput class   processes the input of text when saving a game as open loops
 *                              for pondering and rendering.
 */
class LineInput
{
public:

    void start();

    void ponder();

    void render();

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
