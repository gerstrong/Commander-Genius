#ifndef VORTICONENGINE_H
#define VORTICONENGINE_H

#include "engine/keen/KeenEngine.h"
#include "CEGAGraphicsVort.h"
#include "CInfoScene.h"

namespace vorticon
{


bool setupAudio();

class VorticonEngine : public KeenEngine
{
public:
    const int mEp;

    VorticonEngine(const bool openedGamePlay,
                   const int ep,
                   const std::string &dataPath);

    void ponder(const float deltaT);

    void render();

    void openMainMenu();

    bool loadResources( const Uint8 flags );

    void switchToPassiveMode();

    void pumpEvent(const std::shared_ptr<CEvent> &evPtr);

private:
    std::shared_ptr<CInfoScene> mpInfoScene;
};

}

// Events
struct StartVorticonEngine : public SwitchEngineEvent
{
    StartVorticonEngine(const bool openedGamePlay,
                        const int ep,
                        const std::string &dataPath) :
        SwitchEngineEvent( new vorticon::VorticonEngine(openedGamePlay, ep, dataPath) )
        { }
};


#endif /* VORTICONENGINE_H */
