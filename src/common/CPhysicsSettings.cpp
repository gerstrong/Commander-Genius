/*
 * CPhysicsSettings.cpp
 *
 *  Created on: 26.11.2009
 *      Author: gerstrong
 */

#include "CPhysicsSettings.h"

CPhysicsSettings::CPhysicsSettings() {
	// used for jumping
	player.maxjumpspeed = 120;

	player.maxpogospeed = 165;
	player.impossiblepogospeed = 175;

	player.defaultjumpupdecreasespeed = 4;
	player.jumpdecrease_x = 64;

	//player.max_x_speed = 68;
	player.max_x_speed = 75;
	player.pogostickonstart = false;

	// when falling keen's Y inertia increases up to maximum
	max_fallspeed = 105;
	fallspeed_increase = 4;

	// Pogo values
	player.pogoforce_x = 30;

	misc.visibility = 3;
}

/**
 * \brief This function loads some constants, this normally happens when the game is loaded
 * 		  and the values change depending on what is read for the mods
 */
void CPhysicsSettings::loadGameConstants(int episode, byte* data)
{
	if(episode == 1)
	{	// Code for Episode 1.

		byte pogo = *(data+0x900E);
		player.pogostickonstart = pogo;
		player.start_with_shots = *(data+0x9008);
		vorticon.default_hp = *(data+0x180D)+1;
		vorticon.commander_hp = *(data+0x1819)+1;
		// TODO: ... More stuff will come here
	}
	else if(episode == 2)
	{	// Code for Episode 2.
		player.pogostickonstart = true;
		player.start_with_shots = *(data+0x8AA2);
		// TODO: ... More stuff will come here
	}
	else if(episode == 3)
	{	// Code for Episode 3.
		player.pogostickonstart = true;
		player.start_with_shots = 5;
		// TODO: ... More stuff will come here
	}
}
