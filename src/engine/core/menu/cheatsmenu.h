#ifndef CHEATSMENU_H
#define CHEATSMENU_H

#include "engine/core/menu/GameMenu.h"

#include "widgets/Switch.h"
#include <memory>

class CheatsMenu : public GameMenu
{
public:
    CheatsMenu(const Style style);

    void init();

    void refresh();

    void release();


private:


    std::shared_ptr<Switch>		mpJumpSwitch;
    std::shared_ptr<Switch>		mpGodSwitch;
    std::shared_ptr<Switch>		mpNoClipSwitch;
    std::shared_ptr<Switch>		mpItemsSwitch;
};


#endif // CHEATSMENU_H
