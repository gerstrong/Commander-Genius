#ifndef CHEATSMENU_H
#define CHEATSMENU_H

#include "engine/core/menu/GameMenu.h"

#include "widgets/Switch.h"


class CheatsMenu : public GameMenu
{
public:
    CheatsMenu(const GsControl::Style style);

    void init();

    void refresh();

    void release();


private:

    Switch		*mpJumpSwitch;
    Switch		*mpGodSwitch;
    Switch		*mpNoClipSwitch;
    Switch		*mpItemsSwitch;
};


#endif // CHEATSMENU_H
