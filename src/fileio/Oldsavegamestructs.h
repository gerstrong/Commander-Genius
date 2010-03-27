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

const short MAX_LEVELS = 20;

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
		bool levels_completed[MAX_LEVELS+1];

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
};

#endif /* OLDSAVEGAMESTRUCTS_H_ */
