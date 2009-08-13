#ifndef __CG_DECLARATIONS_H__
#define __CG_DECLARATIONS_H__

#include <string>
#include <SDL.h>
#include <cstdlib>

#define MAX_COMMANDS 8
#define MAX_SOUND_LENGTH      1024
#define MAX_SOUNDS            50


#define MAX_NUMBER_OF_FILES     100

struct stDisplay
{
  unsigned int Width;
  unsigned int Height;
  unsigned short Depth;
  short Mode;
  short Fullscreen;
  short Filtermode;
  short Zoom;
  unsigned short FrameSkip;
	
	// as long as we only have POD
	stDisplay() { memset(this, 0, sizeof(stDisplay)); }	
};

struct stDevice
{
  SDL_Joystick *Joystick;
  SDL_Event Event;
};

struct stLevelControl
{
 // level control
 int command;                 // used to give a command to playgame_levelmanager()
 int chglevelto;              // parameter to LVLC_CHANGE_LEVEL
 int tobonuslevel;            // if 1 player will warp to bonus level on return to WM (for ep1)
 // data about current level
 char curlevel;                // number of current level
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
 int levels_completed[MAX_LEVELS+1];

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
};


struct stControl
{
	bool eseq; 		// see only the ending sequence
	bool dtm; 			// go direct to map.
	bool storyboard; 	// go to storyboard
	bool skipstarting;	// Skip Startmenu, Intro and Mainmenu.

	stLevelControl levelcontrol;
	
	stControl() : eseq(false), dtm(false), storyboard(false), skipstarting(false) {}
};

struct stGameData
{
	std::string DataDirectory;
	short Episode;
	std::string Name;
	std::string FileList[MAX_NUMBER_OF_FILES];
	stGameData() : Episode(0) {}
};


typedef struct stResources
{
	unsigned short GameSelected;
} stResources;

typedef struct stJoyevent
{
	unsigned short button;
	short axis;
} stJoyevent;

typedef struct stCommand
{
	char name[10];
	char description[80];

	Uint8 eventtype;
	SDL_KeyboardEvent key;
	SDL_MouseMotionEvent motion;
	SDL_MouseButtonEvent button;
	SDL_JoyAxisEvent jaxis;
	SDL_JoyBallEvent jball;
	SDL_JoyHatEvent jhat;
	SDL_JoyButtonEvent jbutton;
	short state;
} stCommand;

typedef struct stNewPlayer
{
	stCommand Command;
} stNewPlayer;

#endif
