/*
 * Inventory structure.
 * Simple structure which store the stuff the Player has.
 * Some classes use it as pointer to get access to the data
 */

#ifndef __inventory_h__
#define __inventory_h__

struct stInventory {
	unsigned long score;
	unsigned long extralifeat;
	unsigned int charges;        // ray gun ammo
	signed char lives;
	unsigned char HasPogo;
	unsigned char HasCardYellow;
	unsigned char HasCardRed;
	unsigned char HasCardGreen;
	unsigned char HasCardBlue;
	
	// ep1 normally
	bool HasJoystick;
	bool HasWiskey;
	bool HasBattery;
	bool HasVacuum;
};

#endif
