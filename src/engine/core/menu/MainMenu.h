/*
 * CMainMenu.h
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "GameMenu.h"

#include <base/GsEvent.h>

#include <string_view>

#ifndef MAINMENU_H_INCLUDED
#define MAINMENU_H_INCLUDED


class MainMenu : public GameMenu
{
public:
    MainMenu(const bool openedGamePlay,
             const Style style,
             const bool singlePlayer,
             const std::string_view &engineName);

    void createGameMenu(const bool openedGamePlay,
                        const Style style,
                        const bool singlePlayer,
                        const std::string_view &engineName);

    virtual void ponder(const float dt) override;
};

struct OpenMainMenuEvent : public CEvent {};

#endif /* MAINMENU_H_INCLUDED */
