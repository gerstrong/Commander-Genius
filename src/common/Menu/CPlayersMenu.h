/*
 * CPlayersMenu.h
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#ifndef CPLAYERSMENU_H_
#define CPLAYERSMENU_H_

#include "CBaseMenu.h"
#include <SDL.h>

class CPlayersMenu : public CBaseMenu
{
public:
	CPlayersMenu(Uint8 &players, Uint8 dlg_theme);
	void processSpecific();
	virtual ~CPlayersMenu();

private:
	Uint8 &m_players;
};

#endif /* CPLAYERSMENU_H_ */
