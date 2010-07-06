
#ifndef __CVORTICON__
#define __CVORTICON__

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"
#include <vector>

// TODO: VOrticon, when charges. His speed is 37. Need to implement it still.

#define VORT_JUMP_PROB_EASY     10
#define VORT_JUMP_PROB          30
#define VORT_JUMP_PROB_HARD     60

#define VORT_MIN_JUMP_HEIGHT    90
#define VORT_MAX_JUMP_HEIGHT    150
#define VORT_MAX_FALL_SPEED     320
#define VORT_JUMP_FRICTION      5

#define VORT_WALK_SPEED         28
#define VORT_WALK_ANIM_TIME     14


// number of shots to kill
#define VORT_HP                 4
#define VORT_COMMANDER_HP       105

#define VORT_DIE_ANIM_TIME      25
#define VORT2_DIE_ANIM_TIME     25

#define VORT_LOOK_ANIM_TIME     15

#define VORT1_WALK_LEFT_FRAME    70
#define VORT1_WALK_RIGHT_FRAME   74
#define VORT1_LOOK_FRAME         78
#define VORT1_JUMP_RIGHT_FRAME   80
#define VORT1_JUMP_LEFT_FRAME    81
#define VORT1_DYING_FRAME        82

#define VORT2_WALK_LEFT_FRAME    74
#define VORT2_WALK_RIGHT_FRAME   78
#define VORT2_LOOK_FRAME         82
#define VORT2_JUMP_RIGHT_FRAME   84
#define VORT2_JUMP_LEFT_FRAME    85
#define VORT2_DYING_FRAME        86
#define VORT2_DEAD_FRAME         87

#define VORT3_WALK_LEFT_FRAME    63
#define VORT3_WALK_RIGHT_FRAME   67
#define VORT3_LOOK_FRAME         71
#define VORT3_JUMP_RIGHT_FRAME   73
#define VORT3_JUMP_LEFT_FRAME    74
#define VORT3_DYING_FRAME        75
#define VORT3_DEAD_FRAME         76

#define VORT_PALETTE_FLASH_TIME  5

#define VORT_TRAPPED_DIST        160

class CVorticon : public CObject
{
public:
	CVorticon( CMap *p_map, std::vector<CPlayer> &m_vec_Player,
			Uint32 x, Uint32 y, char hp=1);
	void process();
	void initiateJump();

private:
	int m_Level;
	int m_Episode;
	char m_Difficulty;
	bool m_Dark;

	enum {
	VORT_WALK,
	VORT_JUMP,
	VORT_LOOK,
	VORT_DYING,
	VORT_DEAD,
	VORT2_DYING
	}state;

	unsigned char timer,timer2;
	unsigned int animtimer;
	unsigned char frame;
	unsigned int dist_traveled;
	signed int inertiay;

	unsigned char movedir;
	// these hold the animation frames indexes since they're
	// different for each episode
	int WalkLeftFrame;
	int WalkRightFrame;
	int LookFrame;
	int JumpRightFrame;
	int JumpLeftFrame;
	int DyingFrame;
	int DeadFrame;

	std::vector<CPlayer> &m_Player;
};

#endif //__CVORTICON__
