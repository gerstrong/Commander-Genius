
#include "CosmoEngine.h"
#include "cosmosintro.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/FindFile.h>
#include <base/CInput.h>

#include "cosmogameplay.h"

typedef enum
{
    QUIT,
    PAUSED,
    CONTINUE
} input_state_enum;



extern "C"
{
    void set_renderer(SDL_Renderer *rend);

    void set_game_data_dir(const char *dir, const int len);

    input_state_enum process_ext_input_one_ev(SDL_Event event);

    SDL_Surface *get_cur_vid_surface();
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

    void enable_quick_start_mode();
}

const bool mQuickstart = true;

/**
 * @brief start Starts the Dreams engine which refers to accessing RefKeen code
 */
bool CosmosEngine::start()
{
    const GsRect<Uint16> gameRect(320, 200);
    gVideoDriver.setNativeResolution(gameRect);

    if(!mQuickstart)
    {
        mpScene.reset( new CosmosIntro );
    }
    else
    {
        enable_quick_start_mode();
    }

    const auto gameDir = GetFullFileName(mDataPath);

    set_game_data_dir(gameDir.c_str(),
                      gameDir.size());

    start_cosmo();

    mpScene.reset( new CosmoGameplay );
    mpScene->start();

    return true;
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
*/
    std::vector<SDL_Event> evVec;
    gInput.readSDLEventVec(evVec);

    for(SDL_Event &event : evVec)
    {
        process_ext_input_one_ev(event);
    }


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

    SDL_Surface *cur_sfc = get_cur_vid_surface();

    auto dst = gVideoDriver.getBlitSurface();
    SDL_BlitSurface(cur_sfc, NULL, dst, NULL);
}


}


StartCosmosEngine::StartCosmosEngine(const bool openedGamePlay,
                                     const int ep,
                                     const std::string &dataPath) :
    SwitchEngineEvent(
        new cosmos_engine::CosmosEngine(openedGamePlay, ep, dataPath) )
{ }
