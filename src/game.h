#ifndef _GAME_H_
#define _GAME_H_
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

// player frame indexes for world map
#define PMAPRIGHTFRAME   32
#define PMAPDOWNFRAME    36
#define PMAPLEFTFRAME    40
#define PMAPUPFRAME      44

// uses by the key processing function which changes CTRL, ALT, and CTRL+ALT
// into JUMP, POGO, and FIRE.
#define CTRLALT_DEBOUNCETIME    20

#define YORPSTATUEHEAD     22
#define YORPSTATUEHEADUSED 485
#define YORPSTATUEBOTTOM   22

void gameloop_initialize(int episode, bool show_keensleft);

void recalc_highest_objslot(void);
char IsStopPoint(int x, int y, int o);

void killplayer(int theplayer);

// Referenzed from cinematics/EndingSequenceEp3.cpp
void eseq3_Mortimer();

#endif //_GAME_H_
