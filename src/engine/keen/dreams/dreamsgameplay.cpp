#include "dreamsgameplay.h"

#include <base/CInput.h>

void GameLoopOpen();
void GamePlayStart();
void GamePlayStartLevel();

void processLevelcomplete();

extern "C"
{
void 	PlayLoop();
void 	PlayLoopRender();


extern bool ResumeGame;

}

void PlayLoopInit();

namespace dreams
{



DreamsGamePlay::DreamsGamePlay()
{

}

void startLevel();



void DreamsGamePlay::start()
{
    if(!ResumeGame)
    {
        GamePlayStart();
        GamePlayStartLevel();                
    }    
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
        GamePlayStartLevel();
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
