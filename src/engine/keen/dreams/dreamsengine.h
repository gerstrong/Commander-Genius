#ifndef DREAMSENGINE_H
#define DREAMSENGINE_H

#include <base/GsEngine.h>
#include <engine/core/GameEngine.h>

namespace dreams
{


class DreamsEngine : public GameEngine
{
public:
    DreamsEngine(const bool openedGamePlay,
                 const std::string &dataPath) :
    GameEngine(openedGamePlay, dataPath) {}

    /**
     * @brief start Starts the Dreams engine which refers to accessing RefKeen code
     */
    void start();

};

}

// Events
struct StartDreamsEngine : public SwitchEngineEvent
{
    StartDreamsEngine(const bool openedGamePlay,
                      const std::string &dataPath) :
        SwitchEngineEvent( new dreams::DreamsEngine(openedGamePlay, dataPath) )
        { }
};


#endif // DREAMSENGINE_H
