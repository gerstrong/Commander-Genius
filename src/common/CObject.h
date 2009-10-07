/*
 * CObject.h
 *
 *  Created on: 17.05.2009
 *      Author: gerstrong
 *
 *  This class handles the objects like sprites that are draw on the screen
 *  It also handles the AI if any.
 */

#ifndef COBJECT_H_
#define COBJECT_H_

#define MAX_PLAYERS            8

// structures for each AI module's data
#include "../ai/enemydata.h"

class CObject {
public:
	CObject();
	virtual ~CObject();

	 unsigned int type;        // yorp, vorticon, etc.
	 bool exists;
	 bool onscreen;    // 1=(scrx,scry) position is visible onscreen
	 bool hasbeenonscreen;
	 unsigned int sprite;      // which sprite should this object be drawn with
	 unsigned int x, y;        // x,y location in map coords, CSFed
	 int scrx, scry;           // x,y pixel position on screen

	 // if type is OBJ_PLAYER, this contains the player number that this object
	 // is associated with
	 int AssociatedWithPlayer;

	 // if zero, priority tiles will not be honored and object will always
	 // appear in front of the background
	 bool honorPriority;

	 bool canbezapped;         // if 0 ray will not stop on hitdetect
	 int zapped;              // number of times got hit by keen's raygun
	 int zapx, zapy, zapd;	   // x,y, and direction of last shot at time of impact
	 char zappedbyenemy;	   // if 1, it was an ENEMYRAY and not keen that shot it


	 char inhibitfall;         // if 1 common_enemy_ai will not do falling
	 char cansupportplayer[MAX_PLAYERS];

	 unsigned int blockedl, blockedr, blockedu, blockedd;
	 signed int xinertia, yinertia;
	 unsigned char xinertiatimer, yinertiatimer;

	 unsigned char touchPlayer;      // 1=hit detection with player
	 unsigned char touchedBy;        // which player was hit
	 // Y position on this object the hit was detected
	 // this is used for the yorps' bonk-on-the-head thing.
	 // objects are scanned bottom to top, and first pixel
	 // touching player is what goes in here.
	 unsigned char hity;

	 bool needinit;    // 1=new object--requires initilization
	 unsigned char wasoffscreen;  // set to 1 when object goes offscreen
	 bool dead;

	 // data for ai and such, used differently depending on
	 // what kind of object it is
	 union ai
	 {
			// ep1
			stYorpData yorp;
			stGargData garg;
			stVortData vort;
			stButlerData butler;
			stTankData tank;
			stRayData ray;
			stDoorData door;
			stIceChunk icechunk;
			stTeleportData teleport;
			stRopeData rope;

			// ep2
			stScrubData scrub;
			stPlatformData platform;
			stVortEliteData vortelite;
			stSEData se;
			stBabyData baby;

			// ep3
			stFoobData foob;
			stNinjaData ninja;
			stMeepData meep;
			stMotherData mother;
			stBallJackData bj;
			stNessieData nessie;

	 } ai;

private:


};

#endif /* COBJECT_H_ */
