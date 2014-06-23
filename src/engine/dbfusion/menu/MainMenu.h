
#include "FusionMenu.h"


// Send the key used for the menu to the game.
// usually this ends up opening the ingame menu. (Esc Key Triggering)
struct BackButtonSendDosFusion : public CEvent {};


namespace dbfusion
{


class MainMenu : public FusionMenu
{

public:
    MainMenu();

};

}
