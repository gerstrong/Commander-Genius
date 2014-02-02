/*
 * CMainMenu.h
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "VorticonMenu.h"

#include <base/GsEvent.h>

#ifndef _VORTICON_MAINMENU_H_
#define _VORTICON_MAINMENU_H_

namespace vorticon
{

class MainMenu : public VorticonMenu
{
public:
    MainMenu(const bool openedGamePlay);

	void createVorticonMenu( const bool openedGamePlay );
};

struct OpenMainMenuEvent : public CEvent {};

}

#endif /* _VORTICON_MAINMENU_H_ */
