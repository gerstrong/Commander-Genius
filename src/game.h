#ifndef _GAME_H_
#define _GAME_H_

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

void killplayer(int theplayer);

// Referenzed from cinematics/EndingSequenceEp3.cpp
void eseq3_Mortimer();

#endif //_GAME_H_
