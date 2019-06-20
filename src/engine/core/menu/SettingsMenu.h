
#ifndef GALAXY_SETTINGSMENU_H_INCLUDED
#define GALAXY_SETTINGSMENU_H_INCLUDED

#include "GameMenu.h"

#include "engine/core/options.h"
#include "fileio/CExeFile.h"
#include "engine/core/CBehaviorEngine.h"


class SettingsMenu : public GameMenu
{
public:
    SettingsMenu(const enum Style &style);
};


struct OpenVGamePadSettingsEvent : public CEvent {};


#endif /* GALAXY_SETTINGSMENU_H_INCLUDED */
