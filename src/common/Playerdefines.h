/*
 * Playerdefines.h
 *
 *  Created on: 13.10.2009
 *      Author: gerstrong
 */

#ifndef PLAYERDEFINES_H_
#define PLAYERDEFINES_H_

#define	PA_X		0
#define	PA_Y   		1
#define	PA_JUMP		2
#define	PA_POGO		3
#define	PA_FIRE		4
#define	PA_STATUS	5

#define	PA_MAX_ACTIONS 6

#define MAX_SPEED		7000
#define MAX_BOOST		200
#define TIME_DIVIDER	500		// For speed and acceleration

// upon starting to walk, keen will quickly increase to
// PFASTINCMAXSPEED. keen can, at a slower rate,
// reach up to PMAXSPEED (increased every walk anim frame)
//#define PFASTINCMAXSPEED 36
#define PFASTINCMAXSPEED 40
#define PMAXSPEED        70
#define PJUMPINERTIA     30
#define PFASTINCRATE     4        // accel delay rate up to PFASTINCMAXSPEED
#define PFASTINCRATE_POGO  2      // rate when pogo stick is out
// rates at which player slows down
#define PFRICTION_RATE_INAIR      20      //8
#define PFRICTION_RATE_ONGROUND   2      //2
#define PFRICTION_RATE_WM         1    // on world map
// rate at which player walking animation is shown
#define PWALKANIMRATE             8

// at STOPWALKING time after starting the exit door animation,
// keen stops walking and we go to LEVEL_DONE_WAIT.
#define LEVEL_DONE_STOPWALKING_TIME      100
// at TOTAL_WAIT_TIME, LEVEL_DONE_FADEOUT is initiated.
#define LEVEL_DONE_TOTAL_WAIT_TIME       200

// speed at which player walks through the exit door
#define PMAXEXITDOORSPEED       12

// various states we go through when a level is completed
// to do the walking out the exit door animation
enum levelstate{
	LEVEL_NOT_DONE,           // not completed
	LEVEL_DONE_WALK,           // walking through exit door
	LEVEL_DONE_WAIT,          // finished walk through door, wait a bit
	LEVEL_DONE_FADEOUT,        // fading out
	LEVEL_COMPLETE            // on to the next level!
};

enum jumpstates{
	// the various jump states
	PNOJUMP,                        // not jumping
	PPREPAREJUMP,                   // doing the jump animation
	PJUMPUP,                        // jumping
	PJUMPED,                        // Player has jumped
	PPREPAREPOGO,                   // "pogo compressed" anim frame
	PPOGOING                        // pogoing
};

// the different jumping frames. when CTRL is held down the player will
// go from frame PPREPAREJUMPFRAME to PJUMP_PREPARE_LAST_FRAME at a rate
// of PJUMP_PREPARE_ANIM_RATE until either CTRL is released or the player
// reaches the last frame. How far he got will select one of the various
// jump heights, defined below.
#define PPREPAREJUMPFRAME        8
#define PJUMP_PREPARE_LAST_FRAME 13
#define PJUMP_PREPARE_ANIM_RATE  1

// time to show player in his "pogo compressed" frame before jumping
#define PPOGO_PREPARE_TIME       10

#define PSTANDFRAME				 0		// standing, looking right
#define PFIREFRAME               20     // raygun frame index
#define PFIRE_SHOWFRAME_TIME     25    // minimum time to show raygun frame
#define PFIRE_LIMIT_SHOT_FREQ    7     // maximum speed player can shoot
#define PFIRE_LIMIT_SHOT_FREQ_FA 12     // fully automatic version

// player frame indexes for world map
#define PMAPRIGHTFRAME   32
#define PMAPDOWNFRAME    36
#define PMAPLEFTFRAME    40
#define PMAPUPFRAME      44

// various tile indexes
#define BG_GRAY            143
#define BG_GRAY_PRIORITY   304
#define BG_BLACK           155
#define BG_DARK_EP2        147
#define TILE_FELLOFFMAP_EP1    582
#define TILE_FELLOFFMAP_EP3    0
#define TILE_ICE_LEFTCORNER   403
#define TILE_ICE_RIGHTCORNER  404
#define TILE_ICE              405
#define TILE_ICE_CUBE         394
#define TILE_GLOWCELL      241


// initial negative inertia to provide player at
// start of a normal jump and a (high) pogo jump
#define PJUMPUP_SPEED            40
#define PJUMPX_SPEED			 8
// These control the various jump heights.
// one of these pairs is selected depending on how
// long the CTRL key was held down while Keen
// was preparing to jump. #1 causes the highest
// jump and they get lower from there.
// NORMAL_TIME is the amount of time keen will
// jump straight up at speed -PJUMPUP_SPEED.
// when expired his jump slows down at a rate
// of DECREASERATE.

#define PJUMP_NORMALTIME_1         22
#define PJUMP_UPDECREASERATE_1     128

#define PJUMP_NORMALTIME_2         18
#define PJUMP_UPDECREASERATE_2     102

#define PJUMP_NORMALTIME_3         14
#define PJUMP_UPDECREASERATE_3     96

#define PJUMP_NORMALTIME_4         10
#define PJUMP_UPDECREASERATE_4     80

#define PJUMP_NORMALTIME_5         6
#define PJUMP_UPDECREASERATE_5     64

#define PJUMP_NORMALTIME_6         2
#define PJUMP_UPDECREASERATE_6     48


/// pogoing
#define PPOGOUP_SPEED            80

// for short pogo jumps, uses -PJUMPUP_SPEED
//#define PJUMP_NORMALTIME_POGO_SHORT       6
//#define PJUMP_UPDECREASERATE_POGO_SHORT   128
#define PJUMP_NORMALTIME_POGO_SHORT       22
#define PJUMP_UPDECREASERATE_POGO_SHORT   128

// for high pogo jumps, uses -PPOGOUP_SPEED
//#define PJUMP_NORMALTIME_POGO_LONG        25
//#define PJUMP_UPDECREASERATE_POGO_LONG    12
#define PJUMP_NORMALTIME_POGO_LONG        22
#define PJUMP_UPDECREASERATE_POGO_LONG    64

// for the super-pogo option
#define PPOGOUP_SPEED_SUPER                     176
#define PJUMP_NORMALTIME_POGO_LONG_SUPER        20
#define PJUMP_UPDECREASERATE_POGO_LONG_SUPER    128

// when falling keen's Y inertia increases by INCREASE up to MAXSPEED
#define PFALL_INCREASE       	30
#define PFALL_MAXSPEED          60

// pogo frames
#define PFRAME_POGO           24
#define PFRAME_POGOBOUNCE     25

// frame and animation speed for frozen keen (ep1) and stunned keen (ep2&3)
#define PFRAME_FROZEN         28
#define PFROZENANIMTIME       100
// how long keen should stay frozen when hit by an ice chunk
#define PFROZEN_TIME          1000
#define PFROZEN_THAW          100

// player dieing states
#define PDIE_NODIE      0
#define PDIE_DYING      1
#define PDIE_DEAD       2
#define PDIE_FELLOFFMAP 3

#define DIE_ANIM_RATE       5
#define DIE_TILL_FLY_TIME   80
#define PDIE_RISE_SPEED    -160
#define DIE_MAX_XVECT       320

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

// friction when player is pushed by yorp's, ball's, etc.
#define PLAYPUSH_DECREASERATE   1

// if player[].psupportingtile contains this value the player is actually
// supported by an object and you should look in player[].psupportingobj
// for it's index.
#define PSUPPORTEDBYOBJECT         0

#endif /* PLAYERDEFINES_H_ */
