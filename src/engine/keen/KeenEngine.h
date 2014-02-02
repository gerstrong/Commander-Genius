#ifndef __KEENENGINE_H__
#define __KEENENGINE_H__

#include "core/GameEngine.h"
#include "common/CBehaviorEngine.h"

class KeenEngine : public GameEngine
{
public:

    KeenEngine(const bool openedGamePlay,
               const int ep,
               const std::string &dataPath) :
        GameEngine(openedGamePlay, dataPath),
        mEp(ep),
        mLockedMenu(true) {}

    void switchToGamePlayMode();

    virtual void openMainMenu();

    void start();

    virtual void pumpEvent(const CEvent *evPtr);

    void ponder(const float deltaT);

    virtual bool loadResources( const Uint8 flags ) = 0;

protected:


    enum load_states
    {
     LOADGFX=0x01,
     LOADSTR=0x02,
     LOADSND=0x04,
     LOADALL=0xFF
    };

    const int mEp;
    bool mLockedMenu;
};


struct FinishedLoadingResources : CEvent {};

// Invoked when New Game -> Players selected -> Difficulty selected
struct StartNewGameEvent : CEvent
{
    StartNewGameEvent(const Difficulty difficulty) :
        mDifficulty(difficulty) {}

    const Difficulty mDifficulty;
};



#endif /* __KEENENGINE_H__ */
