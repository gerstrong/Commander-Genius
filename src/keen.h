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

#define CSF    5

//#define OVSIZE    3000

// when crashflag is activated by setting it to QUIT_NONFATAL,
// the application will immediately shut down, however the
// "a Fatal Error Occurred" message box will not pop up and
// the sysbeep will not sound.
#define QUIT_NONFATAL   555

#define SAVEGAMEVERSION '4'
#define ATTRFILEVERSION  1

#define WM_MAP_NUM      80

#define MAX_SPRITES  500
#define MAX_FONT     256
#define MAX_BITMAPS  20

#define MAX_OBJECTS    100
#define MAX_ANIMTILES  200

#define PAL_FADE_SHADES   20
#define PAL_FADE_WHITEOUT 40
typedef struct stFade
{
 int mode;
 int dir;
 int curamt;
 int fadetimer;
 int rate;
} stFade;
#define NO_FADE         0
#define FADE_GO         1
#define FADE_COMPLETE   2

#define FADE_IN         1
#define FADE_OUT        2
#define FADE_NORM       3
#define FADE_FAST       1
#define FADE_SLOW       30

#define NO_QUIT                 0
#define QUIT_PROGRAM            1
#define QUIT_TO_TITLE           2

#define MAX_LEVELS     100
#define SCROLLBUF_XSIZE  512
#define SCROLLBUF_YSIZE  512
#define SCROLLBUF_MEMSIZE ((SCROLLBUF_XSIZE)*(SCROLLBUF_YSIZE+300))
#define TILE_W			16
#define TILE_H			16
#define TILE_S			4
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

 #define MAX_PLAYERS            8

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


typedef struct stMap
{
 unsigned int xsize, ysize;            // size of the map
 bool isworldmap;             // if 1, this is the world map
 unsigned int mapdata[256][256];       // the map data
 // in-game, contains monsters and special object tags like for switches
 // on world map contains level numbers and flags for things like teleporters
 unsigned int objectlayer[256][256];
 bool firsttime;  // used when generating multiplayer positions on world map
} stMap;

struct stBitmap
{
 int xsize;
 int ysize;
 unsigned char *bmptr;
 char name[9];
};

struct stSprite
{
 char xsize, ysize;
 unsigned char imgdata[64][64];
 unsigned char maskdata[64][64];
 // bounding box for hit detection
 unsigned int bboxX1, bboxY1;
 unsigned int bboxX2, bboxY2;
};

struct stInventory
{
 unsigned long score;
 unsigned long extralifeat;
 unsigned int charges;        // ray gun ammo
 signed int lives;
 unsigned char HasPogo;
 unsigned char HasCardYellow;
 unsigned char HasCardRed;
 unsigned char HasCardGreen;
 unsigned char HasCardBlue;
 // ep1 only
 unsigned char HasJoystick;
 unsigned char HasFuel;
 unsigned char HasBattery;
 unsigned char HasVacuum;
};

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

// structures for each AI module's data
#include "ai/enemydata.h"

// and the object structure containing the union of the above structs
typedef struct stObject
{
 unsigned int type;        // yorp, vorticon, etc.
 unsigned int exists;
 unsigned int onscreen;    // 1=(scrx,scry) position is visible onscreen
 unsigned int hasbeenonscreen;
 unsigned int sprite;      // which sprite should this object be drawn with
 unsigned int x, y;        // x,y location in map coords, CSFed
 int scrx, scry;           // x,y pixel position on screen

 // if type is OBJ_PLAYER, this contains the player number that this object
 // is associated with
 int AssociatedWithPlayer;

 // if zero, priority tiles will not be honored and object will always
 // appear in front of the background
 char honorPriority;

 char canbezapped;         // if 0 ray will not stop on hitdetect
 char zapped;              // number of times got hit by keen's raygun
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

 unsigned int needinit;    // 1=new object--requires initilization
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
		stWalkerData walker;
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
 unsigned char erasedata[64][64];   // backbuffer to erase this object
} stObject;

// (map) stripe attribute structures, for animated tiles
// slot 0 is not used. data starts at slot 1. see description
// of AnimTileInUse in map structure to see why.
typedef struct stAnimTile
{
  int slotinuse;        // if 0, this entry should not be drawn
  int x;                // x pixel position in scrollbuf[] where tile is
  int y;                // y pixel position in scrollbuf[]
  int baseframe;        // base frame, i.e. the first frame of animation
  int offset;           // offset from base frame
} stAnimTile;

#define NUM_OBJ_TYPES      40

enum enumerated_Objects{
// ** objects from KEEN1
OBJ_YORP,
OBJ_GARG,
OBJ_VORT,
OBJ_BUTLER,
OBJ_TANK,
OBJ_RAY,                // keen's raygun blast
OBJ_DOOR,               // an opening door
OBJ_ICECHUNK,           // ice chunk from ice cannon
OBJ_ICEBIT,             // piece of shattered ice chunk
OBJ_PLAYER,
OBJ_TELEPORTER,        // world map teleporter
OBJ_ROPE,

// ** objects from KEEN2 (some of these are in ep3 as well)
OBJ_WALKER,
OBJ_TANKEP2,
OBJ_PLATFORM,
OBJ_VORTELITE,
OBJ_SECTOREFFECTOR,
OBJ_BABY,
OBJ_EXPLOSION,
OBJ_EARTHCHUNK,
OBJ_SPARK,

// ** objects from KEEN3
OBJ_FOOB,
OBJ_NINJA,
OBJ_MEEP,
OBJ_SNDWAVE,
OBJ_MOTHER,
OBJ_FIREBALL,
OBJ_BALL,
OBJ_JACK,
OBJ_PLATVERT,
OBJ_NESSIE,

OBJ_DEMOMSG,

// the guns that shoot periodically
OBJ_AUTORAY,
OBJ_AUTORAY_V,
OBJ_ICECANNON,

OBJ_GOTPOINTS,	//this thing is the rising point numbers
OBJ_GHOST,		//ghosted object from map editor
LAST_OBJ_TYPE
};

// default sprites...when an object is spawned it's sprite is set to this
// sprite. the object AI will immediately reset the sprite frame, so it
// wouldn't really matter what these are...except that it does because
// the width and height of the default sprite will determine exactly when
// the object because active the first time it scrolls onto the screen
// from the top or left. if the default sprite is wrong the object may
// suddenly appear on the screen instead of smoothly scrolling on.
#define OBJ_YORP_DEFSPRITE       50
#define OBJ_GARG_DEFSPRITE       60
#define OBJ_VORT_DEFSPRITE_EP1   78
#define OBJ_VORT_DEFSPRITE_EP2   82
#define OBJ_VORT_DEFSPRITE_EP3   71
#define OBJ_BUTLER_DEFSPRITE     88
#define OBJ_TANK_DEFSPRITE       98
#define OBJ_RAY_DEFSPRITE_EP1    108
#define OBJ_RAY_DEFSPRITE_EP2    122
#define OBJ_RAY_DEFSPRITE_EP3    102
#define OBJ_ICECHUNK_DEFSPRITE   112
#define OBJ_ICEBIT_DEFSPRITE     113
#define OBJ_ROPE_DEFSPRITE       114
#define OBJ_TELEPORTER_DEFSPRITE 180


#define OBJ_PLATFORM_DEFSPRITE_EP2   126
#define OBJ_PLATFORM_DEFSPRITE_EP3   107
#define OBJ_WALKER_DEFSPRITE     102
#define OBJ_TANKEP2_DEFSPRITE    112

#define OBJ_VORTELITE_DEFSPRITE  88

#define OBJ_FOOB_DEFSPRITE			95
#define OBJ_NINJA_DEFSPRITE			77
#define OBJ_MOTHER_DEFSPRITE		87
#define OBJ_BALL_DEFSPRITE			109
#define OBJ_JACK_DEFSPRITE			110
#define OBJ_MEEP_DEFSPRITE			118
#define OBJ_NESSIE_DEFSPRITE		132

#define OBJ_BABY_DEFSPRITE_EP2   52
#define OBJ_BABY_DEFSPRITE_EP3   51

#define OBJ_SPARK_DEFSPRITE_EP2		128

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

// scroll triggers
#define SCROLLTRIGGERRIGHT     194
#define SCROLLTRIGGERLEFT      110
#define SCROLLTRIGGERUP        80
#define SCROLLTRIGGERDOWN      114

// this structure contains all the variables used by a player
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

   unsigned char blockedl,blockedr,blockedu,blockedd;
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
};

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

#define TELEPORT_BONUS_DESTX    ((((23085>>CSF>>4)+2)<<4<<CSF)-(8<<CSF))
#define TELEPORT_BONUS_DESTY    (((12501>>CSF>>4)+2)<<4<<CSF)

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

// animation rate of animated tiles
#define ANIM_TILE_TIME        40


#define TELEPORT_GRAY_BASEFRAME_EP1  342
#define TELEPORT_GRAY_IDLEFRAME_EP1  99

#define TELEPORT_RED_BASEFRAME_EP1   338
#define TELEPORT_RED_IDLEFRAME_EP1   325

#define TELEPORT_BASEFRAME_EP3  130
#define TELEPORT_IDLEFRAME_EP3  134

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

#include <SDL.h>
#include "include/declarations.h"
#include "externals.h"
#include "sdl/CSettings.h"

struct stCloneKeenPlus
{
	stCommand Command[MAX_COMMANDS];
	SDL_Joystick *Joystick;
	SDL_Event Event;
	stResources Resources;
	stGameData *GameData;
	stDevice Device;
	stControl Control;
	stOption Option[NUM_OPTIONS];
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
