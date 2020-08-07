
#include "CosmoEngine.h"
#include "cosmosintro.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/FindFile.h>


extern "C"
{
    void set_renderer(SDL_Renderer *rend);

    void set_game_data_dir(const char *dir, const int len);
}



namespace cosmos_engine
{

CosmosEngine::CosmosEngine(const bool openedGamePlay,
                           const int ep,
                           const std::string &dataPath) :
GameEngine(openedGamePlay, dataPath),
mEp(ep)
{
    set_renderer(gVideoDriver.Renderer());
}

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
    mpScene.reset( new CosmosIntro );

    const auto gameDir = GetFullFileName(mDataPath);

    set_game_data_dir(gameDir.c_str(),
                      gameDir.size());

    start_cosmo();

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
