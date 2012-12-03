/*
 * CPhysicsSettings.h
 *
 *  Created on: 26.11.2009
 *      Author: gerstrong
 *
 *  This class has been created to fill with information
 *  about some physical values. Those values were defines before.
 *  Now they are constants. We might provide modding for those values
 *  This is Vorticon only but will be extended for galaxy also!!!
 */

#ifndef CPHYSICSSETTINGS_H_
#define CPHYSICSSETTINGS_H_

#include <SDL.h>
#include "fileio/TypeDefinitions.h"

class CPhysicsSettings {
public:
	CPhysicsSettings();

	/**
	 * \brief This function loads some constants, this normally happens when the game is loaded
	 * 		  and the values change depending on what is read for the mods
	 */	
	void loadGameConstants(int episode, byte* data);

	struct playersettings{
		int maxjumpspeed;
		int defaultjumpupdecreasespeed;
		int jumpdecrease_x;
		int max_x_speed;
		int maxpogospeed;
		int impossiblepogospeed;
		bool pogostickonstart;
		Uint16 start_with_shots;

		// pogo settings
		int pogoforce_x;
	}player;

	struct vorticonsettings{
		char commander_hp;
		char default_hp;
	}vorticon;

	struct{
		int visibility;
		int ctspace_ammo;
		int ctspace_keys;
		int one_eyed_tile;

		// Episode 1 only
		int joy_tile;
		int bat_tile;
		int vac_tile;
		int wsk_tile;

		int pogo_tile;
		int raygun_tile;

		// For the stoneblock normally in level 16
		int changestoneblock;
	}misc;

	int max_fallspeed;
	int fallspeed_increase;
};

#endif /* CPHYSICSSETTINGS_H_ */
