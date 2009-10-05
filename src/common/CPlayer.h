/*
 * CPlayer.h
 *
 *  Created on: 05.10.2009
 *      Author: gerstrong
 */

#ifndef CPLAYER_H_
#define CPLAYER_H_

#define	PA_X		0
#define	PA_Y   		1
#define	PA_JUMP		2
#define	PA_POGO		3
#define	PA_FIRE		4
#define	PA_STATUS	5

#define	PA_MAX_ACTIONS 6

#define MAX_SPEED		7000
#define MAX_BOOST		200
#define TIME_DIVIDER	500		// For speed and acceleration

class CPlayer {
public:
	CPlayer();
	virtual ~CPlayer();

	// these coordinates are CSFed
	unsigned long x;
	unsigned int y;

	unsigned int w;
	unsigned int h;

	char isPlaying;
	int useObject;

	char godmode;

	// used on world map only
	char hideplayer;
	char mounted;

	short treshold;		// This is used for analog devices like joysticks
	signed int pinertia_y;

	unsigned long mapplayx;
	signed int mapplayy;

	unsigned char playframe;

	unsigned char pfalling,plastfalling,pfallspeed,pfallspeed_increasetimer;

	unsigned char pwalking,playspeed;
	unsigned char pslowingdown;
	unsigned char pwalkframe,pwalkframea,pwalkanimtimer;
	unsigned char pwalkincreasetimer, pfriction_timer_x, pfriction_timer_y;
	signed int pinertia_x,pboost_x,playpushed_x;
	int chargedjump;
	unsigned char playpushed_decreasetimer;
	bool widejump;

	bool blockedl,blockedr,blockedu,blockedd;
	unsigned int blockedby;

	unsigned char pjumping, pjumptime, pjumpupspeed_decreasetimer, pjumpdir;
	unsigned char pjumpframe, pjumpanimtimer, pjumpupspeed;
	unsigned char pjumpnormaltime, pjumpupdecreaserate, pjustjumped;
	unsigned char pjustfell;
	unsigned char pjumpfloattimer;

	unsigned char pdir,pshowdir,lastpdir;

	char pfiring,pfireframetimer;
	char inhibitwalking, inhibitfall;

	int ctrltimer, alttimer;
	char keyprocstate;
	char wm_lastenterstate;

	char pdie, pdieframe, pdietimer;
	int pdietillfly;
	signed int pdie_xvect;
	int psupportingtile, psupportingobject, lastsupportingobject;
	char psliding;
	char psemisliding;
	bool ppogostick;
	int pfrozentime,pfrozenframe,pfrozenanimtimer;

	unsigned char keytable[50];
	unsigned char lastkeytable[50];


	// New values
	char playcontrol[PA_MAX_ACTIONS];
	char lastplaycontrol[PA_MAX_ACTIONS];

	char x_friction;
	char y_friction;

	// End new values

	unsigned char dpadcount, dpadlastcount;

	unsigned int ankhtime, ankhshieldobject;

	struct stInventory
	{
	 unsigned long score;
	 unsigned long extralifeat;
	 unsigned int charges;        // ray gun ammo
	 signed char lives;
	 unsigned char HasPogo;
	 unsigned char HasCardYellow;
	 unsigned char HasCardRed;
	 unsigned char HasCardGreen;
	 unsigned char HasCardBlue;

	 // ep1 normally
	 bool HasJoystick;
	 bool HasWiskey;
	 bool HasBattery;
	 bool HasVacuum;
	} inventory;
};

#endif /* CPLAYER_H_ */
