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
    MainMenu(const bool openedGamePlay, const GsControl::Style style);

    void createGameMenu(const bool openedGamePlay , const GsControl::Style style);
};

struct OpenMainMenuEvent : public CEvent {};

#endif /* MAINMENU_H_INCLUDED */
