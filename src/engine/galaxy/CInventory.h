/*
 * CInventory.h
 *
 *  Created on: 25.11.2010
 *      Author: gerstrong
 *
 *  The new Galaxy Inventory. We have another class here, because
 *  the one for vorticon is memory fixed for savegames.
 */


#ifndef CINVENTORY_H_
#define CINVENTORY_H_

#include "common/CHUD.h"
#include "ItemsGalaxy.h"
#include "CStatusScreenGalaxy.h"
#include "SmartPointer.h"
#include <string>
#include <SDL.h>

class CInventory {
public:
	CInventory(Uint8& difficulty, const std::string& levelname);
	void reset();

	// This function draws the HUD on the Screen
	void drawHUD();

	// This one draw the Status screen whenever it's called
	void drawStatus();

	// return if the status screen has to be drawn
	bool showStatus()
	{	return mp_StatusScreen->m_showstatus;	}

	// make the status get opened or closed
	void toggleStatusScreen();

	stItemGalaxy Item;

	CHUD m_HUD;
	SmartPointer<CStatusScreenGalaxy> mp_StatusScreen;
	SDL_Surface *mp_StatusBgrnd;
	const std::string &m_LevelName;
};


#endif /* CINVENTORY_H_ */
