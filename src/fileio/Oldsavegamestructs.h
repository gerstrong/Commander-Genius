/*
 * Oldsavegamestructs.h
 *
 *  Created on: 27.03.2010
 *      Author: gerstrong
 *
 *  This file contains the old savegame-structures
 *  that we used back for version 0.3.0.4 and lower.
 *  These structures only serve for compatibility
 *
 *  NOTE: These structures are old and crappy!
 *  Please do never ever use them in the engine... !!!!!!
 */

#ifndef OLDSAVEGAMESTRUCTS_H_
#define OLDSAVEGAMESTRUCTS_H_

#include "../keen.h"
#include "../common/Playerdefines.h"
#include "../common/inventory.h"

struct OldSaveGameFormat
{
	struct stLevelControl
	{
		// level control
		int command;                 // used to give a command to playgame_levelmanager()
		int chglevelto;              // parameter to LVLC_CHANGE_LEVEL
		int tobonuslevel;            // if 1 player will warp to bonus level on return to WM (for ep1)
		// data about current level
		unsigned char curlevel;                // number of current level
		bool success;                // true if level was finished, 0 if he died
		bool gameovermode;           // true if "Game Over" is displayed
		bool dokeensleft;            // true if we need to do the "Keens Left"
		bool dark;                   // true if level is currently dark (lights are out)

		char episode;                 // which episode we're playing (1-3)
		bool hardmode;
		bool usedhintmb;			  // Has the message box been used?
		int demomode;

		// array of which levels have been completed (have "Done" tiles over them
		// on the world map)
		bool levels_completed[MAX_LEVELS_VORTICON+1];

		// exitXpos: the X pixel position (not <<CSFed) of the frame of the exit
		// door. when walking out the door, keen's sprite will not be drawn past
		// this point.
		unsigned int level_done, level_done_timer;
		unsigned int level_finished_by;      // index of player that finished level
		unsigned int exitXpos;

		// for ep2: how many sparks (tantalus ray machines) are left
		// you must destroy the tantalus ray generator before exiting
		int sparks_left;


		// if true, a moving platform is currently extending/retracting (ep2)
		bool PlatExtending;

		// if > 0, the screen will shake and it will decrement each frame.
		// used when you push a switch on a tantalus ray (ep2), and Mortimer's machine
		int vibratetime;
		// if 1, then while vibrating the game will be paused
		char vibratepause;

		// as long as we only have POD
		stLevelControl() { memset(this, 0, sizeof(stLevelControl)); }

		// stuff for custom episode's options
		struct
		{
			char DoorOpenDir;
			char pShotSpeed;
			char VortDieDoFade;
			char vgatiles;
			char vgasprites;
		} cepvars;
	}LevelControl;

	unsigned long scroll_x;
	unsigned int scrollx_buf;
	unsigned char scrollpix;
	unsigned int mapx;
	unsigned int mapxstripepos;
	unsigned int scroll_y;
	unsigned int scrolly_buf;
	unsigned char scrollpixy;
	unsigned int mapy;
	unsigned int mapystripepos;

	unsigned int max_scroll_x, max_scroll_y;

	struct
	{
		unsigned int xsize, ysize;            // size of the map
		bool isworldmap;             // if 1, this is the world map
		unsigned int mapdata[256][256];       // the map data
		// in-game, contains monsters and special object tags like for switches
		// on world map contains level numbers and flags for things like teleporters
		unsigned int objectlayer[256][256];
		bool firsttime;  // used when generating multiplayer positions on world map
	} map;

	struct stPlayer
	{
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

	   stInventory inventory;

		stPlayer() { memset(this, 0, sizeof(stPlayer)); }
	}Player;
};

#endif /* OLDSAVEGAMESTRUCTS_H_ */
