#ifndef __KEEN_H__
#define __KEEN_H__

#include <cstdio>
#include <cstring>
#include <cstdlib>

#ifdef TARGET_WIN32
#include <windows.h>
#endif

#include "vorticon/sounds.h"
#include "funcdefs.h"
#include "fileio/CTileLoader.h"

#include "include/playeraction.h"

#define CSF    9
#define TILE_W			16
#define TILE_H			16
#define TILE_S			4
#define STC (CSF-TILE_S)

//#define OVSIZE    3000

// when crashflag is activated by setting it to QUIT_NONFATAL,
// the application will immediately shut down, however the
// "a Fatal Error Occurred" message box will not pop up and
// the sysbeep will not sound.
#define QUIT_NONFATAL   555

#define SAVEGAMEVERSION '5'
#define ATTRFILEVERSION  1

#define WM_MAP_NUM      80

#define MAX_SPRITE_WIDTH	80
#define MAX_SPRITE_HEIGHT	62
#define MAX_FONT     256
#define MAX_BITMAPS  20


#define MAX_OBJECTS    100
#define MAX_ANIMTILES  200

#define NO_QUIT                 0
#define QUIT_PROGRAM            1
#define QUIT_TO_TITLE           2

#define MAX_LEVELS     100
#define SCROLLBUF_XSIZE  512
#define SCROLLBUF_YSIZE  512
#define SCROLLBUF_MEMSIZE ((SCROLLBUF_XSIZE)*(SCROLLBUF_YSIZE+300))
#define SCROLLBUF_NUMTILESX (SCROLLBUF_XSIZE / 16)
#define SCROLLBUF_NUMTILESY (SCROLLBUF_YSIZE / 16)

#define BLITBUF_XSIZE  320
#define BLITBUF_YSIZE  200
#define BLITBUF_MEMSIZE ((BLITBUF_XSIZE)*(BLITBUF_YSIZE+30))
// for each entry in the animtileinuse array that is nonzero, that
// location on the display is an animated tile which is currently registered
// in animtiles[]. Used in map_draw_hstripe and map_draw_vstripe.
// When drawing a new stripe over one that previously contained an animated
// tile, this lets it know it needs to unregister the animated tile that
// used to be there. the nonzero value corresponds to the associated entry
// in animtiles[]. the x,y pixel position is the index in here * 16.
#define ATILEINUSE_SIZEX               33
#define ATILEINUSE_SIZEY               33

#define WORLD_MAP              80
#define FINAL_MAP              16

// values returned by gameloop()

#define LVLC_NOCOMMAND         0
#define LVLC_CHANGE_LEVEL      1
#define LVLC_END_SEQUENCE      2
#define LVLC_GAME_OVER         3
#define LVLC_TANTALUS_RAY      4       // switch on tantalus ray pressed
#define LVLC_START_LEVEL       5

#define WON_LEVEL				LVLC_CHANGE_LEVEL


struct stMap
{
	unsigned int xsize, ysize;            // size of the map
	bool isworldmap;             // if 1, this is the world map
	unsigned int mapdata[256][256];       // the map data
	// in-game, contains monsters and special object tags like for switches
	// on world map contains level numbers and flags for things like teleporters
	unsigned int objectlayer[256][256];
	bool firsttime;  // used when generating multiplayer positions on world map
};


struct SDL_Surface;

// for strings loaded from "strings.dat"
#define MAX_STRINGS             100
#define MAX_ATTRIBUTES          16
struct stString
{
	std::string name;    // pointer to malloc'd area containing string name
	std::string stringptr;    // pointer to malloc'd area containing string
	
	int numAttributes;
	unsigned char *attrnames[MAX_ATTRIBUTES+1];
	unsigned int attrvalues[MAX_ATTRIBUTES+1];
};

#include "externals.h"

// default sprites...when an object is spawned it's sprite is set to this
// sprite. the object AI will immediately reset the sprite frame, so it
// wouldn't really matter what these are...except that it does because
// the width and height of the default sprite will determine exactly when
// the object because active the first time it scrolls onto the screen
// from the top or left. if the default sprite is wrong the object may
// suddenly appear on the screen instead of smoothly scrolling on.

// some directions (mostly for OBJ_ICECHUNK and OBJ_ICEBIT)
#define DUPRIGHT         0
#define DUPLEFT          1
#define DUP              2
#define DDOWN            3
#define DDOWNRIGHT       4
#define DDOWNLEFT        5
#define DLEFT            6
#define DRIGHT           7

// directions for OBJ_EARTHCHUNK
#define EC_UPLEFTLEFT       0             // 22 degrees CC of UP/LEFT
#define EC_UPUPLEFT         1             // 22 degrees C of UP/LEFT
#define EC_UP               2             // straight UP
#define EC_UPUPRIGHT        3             // 22 degrees CC of UP/RIGHT
#define EC_UPRIGHTRIGHT     4             // 22 degrees C of UP/RIGHT
#define EC_DOWNLEFTLEFT     5             // 22 degrees CC of DOWN/LEFT
#define EC_DOWNDOWNLEFT     6             // 22 degrees C of DOWN/LEFT
#define EC_DOWN             7             // straight DOWN
#define EC_DOWNDOWNRIGHT    8             // 22 degrees CC of DOWN/RIGHT
#define EC_DOWNRIGHTRIGHT   9             // 22 degrees C of DOWN/RIGHT

#define EC_UPLEFT           10
#define EC_UPRIGHT          11
#define EC_DOWNLEFT         12
#define EC_DOWNRIGHT        13

typedef struct stShipQueue
{
	int cmd;
	int time;
	int flag1;
} stShipQueue;


#define TILE_LITTLE_DONE     77
#define TILE_BIG_DONE1       78
#define TILE_BIG_DONE2       79
#define TILE_BIG_DONE3       80
#define TILE_BIG_DONE4       81

#define TILE_TELEPORTER_GREY_IDLE  99
#define TILE_TELEPORTER_RED_INUSE  338

// special level codes on worldmap
#define LVLS_TELEPORTER_BONUS      46            // bonus teleporter in ep1
#define LVLS_SHIP                  20

#define TELEPORTING_OUT         0
#define TELEPORTING_IN          1

#define TELEPORT_BONUS_DESTX    ((((23085>>CSF)+2)<<CSF)-(8<<CSF))
#define TELEPORT_BONUS_DESTY    (((12501>>CSF)+2)<<CSF)

#define BONUSLEVEL_RESPAWN_X    31812
#define BONUSLEVEL_RESPAWN_Y    18936

#define MAINMNU_1PLAYER      	0
#define MAINMNU_2PLAYER      	1
#define MAINMNU_LOADGAME     	2
#define MAINMNU_STORY	     	3
#define MAINMNU_HIGHSCORES   	4
#define MAINMNU_OPTIONS      	5
#define MAINMNU_DEMO         	6
#define MAINMNU_NEW_GAME     	7
#define MAINMNU_ABOUT        	8
#define MAINMNU_ORDERING_INFO 	9
#define MAINMNU_QUIT         	10
#define MAINMNU_TIMEOUT      	11     // going to demo due to timeout
#define RESTART_GAME         	12
#define BACK2MAINMENU		 	13

#define MAINMENU_NUM_OPTIONS        MAINMNU_QUIT

#define TILE_SWITCH_UP             480
#define TILE_SWITCH_DOWN           493
#define TILE_LIGHTSWITCH           271
#define TILE_EXTENDING_PLATFORM    270

// "Sector Effector" types
enum sector_effector_type{
	SE_EXTEND_PLATFORM,
	SE_RETRACT_PLATFORM,
	SE_ANKHSHIELD,
	SE_MORTIMER_ARM,
	SE_MORTIMER_LEG_LEFT,
	SE_MORTIMER_LEG_RIGHT,
	SE_MORTIMER_SPARK,
	SE_MORTIMER_HEART,
	SE_MORTIMER_ZAPSUP,
	SE_MORTIMER_RANDOMZAPS
};


// special object markers
#define NESSIE_PATH             8192
#define NESSIE_PAUSE            8448
#define NESSIE_MOUNTPOINT       8704
#define GARG_STOPPOINT          1000
#define BALL_NOPASSPOINT        1001

// values for demomode global variable
#define DEMO_NODEMO             0
#define DEMO_RECORD             1
#define DEMO_PLAYBACK           2

#define DEMO_MAX_SIZE           2048

#define DEMO_RESULT_COMPLETED    0
#define DEMO_RESULT_CANCELED     1
#define DEMO_RESULT_FILE_BAD     2

// Runtime of CKP
#define SHUTDOWN_NONE			 0
#define SHUTDOWN_NO_ERRORS		 0
#define SHUTDOWN_BOOTUP			 1
#define SHUTDOWN_RESTART		 2
#define SHUTDOWN_NEW_GAME		 3
#define SHUTDOWN_EXIT		 	 4


// Video Modes
#define VIDEO_MODE_SOFTWARE		 0
#define VIDEO_MODE_HARDWARE		 1
#define VIDEO_MODE_OPENGL		 2

// directory "special" sprites and VGASprites are under
#define GFXDIR					"data/res/gfx/"

#include "include/declarations.h"
#include "sdl/CSettings.h"

struct stCloneKeenPlus
{
	stCommand Command[MAX_COMMANDS];
	SDL_Joystick *Joystick;
	SDL_Event Event;
	stResources Resources;
	stGameData *GameData;
	stControl Control;
	//stOption Option[NUM_OPTIONS];
	unsigned short numGames;
	unsigned short shutdown;
	
	stCloneKeenPlus() : Joystick(NULL), GameData(NULL), numGames(0), shutdown(SHUTDOWN_NONE) {
		memset(&Event, 0, sizeof(Event));
	}
};

// keen.c
void playgame_levelmanager(stCloneKeenPlus *pCKP);
char play_demo(int demonum, stCloneKeenPlus *pCKP, int s);

#endif
