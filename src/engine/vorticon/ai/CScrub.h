/*
 * CScrub.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CSCRUB_H_
#define CSCRUB_H_

#include "engine/vorticon/ai/CCarrier.h"

class CScrub : public CCarrier
{
public:
	CScrub(CMap *p_map, Uint32 x, Uint32 y);
	
	void process();
    
private:
    
	void walkLeft(int mx, int my);
	void walkDown();
	void walkRight(int mx, int my);
	void walkUp();
	void fall();
	void preparetoFall();
    
	unsigned char state;
    
	unsigned char animtimer, dietimer;
	unsigned char walkframe;
	int scrubdie_inertia_y;
	int fallspeed;
};

#endif /* CSCRUB_H_ */
