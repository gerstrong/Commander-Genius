/*
 * CMainMenu.h
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "GameMenu.h"

#include <base/GsEvent.h>

#ifndef MAINMENU_H_INCLUDED
#define MAINMENU_H_INCLUDED


class MainMenu : public GameMenu
{
public:
    MainMenu(const bool openedGamePlay,
             const Style style,
             const bool singlePlayer);

    void createGameMenu(const bool openedGamePlay,
                        const Style style,
                        const bool singlePlayer);

    virtual void ponder(const float dt) override;
};

struct OpenMainMenuEvent : public CEvent {};

#endif /* MAINMENU_H_INCLUDED */
