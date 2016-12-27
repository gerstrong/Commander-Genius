/*
 * CEnemyShot.h
 *
 *  Created on: 01.08.2011
 *      Author: gerstrong
 *
 * This class will process all the enemy shots which kill Keen and go straight one direction
 * Those can be sprite shots in Keen 4 for example.
 */

#ifndef CENEMYSHOT_H_
#define CENEMYSHOT_H_

#include "../CGalaxySpriteObject.h"

namespace galaxy {

class CEnemyShot : public CGalaxySpriteObject
{
public:
    CEnemyShot(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
            const int actionFormatOffset, const int xDir, const int yDir, const float speed, const int srpVar);

	void getTouchedBy(CSpriteObject &theObject);

	void process();

private:
    float m_speedF = 0.0;
    float m_speedInt = 0.0;
};

} /* namespace galaxy */
#endif /* CENEMYSHOT_H_ */
