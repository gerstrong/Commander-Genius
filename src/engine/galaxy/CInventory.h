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
#include "CStatusScreenGalaxy.h"

class CInventory {
public:
	CInventory();
	void reset();

	// This function draws the HUD on the Screen
	void drawHUD();

	// This one draw the Status screen whenever it's called
	void drawStatus();

	// return if the status screen has to be drawn
	bool showStatus()
	{	return m_StatusScreen.m_showstatus;	}

	// make the status get opened or closed
	void toggleStatusScreen()
	{	m_StatusScreen.m_showstatus = !m_StatusScreen.m_showstatus;	}

	signed char m_lifes;
	unsigned long  m_points;
	int m_lifeAt;
	int m_drops;
	unsigned int m_bullets;
	struct {
		int red;
		int blue;
		int green;
		int yellow;
	} m_gem;

	int m_keycards;

	union {
		struct
		{
			bool swimsuit;
			int elders;
		}ep4;

		struct
		{
			bool sandwich;
			bool hook;
			bool rocketKeycard;
		}ep6;
	}m_special;

	CHUD m_HUD;
	CStatusScreenGalaxy m_StatusScreen;
};


#endif /* CINVENTORY_H_ */
