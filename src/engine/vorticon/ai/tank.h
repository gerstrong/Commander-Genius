/*
 * tankep2.h
 *
 *  Created on: 25.06.2010
 *      Author: gerstrong
 */

#ifndef TANKEP2_H_
#define TANKEP2_H_


#define OBJ_TANKEP2_DEFSPRITE    112
#define OBJ_TANK_DEFSPRITE       98

// this same struct is used for both ep1 and ep2 "tanks", although
// they have seperate ai modules
typedef struct stTankData
{
	// Tank Robot (Ep2 and (ep1))
	enum{ TANK_WALK,TANK_TURN,TANK_WAIT,TANK_WAIT_LOOK,TANK_FIRE,TANK_LOOK
	} state;

	unsigned int timer,animtimer;
	unsigned char frame;
	unsigned int dist_to_travel;

	unsigned char movedir;

	unsigned int ponsameleveltime;
	unsigned char alreadyfiredcauseonsamelevel;
	unsigned char fireafterlook;

	bool detectedPlayer;         		  // true if player on same level
	unsigned char detectedPlayerIndex;    // index of player that was detected

	// for tank2
	unsigned int timetillnextshot;
	unsigned int firetimes;
	unsigned int timetillcanfire;
	unsigned int timetillcanfirecauseonsamelevel;
	unsigned int turnaroundtimer;
	int pausetime;
} stTankData;


#endif /* TANKEP2_H_ */
