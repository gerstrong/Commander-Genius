/*
 * CBaseMenu.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CBASEMENU_H_
#define CBASEMENU_H_

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
	AUDIO, OPTIONS, BOUNDS,
	F1, HELP, ENDGAME, QUIT
};

// Active means, when the player is playing, PASSIVE when the Player is not playing
enum menumodes{
	ACTIVE, PASSIVE
};

class CBaseMenu {
public:
	CBaseMenu(char &MenuType);

	// Processes the stuff that the menus have in common
	void processCommon();

	// Processes the stuff that are specific for the derived menu
	virtual void processSpecific() {}

	// Process more common stuff, like drawing or reading input of mp_Dialog
	void postProcess();

	bool mustClose() { return m_mustclose; }
	bool restartVideo() { return m_restartVideo; }

	virtual ~CBaseMenu();

protected:
	bool m_mustclose;
	char &m_MenuType;
	int m_selection;
	CDialog *mp_Dialog;
	bool m_restartVideo;
};

#endif /* CBASEMENU_H_ */
