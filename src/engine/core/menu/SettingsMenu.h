
#ifndef GALAXY_SETTINGSMENU_H_INCLUDED
#define GALAXY_SETTINGSMENU_H_INCLUDED

#include "GameMenu.h"

#include <string_view>

class SettingsMenu : public GameMenu
{
public:
    SettingsMenu(const enum Style &style, const std::string_view &engineName);
};


#endif /* GALAXY_SETTINGSMENU_H_INCLUDED */
