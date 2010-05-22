/*
 * CDifficultyMenu.h
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#ifndef CDIFFICULTYMENU_H_
#define CDIFFICULTYMENU_H_

#include "CBaseMenu.h"
#include "CPlayersMenu.h"

#include <SDL.h>

class CDifficultyMenu : public CBaseMenu
{
public:
	CDifficultyMenu( Sint8 &difficulty, Uint8 &players, Uint8 dlg_theme );
	void processSpecific();
	virtual ~CDifficultyMenu();

private:
	CPlayersMenu *mp_PlayersMenu;

	Sint8 &m_difficulty;
	Uint8 &m_players;
};

#endif /* CDIFFICULTYMENU_H_ */
