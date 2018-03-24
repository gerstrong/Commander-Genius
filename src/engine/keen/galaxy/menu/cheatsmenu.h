#ifndef CHEATSMENU_H
#define CHEATSMENU_H

#include "GalaxyMenu.h"

#include "widgets/Switch.h"

namespace galaxy
{



class CheatsMenu : public GalaxyMenu
{
public:
    CheatsMenu();

    void init();

    void refresh();

    void release();


private:

    Switch		*mpJumpSwitch;
    Switch		*mpGodSwitch;
    Switch		*mpNoClipSwitch;
    Switch		*mpItemsSwitch;
};

}

#endif // CHEATSMENU_H
