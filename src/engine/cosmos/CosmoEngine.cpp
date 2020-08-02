
#include "CosmoEngine.h"


namespace cosmos_engine
{

CosmosEngine::CosmosEngine(const bool openedGamePlay,
                           const int ep,
                           const std::string &dataPath) :
GameEngine(openedGamePlay, dataPath),
mEp(ep)
{}

CosmosEngine::~CosmosEngine()
{}

extern "C"
{
    int start_cosmo();
}

/**
 * @brief start Starts the Dreams engine which refers to accessing RefKeen code
 */
bool CosmosEngine::start()
{
    start_cosmo();
    return false;
}

/**
 * @brief ponder    Everything logic related here
 * @param deltaT    timestep
 */
void CosmosEngine::ponder(const float deltaT)
{}

void CosmosEngine::pumpEvent(const CEvent *evPtr)
{}


/**
 * @brief render    Everything to ne drawn here!
 */
void CosmosEngine::render()
{}


}


StartCosmosEngine::StartCosmosEngine(const bool openedGamePlay,
                                     const int ep,
                                     const std::string &dataPath) :
    SwitchEngineEvent(
        new cosmos_engine::CosmosEngine(openedGamePlay, ep, dataPath) )
{ }
