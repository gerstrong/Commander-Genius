#ifndef __DBFUSIONNGINE_H__
#define __DBFUSIONNGINE_H__

#include <base/GsEvent.h>
#include <base/GsEngine.h>
#include <base/utils/ThreadPool.h>
#include <graphics/GsSurface.h>
#include <string>

#ifdef DBFUSION

extern bool dosMachinePause;

namespace dbfusion
{


struct OpenMainMenuEvent : public CEvent {};

// Pauses the Dos Fusion
struct PauseDosFusion : public CEvent {};


class DBFusionEngine : public GsEngine
{
public:
    // Constructor in case you want to be dropped into shell and nothing else
    DBFusionEngine() : mMenuLocked(false) {}

    // In case you want to directly start the DOS game
    DBFusionEngine(const std::string gamepath) : mGamePath(gamepath) {}


private:

    void start();
    void pumpEvent(const CEvent *evPtr);
    void ponder(const float deltaT);
    void render();

    std::unique_ptr<ThreadPoolItem> mp_Thread;

    const std::string mGamePath;

    // Used, when the menu is open. After it is closed you might want to get the last frame.
    GsSurface mBackbuffer;

    bool mMenuLocked;
};


}

// It's a simple start event which will force CG launch a game
struct GMDBFusionStart : CEvent {};

// This will start a game choosen from the list directly
struct GMDosGameFusionStart : CEvent {};

// Events
struct StartDBFusionEngine : public SwitchEngineEvent
{
    StartDBFusionEngine() :
        SwitchEngineEvent( new dbfusion::DBFusionEngine() )
        { }

    StartDBFusionEngine(const std::string &dataDir) :
        SwitchEngineEvent( new dbfusion::DBFusionEngine(dataDir) )
        { }

};

#endif

#endif /* __DBFUSIONNGINE_H__ */
