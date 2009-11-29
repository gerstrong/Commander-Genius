/*
 * CPhysicsSettings.cpp
 *
 *  Created on: 26.11.2009
 *      Author: gerstrong
 */

#include "CPhysicsSettings.h"

CPhysicsSettings::CPhysicsSettings() {
	// used for jumping
	player.maxjumpspeed = 103 ;
	player.defaultjumpupdecreasespeed = 3;	// TODO Auto-generated constructor stub
	player.jumpdecrease_x = 60;

	player.max_x_speed_air = 90;
	player.max_x_speed_ground = 70;

	// when falling keen's Y inertia increases up to maximum
	player.max_fallspeed = 103;
	player.fallspeed_decrease = 1;
}

CPhysicsSettings::~CPhysicsSettings() {
	// TODO Auto-generated destructor stub
}
