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

