#ifndef __GALAXYENGINE_H__
#define __GALAXYENGINE_H__

#include "engine/keen/KeenEngine.h"
#include "res/CEGAGraphicsGalaxy.h"
#include "menu/ComputerWrist.h"
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

    /**
     * @brief ponder    Ponder cycle of the Galaxy Engine
     * @param deltaT    time to do logics
     */
    void ponder(const float deltaT);

    /**
     * @brief render    Render what ever happens in the engine
     */
    void render();

    /**
     * @brief openMainMenu Open the famous swatch main menu
     */
    void openMainMenu();




    /**
     * @brief loadResources This is used for loading all the resources of the game the use has chosen.
     *                      It loads graphics, sound and text into the memory
     * @param flags     Tells what type(s) of resources to load
     * @return  true if everything went fine, otherwise false.
     */
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

    std::unique_ptr<ComputerWrist> mpComputerWrist;
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
