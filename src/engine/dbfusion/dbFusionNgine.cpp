#include "dbFusionNgine.h"
#include "engine/CGameLauncher.h"

namespace dbfusion
{


void DBFusionEngine::start()
{

}

void DBFusionEngine::pumpEvent(const CEvent *evPtr)
{

}

void DBFusionEngine::ponder(const float deltaT)
{
    gEventManager.add(new GMSwitchToGameLauncher);
}

void DBFusionEngine::render()
{

}


}
