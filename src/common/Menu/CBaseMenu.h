/*
 * CBaseMenu.h
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#ifndef CBASEMENU_H_
#define CBASEMENU_H_

const int NO_SELECTION = -1;

#include "dialog/CDialog.h"

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
	CBaseMenu(const Uint8 dlgTheme);

	// Processes the stuff that the menus have in common
	virtual void process();

	virtual ~CBaseMenu();
};

#endif /* CBASEMENU_H_ */
