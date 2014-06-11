#ifndef __DBFUSIONNGINE_H__
#define __DBFUSIONNGINE_H__

#include <base/GsEvent.h>
#include <base/GsEngine.h>
#include <base/utils/ThreadPool.h>

#ifdef DBFUSION

namespace dbfusion
{



class DBFusionEngine : public GsEngine
{
    void start();
    void pumpEvent(const CEvent *evPtr);
    void ponder(const float deltaT);
    void render();

private:
    std::unique_ptr<ThreadPoolItem> mp_Thread;
};


}

// It's a simple start event which will force CG launch a game
struct GMDBFusionStart : CEvent {};

// This will start a game choosen from the list directly
struct GMDosFusionStart : CEvent {};

// Events
struct StartDBFusionEngine : public SwitchEngineEvent
{
    StartDBFusionEngine() :
        SwitchEngineEvent( new dbfusion::DBFusionEngine() )
        { }
};

#endif

#endif /* __DBFUSIONNGINE_H__ */
