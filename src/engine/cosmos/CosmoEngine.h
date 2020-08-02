#include <engine/core/GameEngine.h>
#include <string>

namespace cosmos_engine
{

class CosmosEngine : public GameEngine
{
public:
    CosmosEngine(const bool openedGamePlay,
                 const int ep,
                 const std::string &dataPath);


    virtual ~CosmosEngine();

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
    bool start() override;

    /**
     * @brief ponder    Everything logic related here
     * @param deltaT    timestep
     */
    void ponder(const float deltaT) override;

    void pumpEvent(const CEvent *evPtr) override;


    /**
     * @brief render    Everything to ne drawn here!
     */
    void render() override;


private:

    /**
     * @brief mChangeMode   An integer that triggers internal resolution change
     */
    int mChangeMode = 0; // 0 means nothing, any other value sets the mode before


    /**
     * @brief mpScene   A flexible pointer to a class instance in which different functionalities can projected
     */
    std::unique_ptr<GsEngine> mpScene;

    const int mEp;
};

}


// Events
struct StartCosmosEngine : public SwitchEngineEvent
{
    StartCosmosEngine(const bool openedGamePlay,
                      const int ep,
                      const std::string &dataPath);
};

