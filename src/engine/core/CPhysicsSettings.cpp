/*
 * CPhysicsSettings.cpp
 *
 *  Created on: 26.11.2009
 *      Author: gerstrong
 */

#include "CPhysicsSettings.h"
#include <base/GsTimer.h>

CPhysicsSettings::CPhysicsSettings()
{
	// used for jumping
	player.maxjumpspeed = 120;
	player.defaultjumpupdecreasespeed = 4;

	player.jumpdecrease_x = 64;

	player.maxpogospeed = 165;
	player.impossiblepogospeed = 175;

	player.max_x_speed = 68;
	player.pogostickonstart = false;

	// Pogo values
	player.pogoforce_x = 30;

	misc.ctspace_ammo = 100;
	misc.ctspace_keys = 1;
}



void CPhysicsSettings::loadGameConstants(int episode, byte* data)
{

	if(episode >= 4 && episode <= 6 )
	{
		// Galaxy Engine
		fallspeed_increase = 7;
		max_fallspeed = 175; // Factor 2.5 to the original assembly it is
		misc.visibility = 7;
	}
	else
	{
		// Vorticon Engine
		misc.visibility = 3;
		fallspeed_increase = 4;
		max_fallspeed = 104;

		if(episode == 1)
		{	// Code for Episode 1.

			// player
			byte pogo = *(data+0x900E);
			player.pogostickonstart = pogo;
			player.start_with_shots = *(data+0x9008);

			// vorticon
			vorticon.default_hp = *(data+0x180D)+1;
			vorticon.commander_hp = *(data+0x1819)+1;

			misc.ctspace_ammo = GETWORD(data+0xE03);
			misc.ctspace_keys = GETWORD(data+0xE11);
			misc.one_eyed_tile = GETWORD(data+0x4547);

			misc.joy_tile = GETWORD(data+0x992A);
			misc.bat_tile = GETWORD(data+0x995D);
			misc.vac_tile = GETWORD(data+0x9990);
			misc.wsk_tile = GETWORD(data+0x99C3);

			misc.raygun_tile = GETWORD(data+0x1166);
			misc.pogo_tile = GETWORD(data+0x11A7);
			misc.changestoneblock = GETWORD(data+0x2414);
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
}
