/*
 * Inventory structure.
 * Simple structure which store the stuff the Player has.
 * Some classes use it as pointer to get access to the data
 */

#ifndef __inventory_h__
#define __inventory_h__

#include <boost/property_tree/ptree.hpp>

struct stInventory
{
	unsigned long score;
	unsigned long extralifeat;
	unsigned int charges;        // ray gun ammo
	signed char lives;
	bool HasPogo;
	unsigned char HasCardYellow;
	unsigned char HasCardRed;
	unsigned char HasCardGreen;
	unsigned char HasCardBlue;
	
	// ep1 normally
	bool HasJoystick;
	bool HasWiskey;
	bool HasBattery;
	bool HasVacuum;

	bool canlooseitem[4];

    void serialize(boost::property_tree::ptree &node)
    {
        node.put("score", score);
        node.put("extralifeat", extralifeat);

        node.put("charges", charges);
        node.put("lives", lives);
        node.put("HasPogo", HasPogo);
        node.put("HasCardYellow", HasCardYellow);
        node.put("HasCardRed", HasCardRed);
        node.put("HasCardGreen", HasCardGreen);
        node.put("HasCardBlue", HasCardBlue);

        node.put("HasJoystick",HasJoystick);
        node.put("HasWiskey",HasWiskey);
        node.put("HasBattery",HasBattery);
        node.put("HasVacuum",HasVacuum);

        node.put("canlooseitem1", canlooseitem[0]);
        node.put("canlooseitem2", canlooseitem[1]);
        node.put("canlooseitem3", canlooseitem[2]);
        node.put("canlooseitem4", canlooseitem[3]);
    }
};

#endif
