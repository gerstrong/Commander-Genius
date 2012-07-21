/*
 * direction.h
 *
 *  Created on: 29.05.2011
 *      Author: gerstrong
 *
 *  Small file which only has the enum for the possible directions in the game.
 *  It is mainly used by CSpriteObject but in some occasions it can be used by other classes or functions
 */

#ifndef DIRECTION_H_
#define DIRECTION_H_


enum direction_t
{
	NONE = 0,
	RIGHT = 1,
	LEFT = -1,
	UP = -1,
	DOWN = 1
};

#endif /* DIRECTION_H_ */
