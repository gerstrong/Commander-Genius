#include "GsTTFSystem.h"
#include <cstdio>

#if defined(USE_SDL_TTF)
#include <SDL_ttf.h>
#endif


GsTTFSystem::GsTTFSystem()
{

}

bool GsTTFSystem::init()
{    
    //Loading success flag
    bool success = true;

#if defined(USE_SDL_TTF)
    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

    mActive = true;
#endif

    return success;
}

void GsTTFSystem::cleanup()
{

#if defined(USE_SDL_TTF)
    if(mActive)
    {
        TTF_Quit();    
    }
#endif

}

