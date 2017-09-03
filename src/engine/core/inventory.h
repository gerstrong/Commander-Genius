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
    unsigned long score = 0;
    unsigned long extralifeat = 0;
    unsigned int charges = 0;        // ray gun ammo
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


    void deserialize(boost::property_tree::ptree &node)
    {
        score = node.get<unsigned long>("score", 0);
        extralifeat = node.get<unsigned long>("extralifeat", 0);

        charges = node.get<unsigned int>("charges", 0);
        lives = node.get<signed char>("lives", 0);
        HasPogo = node.get<bool>("HasPogo", false);
        HasCardYellow = node.get<unsigned char>("HasCardYellow", 0);
        HasCardRed = node.get<unsigned char>("HasCardRed", 0);
        HasCardGreen = node.get<unsigned char>("HasCardGreen", 0);
        HasCardBlue = node.get<unsigned char>("HasCardBlue", 0);

        HasJoystick = node.get<bool>("HasJoystick", false);
        HasWiskey = node.get<bool>("HasWiskey", false);
        HasBattery = node.get<bool>("HasBattery", false);
        HasVacuum = node.get<bool>("HasVacuum", false);

        canlooseitem[0] = node.get<bool>("canlooseitem1", false);
        canlooseitem[1] = node.get<bool>("canlooseitem2", false);
        canlooseitem[2] = node.get<bool>("canlooseitem3", false);
        canlooseitem[3] = node.get<bool>("canlooseitem4", false);
    }

};

#endif
