/*
 * CEarthChunk.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CEARTHCHUNK_H_
#define CEARTHCHUNK_H_

// directions for OBJ_EARTHCHUNK
enum st_ec_direction{
EC_UPLEFTLEFT = 0,                    // 22 degrees CC of UP/LEFT
EC_UPUPLEFT,                      // 22 degrees C of UP/LEFT
EC_UP,                            // straight UP
EC_UPUPRIGHT,                     // 22 degrees CC of UP/RIGHT
EC_UPRIGHTRIGHT,                  // 22 degrees C of UP/RIGHT
EC_DOWNLEFTLEFT,                  // 22 degrees CC of DOWN/LEFT
EC_DOWNDOWNLEFT,                  // 22 degrees C of DOWN/LEFT
EC_DOWN,                          // straight DOWN
EC_DOWNDOWNRIGHT,                 // 22 degrees CC of DOWN/RIGHT
EC_DOWNRIGHTRIGHT,                // 22 degrees C of DOWN/RIGHT
EC_UPLEFT,
EC_UPRIGHT,
EC_DOWNLEFT,
EC_DOWNRIGHT
};

#include "../CVorticonSpriteObject.h"

class CEarthChunk : public CVorticonSpriteObject
{
public:
	CEarthChunk(CMap *pmap, Uint32 x, Uint32 y);
	void process();

	st_ec_direction m_Direction;

private:

};


#endif /* CEARTHCHUNK_H_ */
