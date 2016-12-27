#ifndef __GALAXYENGINE_H__
#define __GALAXYENGINE_H__

#include "engine/keen/KeenEngine.h"
#include "res/CEGAGraphicsGalaxy.h"
#include <base/GsEvent.h>

/**
 * The Galaxy engine derived from KeenEngine has the task to manage the whole Keen 4-6
 * games including intro, title screen, game play, everything within
 */

namespace galaxy
{

bool setupAudio();

bool loadLevelMusic(const int level);

class GalaxyEngine : public KeenEngine
{
public:

    const int mEp;

    GalaxyEngine(const bool openedGamePlay,
                 const int ep,
                 const std::string &dataPath) :
        KeenEngine(openedGamePlay, ep, dataPath),
        mEp(ep) {}

    void openMainMenu();

    bool loadResources( const Uint8 flags );

    /**
     * @brief switchToPassive   Switch to passive mode meaning intro, title text will be cycling
     */
    void switchToPassive();

    /**
     * @brief switchToGameplay  Switch directly to gameplay and start certain level
     * @param startLevel    Level which to start
     */
    void switchToGameplay(const int startLevel);

    void pumpEvent(const CEvent *evPtr);
};

}

// Events
struct StartGalaxyEngine : public SwitchEngineEvent
{
    StartGalaxyEngine(const bool openedGamePlay,
                      const int ep,
                      const std::string &dataPath) :
        SwitchEngineEvent( new galaxy::GalaxyEngine(openedGamePlay, ep, dataPath) )
        { }
};


#endif /* __GALAXYENGINE_H__ */
