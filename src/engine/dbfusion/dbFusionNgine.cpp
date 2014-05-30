#include "dbFusionNgine.h"
#include "engine/CGameLauncher.h"

int dosbox_main(int argc, const char* argv[]);

namespace dbfusion
{


void DBFusionEngine::start()
{
    const int argc = 1;
    const char* argv[1] = { "dosbox" };

    dosbox_main(argc, argv);
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
