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
#define PFASTINCMAXSPEED 2
#define PMAXSPEED        14
#define PJUMPINERTIA     30
#define PFASTINCRATE     64        // accel delay rate up to PFASTINCMAXSPEED
#define PFASTINCRATE_POGO  50      // rate when pogo stick is out
// rates at which player slows down
#define PFRICTION_RATE_INAIR      25      //8
#define PFRICTION_RATE_ONGROUND   5      //2
#define PFRICTION_RATE_WM         1      // on world map
// rate at which player walking animation is shown
//#define PWALKANIMRATE             40
#define PWALKANIMRATE             8

// the various jump states
#define PNOJUMP       0                 // not jumping
#define PPREPAREJUMP  1                 // doing the jump animation
#define PJUMPUP       2                 // jumping
#define PJUMPED       3                 // Player has jumped
#define PPREPAREPOGO  4                 // "pogo compressed" anim frame
#define PPOGOING      5                 // pogoing

// the different jumping frames. when CTRL is held down the player will
// go from frame PPREPAREJUMPFRAME to PJUMP_PREPARE_LAST_FRAME at a rate
// of PJUMP_PREPARE_ANIM_RATE until either CTRL is released or the player
// reaches the last frame. How far he got will select one of the various
// jump heights, defined below.
#define PPREPAREJUMPFRAME        8
#define PJUMP_PREPARE_LAST_FRAME 13
#define PJUMP_PREPARE_ANIM_RATE  11

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

// initial negative inertia to provide player at
// start of a normal jump and a (high) pogo jump
#define PJUMPUP_SPEED            20
#define PPOGOUP_SPEED            24

// These control the various jump heights.
// one of these pairs is selected depending on how
// long the CTRL key was held down while Keen
// was preparing to jump. #1 causes the highest
// jump and they get lower from there.
// NORMAL_TIME is the amount of time keen will
// jump straight up at speed -PJUMPUP_SPEED.
// when expired his jump slows down at a rate
// of DECREASERATE.

#define PJUMP_NORMALTIME_1         6
#define PJUMP_UPDECREASERATE_1     6

#define PJUMP_NORMALTIME_2         5
#define PJUMP_UPDECREASERATE_2     5

#define PJUMP_NORMALTIME_3         4
#define PJUMP_UPDECREASERATE_3     4

#define PJUMP_NORMALTIME_4         3
#define PJUMP_UPDECREASERATE_4     2

#define PJUMP_NORMALTIME_5         2
#define PJUMP_UPDECREASERATE_5     1

#define PJUMP_NORMALTIME_6         1
#define PJUMP_UPDECREASERATE_6     0


// for short pogo jumps, uses -PJUMPUP_SPEED
#define PJUMP_NORMALTIME_POGO_SHORT       6
#define PJUMP_UPDECREASERATE_POGO_SHORT   6

// for high pogo jumps, uses -PPOGOUP_SPEED
//#define PJUMP_NORMALTIME_POGO_LONG        25
//#define PJUMP_UPDECREASERATE_POGO_LONG    12
#define PJUMP_NORMALTIME_POGO_LONG        36
#define PJUMP_UPDECREASERATE_POGO_LONG    6

// for the super-pogo option
#define PPOGOUP_SPEED_SUPER                     40
#define PJUMP_NORMALTIME_POGO_LONG_SUPER        10
#define PJUMP_UPDECREASERATE_POGO_LONG_SUPER    4

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

#define DIE_ANIM_RATE       50
#define DIE_TILL_FLY_TIME   320
#define PDIE_RISE_SPEED    -20
#define DIE_MAX_XVECT       10


#endif /* PLAYERDEFINES_H_ */
