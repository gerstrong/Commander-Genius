
#define MAX_COMMANDS 8
#define MAX_SOUND_LENGTH      1024
#define MAX_SOUNDS            50
#define MAX_STRING_LENGTH	  256


#define MAX_NUMBER_OF_FILES     100

typedef struct stDisplay
{
  unsigned int Width;
  unsigned int Height;
  unsigned short Depth;
  short Mode;
  short Fullscreen;
  short Filtermode;
  short Zoom;
  unsigned short FrameSkip;
} stDisplay;

typedef struct stDevice
{
  SDL_Joystick *Joystick;
  SDL_Event Event;
} stDevice;

typedef struct stLevelControl
{
 // level control
 int command;                 // used to give a command to playgame_levelmanager()
 int chglevelto;              // parameter to LVLC_CHANGE_LEVEL
 int tobonuslevel;            // if 1 player will warp to bonus level on return to WM (for ep1)
 // data about current level
 int curlevel;                // number of current level
 char success;                // 1 if level was finished, 0 if he died
 char isfinallevel;           // 1 if this is the final level
 char canexit;                // 1 if player is allowed to use the exit door
 char gameovermode;           // 1 if "Game Over" is displayed
 char dokeensleft;            // 1 if we need to do the "Keens Left"
 char dark;                   // 1 if level is currently dark (lights are out)

 int episode;                 // which episode we're playing (1-3)
 bool hardmode;

 // array of which levels have been completed (have "Done" tiles over them
 // on the world map)
 int levels_completed[MAX_LEVELS+1];

 // exitXpos: the X pixel position (not <<CSFed) of the frame of the exit
 // door. when walking out the door, keen's sprite will not be drawn past
 // this point.
 unsigned int level_done, level_done_timer;
 unsigned int level_finished_by;      // index of player that finished level
 unsigned int exitXpos;
} stLevelControl;


typedef struct stControl
{
	short eseq; 		// see only the ending sequence
	short dtm; 			// go direct to map.
	short storyboard; 	// go to storyboard
	short skipstarting;	// Skip Startmenu, Intro and Mainmenu.

	stLevelControl levelcontrol;
} stControl;

typedef struct stGameData
{
  char DataDirectory[MAX_STRING_LENGTH];
  short Episode;
  char Name[MAX_STRING_LENGTH];
  char FileList[MAX_NUMBER_OF_FILES][MAX_STRING_LENGTH];
} stGameData;


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
