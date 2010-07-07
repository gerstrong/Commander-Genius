/*
 * enemydata.h
 *
 *  Created on: 05.08.2009
 *      Author: gerstrong
 */

#ifndef ENEMYDATA_H_
#define ENEMYDATA_H_

#include "../../../keen.h"

// Structs used for different enemy's variables. These are in a union.

typedef struct stVortData
{
	unsigned char state;
	
	unsigned char timer,timer2;
	unsigned int animtimer;
	unsigned char frame;
	unsigned int dist_traveled;
	signed int inertiay;
	
	char hp;                // episode 1 style four-shots-to-kill
	
	unsigned char movedir;
	// these hold the animation frames indexes since they're
	// different for each episode
	int WalkLeftFrame;
	int WalkRightFrame;
	int LookFrame;
	int JumpRightFrame;
	int JumpLeftFrame;
	int DyingFrame;
	int DeadFrame;
} stVortData;


// Vorticon Elite = the purple guys that shoot you in ep2
typedef struct stVortEliteData
{
	unsigned char state;
	
	unsigned char timer,timer2;
	unsigned int animtimer;
	unsigned char frame;
	signed int inertiay;
	unsigned char movedir;
	unsigned int timesincefire;
	
	int dist_traveled;
} stVortEliteData;


// the rope that you cut to kill the vorticon at the end of ep1
typedef struct stRopeData
{
	char state;
	int droptimer;
	int stoneX, stoneY;
	int bgtile;
} stRopeData;

// vorticon mother
typedef struct stMotherData
{
	char state;
	direction_t dir;
	char hittimes;
	
	int animframe, animtimer;
	int timer;
} stMotherData;


#define NESSIETRAILLEN   5
typedef struct stNessieData
{
	char state;
	char leftrightdir, updowndir;
	unsigned int baseframe;

	unsigned int tiletrailX[NESSIETRAILLEN+1];
	unsigned int tiletrailY[NESSIETRAILLEN+1];
	int tiletrailhead;

	char animframe, animtimer;
	unsigned int destx, desty;
	
	unsigned int pausetimer;
	unsigned int pausex, pausey;
	
	unsigned int mortimer_swim_amt;
	bool mounted[MAX_PLAYERS];
} stNessieData;

#endif /* ENEMYDATA_H_ */
