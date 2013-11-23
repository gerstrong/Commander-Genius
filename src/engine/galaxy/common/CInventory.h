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
#include "engine/galaxy/res/ItemsGalaxy.h"
#include "CStatusScreenGalaxy.h"
#include "fileio/CSaveGameController.h"
#include <memory>
#include <string>
#include <SDL.h>

class CInventory
{
public:
    CInventory(const int id = 0);
	void reset();

	// This function draws the HUD on the Screen
    void drawHUD();

    void setup(const int sprVar);

	// This one draw the Status screen whenever it's called
	void drawStatus();

	// return if the status screen has to be drawn
	bool showStatus()
	{	return mp_StatusScreen->m_showstatus;	}

    // If a player dies forever and carried and important item, we might
    // want to pass those important item to another one
    // so the game can be continued.
    void fetchImportantStuff(const CInventory &other);

	// make the status get opened or closed
	void toggleStatusScreen();

	// Saves the inventory using the Savegamecontroller.
	void operator>>(CSaveGameController &savedGame);
    void operator>>(boost::property_tree::ptree &invNode);

	// This is for loading the game
	void operator<<(CSaveGameController &savedGame);
    void operator<<(boost::property_tree::ptree &invNode);

	stItemGalaxy Item;

	CHUD m_HUD;
	std::shared_ptr<CStatusScreenGalaxy> mp_StatusScreen;
	SDL_Surface *mp_StatusBgrnd;
};


#endif /* CINVENTORY_H_ */
