#include "dreamsgameplay.h"

#include <base/CInput.h>

void GameLoopOpen();
void GamePlayStart();

void processLevelcomplete();

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
    if( dynamic_cast<const CompleteLevel*>(evPtr) )
    {
        processLevelcomplete();
        mPlayloopPtr = nullptr;
        mPlayloopRenderPtr = nullptr;
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
        gInput.flushAll();
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
