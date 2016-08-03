#ifndef DREAMSCONTROLPANEL_H
#define DREAMSCONTROLPANEL_H

#include <base/GsEngine.h>



namespace dreams
{

struct LaunchControlPanel : CEvent {};


class DreamsControlPanel : public GsEngine
{
public:

    DreamsControlPanel();

    void start();

    /**
     * @brief pumpEvent Events like user closes windows or mouse presses are processed here.
     * @param evPtr
     */
    void pumpEvent(const CEvent *evPtr) {};

    /**
     * @brief ponder
     */
    void ponder(const float deltaT);

    /**
     * @brief render
     */
    void render();

};

}

#endif // DREAMSCONTROLPANEL_H
