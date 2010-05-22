/*
 * CBaseMenu.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CBASEMENU_H_
#define CBASEMENU_H_

const int NO_SELECTION = -1;

#include "../../dialog/CDialog.h"

// Which Menu has to be shown?

enum menutypes{
	MAIN, NEW, OVERWRITE,
	CONTROLPLAYERS, STORY,
	HIGHSCORES, ABOUTCG,
	ABOUTID, ORDERING,
	LOAD, SAVE, START,
	DIFFICULTY, CONFIGURE,
	GRAPHICS, CONTROLS,
	AUDIO, OPTIONS,
	F1, HELP, ENDGAME, QUIT,
	BOUNDS, VOLUME, MENU_DEBUG,
	MODCONF
};

const unsigned int NUM_MENUS = MODCONF+1;

// Active means, when the player is playing, PASSIVE when the Player is not playing
enum menumodes{
	ACTIVE, PASSIVE
};

class CBaseMenu {
public:
	CBaseMenu(Uint8 dlg_theme);

	// Processes the stuff that the menus have in common
	void processCommon();
	// Processes the stuff that are specific for the derived menu
	virtual void processSpecific() {}
	// Process more common stuff, like drawing or reading input of mp_Dialog
	void postProcess();

	bool mustClose();

	virtual ~CBaseMenu();

protected:
	bool m_mustclose;
	int m_selection;
	CDialog *mp_Dialog;
	Uint8 m_dlg_theme;
	bool m_suspended;
};

#endif /* CBASEMENU_H_ */
