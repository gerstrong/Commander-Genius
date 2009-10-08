/*
 * CPlayer.h
 *
 *  Created on: 05.10.2009
 *      Author: gerstrong
 */

#ifndef CPLAYER_H_
#define CPLAYER_H_

#include "CObject.h"

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

// upon starting to walk, keen will quickly increase to
// PFASTINCMAXSPEED. keen can, at a slower rate,
// reach up to PMAXSPEED (increased every walk anim frame)
//#define PFASTINCMAXSPEED 36
#define PFASTINCMAXSPEED 2
#define PMAXSPEED        14
#define PJUMPINERTIA     30
#define PFASTINCRATE     64        // accel delay rate up to PFASTINCMAXSPEED
#define PFASTINCRATE_POGO  50      // rate when pogo stick is out
// rates at which player slows down
#define PFRICTION_RATE_INAIR      25      //8
#define PFRICTION_RATE_ONGROUND   5      //2
#define PFRICTION_RATE_WM         1      // on world map
// rate at which player walking animation is shown
//#define PWALKANIMRATE             40
#define PWALKANIMRATE             8

// the various jump states
#define PNOJUMP       0                 // not jumping
#define PPREPAREJUMP  1                 // doing the jump animation
#define PJUMPUP       2                 // jumping
#define PJUMPED       3                 // Player has jumped
#define PPREPAREPOGO  4                 // "pogo compressed" anim frame
#define PPOGOING      5                 // pogoing

// the different jumping frames. when CTRL is held down the player will
// go from frame PPREPAREJUMPFRAME to PJUMP_PREPARE_LAST_FRAME at a rate
// of PJUMP_PREPARE_ANIM_RATE until either CTRL is released or the player
// reaches the last frame. How far he got will select one of the various
// jump heights, defined below.
#define PPREPAREJUMPFRAME        8
#define PJUMP_PREPARE_LAST_FRAME 13
#define PJUMP_PREPARE_ANIM_RATE  11

// time to show player in his "pogo compressed" frame before jumping
#define PPOGO_PREPARE_TIME       50

#define PSTANDFRAME				 0		// standing, looking right
#define PFIREFRAME               20     // raygun frame index
#define PFIRE_SHOWFRAME_TIME     100    // minimum time to show raygun frame
#define PFIRE_LIMIT_SHOT_FREQ    30     // maximum speed player can shoot
#define PFIRE_LIMIT_SHOT_FREQ_FA 50     // fully automatic version

// player frame indexes for world map
#define PMAPRIGHTFRAME   32
#define PMAPDOWNFRAME    36
#define PMAPLEFTFRAME    40
#define PMAPUPFRAME      44

// player dieing states
#define PDIE_NODIE      0
#define PDIE_DYING      1
#define PDIE_DEAD       2
#define PDIE_FELLOFFMAP 3

#define DIE_ANIM_RATE       50
#define DIE_TILL_FLY_TIME   320
#define PDIE_RISE_SPEED    -20
#define DIE_MAX_XVECT       10

///
// Class definition starts here!
///
class CPlayer {
public:

	// direction defines used for various things
	enum e_directions{
	RIGHT,LEFT,	UP,	DOWN
	};

	enum e_playingmodes{
		NONE, WORLDMAP, LEVELPLAY
	};

	CPlayer();

	// World Map specific
	void processWorldMap();
	void setWorldMapdir();
	void selectFrameOnWorldMap();
	void setWMblockedlrud();
	void AllowEnterLevelonWM();
	bool isWMSolid(int xb, int yb, bool *levels_completed);
	void Walking();
	void WalkingAnimation();
	void InertiaAndFriction_X();
	void InertiaAndFriction_Y();
	void AllowMountUnmountNessie();

	// In Level specific
	void processInLevel();

	// Used for both situations
	void StatusBox();
	void ProcessInput();

	virtual ~CPlayer();

	///
	// variables
	// these coordinates are CSFed
	unsigned long x;
	unsigned int y;

	unsigned int w;
	unsigned int h;

	char m_playingmode;
	char m_episode;
	int m_player_number;
	// Pointer to the Object type assign to this player
	CObject *mp_object;

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

	bool *mp_levels_completed;

	unsigned char pjumping, pjumptime, pjumpupspeed_decreasetimer, pjumpdir;
	unsigned char pjumpframe, pjumpanimtimer, pjumpupspeed;
	unsigned char pjumpnormaltime, pjumpupdecreaserate, pjustjumped;
	unsigned char pjustfell;
	unsigned char pjumpfloattimer;

	unsigned char pdir,pshowdir,lastpdir;

	char pfiring,pfireframetimer;
	bool inhibitwalking, inhibitfall;

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

	// New values
	char playcontrol[PA_MAX_ACTIONS];

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

	unsigned int m_level_coordinates;
};

#endif /* CPLAYER_H_ */
