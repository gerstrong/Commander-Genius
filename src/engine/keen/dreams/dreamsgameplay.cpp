#include "dreamsgameplay.h"


void GameLoopOpen();
void GamePlayStart();

extern "C"
{
void 	PlayLoop();
void 	PlayLoopRender();
}

void PlayLoopInit();

namespace dreams
{



DreamsGamePlay::DreamsGamePlay()
{

}


void DreamsGamePlay::start()
{
    GamePlayStart();
}

void DreamsGamePlay::pumpEvent(const CEvent *evPtr)
{
    if( dynamic_cast<const GoIntoPlayLoop*>(evPtr) )
    {
        PlayLoopInit();

        mPlayloopPtr = &PlayLoop;
        mPlayloopRenderPtr = &PlayLoopRender;
    }
}

void DreamsGamePlay::ponder(const float deltaT)
{
    if(mPlayloopPtr)
    {
        (*mPlayloopPtr)();
    }
    else
    {
        GameLoopOpen();
    }
}

void DreamsGamePlay::render()
{
    if(mPlayloopRenderPtr)
    {
        (*mPlayloopRenderPtr)();
    }
}

}
