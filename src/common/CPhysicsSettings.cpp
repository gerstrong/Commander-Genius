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

	player.max_x_speed = 68;

	// when falling keen's Y inertia increases up to maximum
	player.max_fallspeed = 105;
	player.fallspeed_decrease = 5;

	// Pogo values
	player.pogoforce_x = 30;

	misc.visibility = 3;
}

CPhysicsSettings::~CPhysicsSettings() {
	// TODO Auto-generated destructor stub
}
