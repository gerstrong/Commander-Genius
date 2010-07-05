/*
 * CDoor.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CDOOR_H_
#define CDOOR_H_

// "AI" for the door object (to do the animation when a door
// opens, the door tiles are removed and replaced with a sprite
// that looks exactly the old tiles, which then moves down into
// the floor)

#define DOOR_OPEN_SPEED    1


class CDoor : CObject
{
public:
	CDoor();
	void process();

private:
	char timer;
};

#endif /* CDOOR_H_ */
