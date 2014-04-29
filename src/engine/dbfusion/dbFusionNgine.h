#ifndef __DBFUSIONNGINE_H__
#define __DBFUSIONNGINE_H__

#include <base/GsEvent.h>
#include <base/GsEngine.h>

namespace dbfusion
{

class DBFusionEngine : public GsEngine
{
    void start();
    void pumpEvent(const CEvent *evPtr);
    void ponder(const float deltaT);
    void render();
};

}

// It's a simple start event which will force CG launch a game
struct GMDBFusionStart : CEvent {};

// Events
struct StartDBFusionEngine : public SwitchEngineEvent
{
    StartDBFusionEngine() :
        SwitchEngineEvent( new dbfusion::DBFusionEngine() )
        { }
};

#endif /* __DBFUSIONNGINE_H__ */
