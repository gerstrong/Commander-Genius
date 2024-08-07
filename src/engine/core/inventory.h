/*
 * Inventory structure.
 * Simple structure which store the stuff the Player has.
 * Some classes use it as pointer to get access to the data
 */

#ifndef inventory_h
#define inventory_h


#include <base/interface/property_tree/property_tree.h>

struct stInventory
{
    unsigned long score = 0;
    unsigned long extralifeat = 0;
    int charges = 0;        // ray gun ammo
    signed char lives = 0;
    bool HasPogo = false;
    unsigned char HasCardYellow = 0;
    unsigned char HasCardRed = 0;
    unsigned char HasCardGreen = 0;
    unsigned char HasCardBlue = 0;
	
	// ep1 normally
    bool HasJoystick = false;
    bool HasWiskey = false;
    bool HasBattery = false;
    bool HasVacuum = false;

    bool canlooseitem[4] = {0,0,0,0};

    void serialize(GsKit::ptree &node);

    void deserialize(GsKit::ptree &node);

};

#endif /* inventory_h */
