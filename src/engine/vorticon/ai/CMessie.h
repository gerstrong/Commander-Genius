/*
 * CMessie.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CMESSIE_H_
#define CMESSIE_H_

#include "engine/vorticon/CVorticonSpriteObject.h"
#include "common/CPlayer.h"
#include "engine/CEvent.h"



// World Map Code for Nessie
const int NESSIE_PATH = 8192;
const int NESSIE_WEED = 8448;
const int NESSIE_LAND = 8704;

const unsigned int NESSIETRAILLEN = 5;

// Forward Declaration
class CPlayer;

class CMessie : public CVorticonSpriteObject
{
public:     
    
    // Constructor
	CMessie(CMap *p_map, Uint32 x, Uint32 y);
	void getTouchedBy(CVorticonSpriteObject &theObject);
	bool tryMounting(CPlayer &player);
	bool tryToUnmount();
	void process();
	void move_nessie();
	void nessie_find_next_checkpoint();
	void processFalling() {}
	
private:
	char state;

	unsigned int baseframe;

	unsigned int tiletrailX[NESSIETRAILLEN+1];
	unsigned int tiletrailY[NESSIETRAILLEN+1];
	unsigned int tiletrailhead;

	char animframe, animtimer;
	unsigned int destx, desty;

	unsigned int pausetimer;
	unsigned int pausex, pausey;

	unsigned int mortimer_swim_amt;
	
	CPlayer *mounted;
};

#endif /* CMESSIE_H_ */
