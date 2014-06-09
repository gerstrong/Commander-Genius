#include "dbFusionNgine.h"
#include "engine/CGameLauncher.h"

#include <base/video/CVideoDriver.h>

int dosbox_main(int argc, const char* argv[]);

namespace dbfusion
{


int mainDosbox(void*)
{
    const int argc = 1;
    const char* argv[1] = { "dosbox" };

    dosbox_main(argc, argv);

    return 0;
}

void DBFusionEngine::start()
{
    const GsRect<Uint16> dosRect(640,400);
    gVideoDriver.setNativeResolution(dosRect);


    mp_Thread.reset(threadPool->start(mainDosbox, nullptr, "DosBoxMain"));        
}

void DBFusionEngine::pumpEvent(const CEvent *evPtr)
{

}

void DBFusionEngine::ponder(const float deltaT)
{
    if(mp_Thread->finished)
    {
        gEventManager.add(new GMSwitchToGameLauncher);
    }
}

void DBFusionEngine::render()
{

}


}
