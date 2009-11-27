/*
 * CPhysicsSettings.cpp
 *
 *  Created on: 26.11.2009
 *      Author: gerstrong
 */

#include "CPhysicsSettings.h"

CPhysicsSettings::CPhysicsSettings() {
	player.maxjumpspeed = 103;
	player.defaultdecreasespeed = 3;	// TODO Auto-generated constructor stub

	// when falling keen's Y inertia increases up to maximum
	player.max_fallspeed = 60;
	player.fallspeed_decrease = 1;
}

CPhysicsSettings::~CPhysicsSettings() {
	// TODO Auto-generated destructor stub
}
