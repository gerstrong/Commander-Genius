#include "GsTTFSystem.h"
#include <cstdio>

#include <SDL_ttf.h>


GsTTFSystem::GsTTFSystem()
{

}

bool GsTTFSystem::init()
{    
    //Loading success flag
    bool success = true;

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }

    mActive = true;

    return success;
}

void GsTTFSystem::cleanup()
{

    if(mActive)
    {
        TTF_Quit();    
    }

}

