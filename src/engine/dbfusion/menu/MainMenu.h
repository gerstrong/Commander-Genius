
#include "FusionMenu.h"

#include <base/utils/ThreadPool.h>

void MAPPER_RunInternal();

// Send the key used for the menu to the game.
// usually this ends up opening the ingame menu. (Esc Key Triggering)
struct BackButtonSendDosFusion : public CEvent {};

namespace dbfusion
{

int mapperRun(void*);


struct ExecuteMappperEvent : CEvent
{
    virtual void operator()() const
    {
        threadPool->start(mapperRun, nullptr, "DosBoxMain");
    }
};


class MainMenu : public FusionMenu
{

public:
    MainMenu();

};

}
