// various states we go through when a level is completed
// to do the walking out the exit door animation
enum levelstate
{
LEVEL_NOT_DONE,           // not completed
LEVEL_DONE_WALK,           // walking through exit door
LEVEL_DONE_WAIT,          // finished walk through door, wait a bit
LEVEL_DONE_FADEOUT,        // fading out
LEVEL_COMPLETE            // on to the next level!
};

// width of the player sprite, used for walking "behind" the exit door frame
#define PLAYERSPRITE_WIDTH      16

// at STOPWALKING time after starting the exit door animation,
// keen stops walking and we go to LEVEL_DONE_WAIT.
#define LEVEL_DONE_STOPWALKING_TIME      450
// at TOTAL_WAIT_TIME, LEVEL_DONE_FADEOUT is initiated.
#define LEVEL_DONE_TOTAL_WAIT_TIME       1000

// for ankh shield...
// stage1 is no flicker of the shield, changes colors
// in stage2 the shield flickers fast, changes colors
// in stage3 the shield flashes slow, no color change
#define ANKH_STAGE1_TIME    3000
#define ANKH_STAGE2_TIME    1500
#define ANKH_STAGE3_TIME    500
#define PLAY_ANKH_TIME      (ANKH_STAGE1_TIME+ANKH_STAGE2_TIME+ANKH_STAGE3_TIME)
// these correspond stage 1,2 and 3 respectively
#define ANKH_STATE_NOFLICKER    0
#define ANKH_STATE_FLICKERFAST  1
#define ANKH_STATE_FLICKERSLOW  2

// rate at which the periodically firing guns in ep3 will fire
#define GUN_FIRE_FREQ        800
// rate at which the ice cannons in ep1 will fire
#define ICECANNON_FIRE_FREQ  400

// direction defines used for various things
enum directions{
RIGHT,
LEFT,
UP,
DOWN
};

#define ANKH_SHIELD_FRAME       61


// speed at which player walks through the exit door
#define PMAXEXITDOORSPEED       3

// the various jump states
#define PNOJUMP       0                 // not jumping
#define PPREPAREJUMP  1                 // doing the jump animation
#define PJUMPUP       2                 // jumping
#define PJUMPED       3                 // Player has jumped
#define PPREPAREPOGO  4                 // "pogo compressed" anim frame
#define PPOGOING      5                 // pogoing


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



// friction when player is pushed by yorp's, ball's, etc.
#define PLAYPUSH_DECREASERATE   1

// uses by the key processing function which changes CTRL, ALT, and CTRL+ALT
// into JUMP, POGO, and FIRE.
#define CTRLALT_DEBOUNCETIME    20


// special sprites (they either weren't in the game originally,
// or are used for internal engine stuff).
#define BLANKSPRITE			(MAX_SPRITES-1)
#define DOOR_YELLOW_SPRITE	(MAX_SPRITES-2)			// opening door (yellow)
#define DOOR_BLUE_SPRITE	(MAX_SPRITES-3)
#define DOOR_GREEN_SPRITE	(MAX_SPRITES-4)
#define DOOR_RED_SPRITE		(MAX_SPRITES-5)
#define PT5000_SPRITE		(MAX_SPRITES-6)			// rising bonus pts (+5000)
#define PT1000_SPRITE		(MAX_SPRITES-7)
#define PT500_SPRITE		(MAX_SPRITES-8)
#define PT200_SPRITE		(MAX_SPRITES-9)
#define PT100_SPRITE		(MAX_SPRITES-10)
#define PT1UP_SPRITE		(MAX_SPRITES-11)		// rising 1up bonus
#define PTCARDY_SPRITE		(MAX_SPRITES-12)		// rising access card
#define PTCARDR_SPRITE		(MAX_SPRITES-13)
#define PTCARDG_SPRITE		(MAX_SPRITES-14)
#define PTCARDB_SPRITE		(MAX_SPRITES-15)
#define SHOTUP_SPRITE		(MAX_SPRITES-16)		// rising single shot (ep3)
#define ANKHUP_SPRITE		(MAX_SPRITES-17)		// rising single shot (ep3)
#define GUNUP_SPRITE		(MAX_SPRITES-18)		// rising ray gun
#define YORPSHIELD_SPRITE	(MAX_SPRITES-19)		// invincibility force field, used in editor and in "yorps have forcefields" mode
#define VERSION_SPRITE		(MAX_SPRITES-20)		// version text shown in lower-right corner at startup
#define DEMOBOX_SPRITE		(MAX_SPRITES-21)		// says "Demo"
#define OSD_LIVES_SPRITE	(MAX_SPRITES-22)
#define OSD_AMMO_SPRITE		(MAX_SPRITES-23)
#define OSD_YORPS_SPRITE	(MAX_SPRITES-24)
#define TITLE_LOGO1_SPRITE	(MAX_SPRITES-25)		// left half of the logo
#define TITLE_LOGO2_SPRITE	(MAX_SPRITES-26)		// right half of the logo
#define TITLE_FLOOR_SPRITE	(MAX_SPRITES-27)		// more stuff for the logo
#define TITLE_VORT_SPRITE   (MAX_SPRITES-28)
#define ARROWLR_SPRITE		(MAX_SPRITES-29)		// these are directional arrows used in the editor
#define ARROWUD_SPRITE		(MAX_SPRITES-30)
#define ARROWUR_SPRITE		(MAX_SPRITES-31)
#define ARROWUL_SPRITE		(MAX_SPRITES-32)
#define ARROWU_SPRITE		(MAX_SPRITES-33)
#define ARROWD_SPRITE		(MAX_SPRITES-34)
#define LAST_SPECIAL_SPRITE (MAX_SPRITES-35)

#define YORPSTATUEHEAD     22
#define YORPSTATUEHEADUSED 485
#define YORPSTATUEBOTTOM   22
#define ENEMYRAY           109
#define ENEMYRAYEP2        123
#define ENEMYRAYEP3        103
#define RAY_VERT_EP3       104

void gameloop(stCloneKeenPlus *pCKP);
void gameloop_initialize(int episode, bool show_keensleft);
void initsprites(stCloneKeenPlus *pCKP, int s);
void keen_get_goodie(int px, int py, int theplayer, stCloneKeenPlus *pCKP);
void procgoodie(int t, int mpx, int mpy, int theplayer, stCloneKeenPlus *pCKP);

void recalc_highest_objslot(void);
char IsStopPoint(int x, int y, int o);

int initgamefirsttime(stCloneKeenPlus *pCKP, int s);

void open_door(int doortile, int doorsprite, int mpx, int mpy, int cp, stCloneKeenPlus *pCKP);
void killplayer(int theplayer);
void PlayerTouchedExit(int theplayer, stCloneKeenPlus *pCKP);

void endlevel(int reason_for_leaving, stLevelControl *levelcontrol);

char checkissolidl(int x, int y, int cp, stCloneKeenPlus *pCKP);
char checkissolidr(int x, int y, int cp, stCloneKeenPlus *pCKP);

void gamedo_enemyai(stLevelControl *p_levelcontrol);

// Referenzed from cinematics/EndingSequenceEp3.cpp
void eseq3_Mortimer();

void start_gameover(stCloneKeenPlus *pCKP);
