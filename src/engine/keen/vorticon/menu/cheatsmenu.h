#ifndef CHEATSMENU_H
#define CHEATSMENU_H

#include "VorticonMenu.h"

#include "widgets/Switch.h"

namespace vorticon
{



class CheatsMenu : public VorticonMenu
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
