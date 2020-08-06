
#include "CosmoEngine.h"
#include "cosmosintro.h"


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

    mpScene.reset( new CosmosIntro );

    return false;
}

/**
 * @brief ponder    Everything logic related here
 * @param deltaT    timestep
 */
void CosmosEngine::ponder(const float deltaT)
{
    /*
    if(!mResourcesLoaded)
        return;

    std::vector<SDL_Event> evVec;
    gInput.readSDLEventVec(evVec);

    for(SDL_Event event : evVec)
    {
        BE_ST_PollEvents(event);
    }
*/

    if(mpScene)
    {
        mpScene->ponder(deltaT);
    }
}

void CosmosEngine::pumpEvent(const CEvent *evPtr)
{
    if(mpScene)
    {
        mpScene->pumpEvent(evPtr);
    }
}


/**
 * @brief render    Everything to ne drawn here!
 */
void CosmosEngine::render()
{
    if(mpScene)
    {
        mpScene->render();
    }

    //SDL_Surface *blitSfc = gVideoDriver.getBlitSurface();
}


}


StartCosmosEngine::StartCosmosEngine(const bool openedGamePlay,
                                     const int ep,
                                     const std::string &dataPath) :
    SwitchEngineEvent(
        new cosmos_engine::CosmosEngine(openedGamePlay, ep, dataPath) )
{ }
