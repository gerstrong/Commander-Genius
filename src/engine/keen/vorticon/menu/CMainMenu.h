/*
 * CMainMenu.h
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "VorticonMenu.h"

#include <base/GsEvent.h>

#ifndef CMAINMENU_H_
#define CMAINMENU_H_

class CMainMenu : public VorticonMenu
{
public:
	CMainMenu(const bool openedGamePlay);

	void createVorticonMenu( const bool openedGamePlay );
	void createGalaxyMenu( const bool openedGamePlay );
};

struct OpenMainMenuEvent : public CEvent {};

#endif /* CMAINMENU_H_ */
