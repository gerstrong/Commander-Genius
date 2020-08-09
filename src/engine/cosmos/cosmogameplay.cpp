#include "cosmogameplay.h"

#include <SDL_events.h>



extern "C"
{
    int run_gameplay();
    void load_current_level();
    bool executeLogics();
    void video_update();

}

CosmoGameplay::CosmoGameplay()
{

}


bool CosmoGameplay::start()
{
    load_current_level();
    return true;
}

void CosmoGameplay::pumpEvent(const CEvent *evPtr)
{

}



void CosmoGameplay::ponder(const float deltaT)
{
    /*
    int input_state = process_ext_input();

    if (input_state == QUIT)
    {
        //return false;
        return;
    }

    if (input_state == PAUSED)
    {
        //return true;
        return;
    }
    */

    executeLogics();
    //run_gameplay();
}

void CosmoGameplay::render()
{
    //video_update();
}
