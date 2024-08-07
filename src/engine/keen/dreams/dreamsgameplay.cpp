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
extern bool	ingame;
extern bool mGamePlayRunning;

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



void DreamsGamePlay::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    if( std::dynamic_pointer_cast<const GoIntoPlayLoop>(evPtr) )
    {
        PlayLoopInit();

        mPlayloopPtr = &PlayLoop;
        mPlayloopRenderPtr = &PlayLoopRender;
        gInput.flushAll();
    }
    if( std::dynamic_pointer_cast<const CompleteLevel>(evPtr) )
    {
        processLevelcomplete();
        mPlayloopPtr = nullptr;
        mPlayloopRenderPtr = nullptr;
        GamePlayStartLevel();
        gInput.flushAll();
    }
    if( std::dynamic_pointer_cast<const RestartGame>(evPtr) )
    {
        processLevelcomplete();
        mPlayloopPtr = nullptr;
        mPlayloopRenderPtr = nullptr;
        ingame = false;
        mGamePlayRunning = false;
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
