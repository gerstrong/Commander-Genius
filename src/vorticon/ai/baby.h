/*
 * baby.h
 *
 *  Created on: 26.07.2009
 *      Author: gerstrong
 */

#ifndef BABY_H_
#define BABY_H_

/* located in game.cpp */
void freezeplayer(int theplayer);

/* located in misc.cpp */
unsigned int rnd(void);

/* located in ai/baby.cpp */
void baby_ai(int o, int episode, bool hard);
void baby_jump(int o, int big);

#endif /* BABY_H_ */
