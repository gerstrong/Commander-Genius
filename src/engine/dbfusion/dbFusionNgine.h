#ifndef __DBFUSIONNGINE_H__
#define __DBFUSIONNGINE_H__

#include <base/GsEvent.h>
#include <base/GsEngine.h>
#include <base/utils/ThreadPool.h>
#include <string>

#ifdef DBFUSION

namespace dbfusion
{



class DBFusionEngine : public GsEngine
{
public:
    // Constructor in case you want to be dropped into shell and nothing else
    DBFusionEngine() {}

    // In case you want to directly start the DOS game
    DBFusionEngine(const std::string gamepath) : mGamePath(gamepath) {}


private:

    void start();
    void pumpEvent(const CEvent *evPtr);
    void ponder(const float deltaT);
    void render();

    std::unique_ptr<ThreadPoolItem> mp_Thread;

    const std::string mGamePath;
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
