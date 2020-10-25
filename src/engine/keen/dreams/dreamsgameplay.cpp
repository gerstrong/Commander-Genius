#include "dreamsgameplay.h"
#include "dreamsintro.h"

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



bool DreamsGamePlay::start()
{
    if(!ResumeGame)
    {
        GamePlayStart();
        GamePlayStartLevel();                
    }    

    return true;
}



void DreamsGamePlay::pumpEvent(const CEvent *evPtr)
{
    if( dynamic_cast<const GoIntoPlayLoop*>(evPtr) )
    {
        PlayLoopInit();

        mPlayloopPtr = &PlayLoop;
        mPlayloopRenderPtr = &PlayLoopRender;        
        gInput.flushAll();
    }
    if( dynamic_cast<const CompleteLevel*>(evPtr) )
    {
        processLevelcomplete();
        mPlayloopPtr = nullptr;
        mPlayloopRenderPtr = nullptr;        
        GamePlayStartLevel();
        gInput.flushAll();
    }
    if( dynamic_cast<const RestartGame*>(evPtr) )
    {
        processLevelcomplete();
        mPlayloopPtr = nullptr;
        mPlayloopRenderPtr = nullptr;
        gEventManager.add( new SwitchToIntro );
        gInput.flushAll();
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
