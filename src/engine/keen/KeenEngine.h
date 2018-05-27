#ifndef __KEENENGINE_H__
#define __KEENENGINE_H__

#include "engine/core/GameEngine.h"
#include "engine/core/CBehaviorEngine.h"

class KeenEngine : public GameEngine
{
public:

    KeenEngine(const bool openedGamePlay,
               const int ep,
               const std::string &dataPath) :
        GameEngine(openedGamePlay, dataPath),
        mEp(ep),
        mLockedMenu(true) {}

    void switchToGamePlayMode(const int startLevel = -1);

    virtual void openMainMenu() = 0;

    bool start() override;

    virtual void pumpEvent(const CEvent *evPtr) override;

    virtual void ponder(const float deltaT) override;

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

// Event pushed when all the resources have been loaded
struct FinishedLoadingResources : CEvent {};

// Invoked when New Game -> Players selected -> Difficulty selected
struct StartNewGameEvent : CEvent
{
    StartNewGameEvent(const Difficulty difficulty,
                      const int startLevel = -1) :
        mDifficulty(difficulty),
        mStartLevel(startLevel) {}

    const Difficulty mDifficulty = EASY;
    const int mStartLevel = -1;
};


// Invoked when New Game -> 1 Player-> Sprite Number
struct SelectPlayerSpriteVarEvent : CEvent
{
    SelectPlayerSpriteVarEvent(const int special) :
        mSpecialIdx(special) {}

    int mSpecialIdx = 0;
};



#endif /* __KEENENGINE_H__ */
