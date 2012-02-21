/*
 * CSaveMenu.h
 *
 *  Created on: 21.05.2010
 *      Author: gerstrong
 */

#ifndef CSAVEMENU_H_
#define CSAVEMENU_H_

#include "CConfirmMenu.h"
#include "fileio/CSavedGame.h"

class CSaveMenu : public CBaseMenu
{
public:
	CSaveMenu(Uint8 dlg_theme, CSavedGame &SavedGame);
	void processSpecific();
	void saveSelection();

private:
	CConfirmMenu *mp_OverwriteMenu;
	CSavedGame &m_SavedGame;
	bool m_overwrite;
};

#endif /* CSAVEMENU_H_ */
