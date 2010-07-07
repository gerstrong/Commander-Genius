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

typedef struct stIceChunk
{
	int vector_x, vector_y;
	int veloc_x, veloc_y;
	int timer;
} stIceChunk;


// the rope that you cut to kill the vorticon at the end of ep1
typedef struct stRopeData
{
	char state;
	int droptimer;
	int stoneX, stoneY;
	int bgtile;
} stRopeData;


typedef struct stScrubData
{
	unsigned char state;
	
	unsigned char animtimer, dietimer;
	unsigned char walkframe;
	signed int scrubdie_inertia_y;
	int fallinctimer,fallspeed;
	
	unsigned char walkdir;
	unsigned char kickedplayer[MAX_PLAYERS];
} stScrubData;


// a moving platform
typedef struct stPlatformData
{
	unsigned char state;
	unsigned char animframe;
	unsigned int animtimer;
	unsigned int waittimer;
	
	unsigned char movedir;
	unsigned char kickedplayer[MAX_PLAYERS];
} stPlatformData;


// many different assorted things, many sector-effectors affect tiles
// in the level instead of being a sprite. named for the similiarity
// to the object in duke3d
typedef struct stSEData
{
	unsigned int type;
	
	unsigned char state;
	unsigned int timer;
	unsigned int platx, platy;
	unsigned int dir;
	
	int counter,destroytiles;
	unsigned int frame;
	int mx,my;
	int blowx,blowy;
} stSEData;



typedef struct stBabyData
{
	char state;
	direction_t dir;
	signed int inertia_x, inertia_y;
	int jumpdectimer, xdectimer;
	int jumpdecrate;
	int dietimer;
	
	char walkframe;
	int walktimer;
} stBabyData;


// Ninja = the kung-fu bears in ep3
typedef struct stNinjaData
{
	char state;
	char dir;
	
	int animframe, animtimer;
	unsigned int timetillkick;
	
	signed int XInertia, YInertia;
	unsigned int XFrictionTimer, YFrictionTimer;
	unsigned int XFrictionRate, YFrictionRate;
	int KickMoveTimer;
	int isdying;
	int dietimer;
} stNinjaData;


// vorticon mother
typedef struct stMotherData
{
	char state;
	direction_t dir;
	char hittimes;
	
	int animframe, animtimer;
	int timer;
} stMotherData;



typedef struct stMeepData
{
	char state;
	char dir;
	
	int animframe, animtimer;
	int timer;
} stMeepData;


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
