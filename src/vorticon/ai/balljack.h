/*
 * balljack.h
 *
 *  Created on: 27.07.2009
 *      Author: gerstrong
 */

#ifndef BALLJACK_H_
#define BALLJACK_H_

/* located in game.cpp */

//-------------------[referenced from ai/balljack.cpp]-----------------//
void bumpplayer(int p, int pushamt, bool solid);
void killplayer(int theplayer);


/* located in map.cpp */

//-------------------[referenced from ai/balljack.cpp]-----------------//
unsigned int getmaptileat(unsigned int x, unsigned int y);
unsigned int getlevelat(unsigned int xpx, unsigned int ypx);


/* located in misc.cpp */

//-------------------[referenced from ai/balljack.cpp]-----------------//
unsigned int rnd(void);


/* located in ai/balljack.c */

//-------------------[referenced from ai/balljack.cpp]-----------------//
void ballandjack_ai(int o);
char BJ_BlockedD(int o);

#endif /* BALLJACK_H_ */
