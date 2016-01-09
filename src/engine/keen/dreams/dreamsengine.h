#ifndef DREAMSENGINE_H
#define DREAMSENGINE_H

#include <base/GsEngine.h>
#include <engine/core/GameEngine.h>
#include <base/utils/ThreadPool.h>
#include <graphics/GsSurface.h>
#include <memory>


namespace dreams
{


class DreamsEngine : public GameEngine
{
public:
    DreamsEngine(const bool openedGamePlay,
                 const std::string &dataPath) :
    GameEngine(openedGamePlay, dataPath) {}


    ~DreamsEngine();

    /**
     * @brief loadResources Load DOS data.
     * @return true if everything went well otherwise false.
     */
    bool loadResources();


    void GameLoop();

    /**
     * @brief setScreenMode     Set the screen mode for refkeen which will change the DreamsSurface for different graphics card mode emulations.
     *                          By setting mChangeMode, applyScreenmode is called whenever before rendering occurs.
     *                          This is necessary because Dreams uses a second thread. That one must never change the DreamsSurface, because the main
     *                          thread could be rendering whiletime
     * @param mode              Mode to be set as supplied by refkeen
     */
    void setScreenMode(const int mode)
    {
        mChangeMode = mode;
    }


    /**
     * @brief applyScreenMode
     */
    void applyScreenMode();


    /**
     * @brief start Starts the Dreams engine which refers to accessing RefKeen code
     */
    void start();

    /**
     * @brief ponder    Everything logic related here
     * @param deltaT    timestep
     */
    void ponder(const float deltaT);

    void pumpEvent(const CEvent *evPtr);


    /**
     * @brief render    Everything to ne drawn here!
     */
    void render();


private:

    enum GameState
    {
        INTRO_TEXT,     // The famous screen where hardware is detected and some notes about the versions are told
        INTRO_SCREEN    // Within the gameloop it will show the intro screen of the dreams game
    } mGameState = INTRO_TEXT;

    std::unique_ptr<Action> mpPlayLoopAction;
    std::unique_ptr<ThreadPoolItem> mpPlayLoopThread;

    /**
     * @brief mChangeMode   An integer that triggers internal resolution change
     */
    int mChangeMode = 0; // 0 means nothing, any other value sets the mode before
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


struct mapfiletype_modern
{
    uint16_t                       RLEWtag;
    uint32_t                       headeroffsets[100];
    uint8_t                        headersize[100];		// headers are very small
    std::vector<uint8_t>           tileinfo;
};



#endif // DREAMSENGINE_H
