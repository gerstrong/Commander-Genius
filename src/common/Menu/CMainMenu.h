/*
 * CMainMenu.h
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include <widgets/CBaseMenu.h>

#include "engine/CEvent.h"

#ifndef CMAINMENU_H_
#define CMAINMENU_H_

class CMainMenu : public CBaseMenu
{
public:
	CMainMenu(const bool openedGamePlay);

	void createVorticonMenu( const bool openedGamePlay );
	void createGalaxyMenu( const bool openedGamePlay );
};


#endif /* CMAINMENU_H_ */
